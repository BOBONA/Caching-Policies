#pragma once

#include <rocksdb/db.h>
#include <rocksdb/iostats_context.h>
#include <rocksdb/options.h>
#include <rocksdb/perf_context.h>
#include <rocksdb/table.h>

#include <chrono>
#include <condition_variable>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <thread>

#include "config_options.h"

#include "ASSERT_message.h"

#include <rocksdb/statistics.h>

inline std::mutex mtx;
inline std::condition_variable cv;
inline bool compaction_complete = false;

/** Forward declaration */
void PrintExperimentalSetup(const DBEnv& env);

/** Notifies the cv condition when a compaction is completed. */
class CompactionsListener final : public EventListener {
public:
  explicit CompactionsListener() = default;

  void OnCompactionCompleted(DB *db, const CompactionJobInfo &ci) override {
    std::lock_guard lock(mtx);
    compaction_complete = true;
    cv.notify_one();
  }
};

/** Waits for all compactions to complete. */
inline void WaitForCompactions(DB *db) {
  std::unique_lock lock(mtx);
  uint64_t num_running_compactions;
  uint64_t pending_compaction_bytes;
  uint64_t num_pending_compactions;

  while (!compaction_complete) {
    db->GetIntProperty("rocksdb.num-running-compactions", &num_running_compactions);
    db->GetIntProperty("rocksdb.estimate-pending-compaction-bytes", &pending_compaction_bytes);
    db->GetIntProperty("rocksdb.compaction-pending", &num_pending_compactions);

    if (num_running_compactions == 0 && pending_compaction_bytes == 0 && num_pending_compactions == 0) {
      break;
    }

    cv.wait_for(lock, std::chrono::seconds(2));
  }
}

/** Runs the workload specified in the workload.txt file. */
inline bool RunWorkload(DBEnv& env) {
  Options options;
  WriteOptions write_options;
  ReadOptions read_options;
  BlockBasedTableOptions table_options;

  configureOptions(env, options);
  configureTableOptions(env, table_options);
  configureWriteOptions(env, write_options);
  configureReadOptions(env, read_options);

  options.table_factory.reset(NewBlockBasedTableFactory(table_options));

  if (env.destroy_database) {
    std::cout << "Destroying database..." << std::endl;
    DestroyDB(env.db_path, options);
  }

  if (env.clear_system_cache) {
    std::cout << "Clearing system cache..." << std::endl;
    system("sudo sh -c 'echo 3 >/proc/sys/vm/drop_caches'");
  }

  if (env.enable_perf_iostat) {
    SetPerfLevel(kEnableTimeAndCPUTimeExceptForMutex);
    get_perf_context()->Reset();
    get_perf_context()->ClearPerLevelPerfContext();
    get_perf_context()->EnablePerLevelPerfContext();
    get_iostats_context()->Reset();
    options.statistics = CreateDBStatistics();
  }

  PrintExperimentalSetup(env);

  auto compaction_listener = std::make_shared<CompactionsListener>();
  options.listeners.emplace_back(compaction_listener);

  DB* db;
  Status s = DB::Open(options, env.db_path, &db);
  ASSERT(s.ok(), s.ToString());

  std::ifstream workload_file(env.workload_file_path);
  ASSERT(workload_file.is_open(), "Failed to open workload file " + env.workload_file_path);

  auto it = db->NewIterator(read_options);
  int line_num = 1;
  char instruction;
  while (workload_file >> instruction) {
    std::string key, start_key, end_key, value;

    // Print progress
    if (line_num % env.log_interval == 0) {
      std::cout << "#" << std::flush;
    }

    switch (instruction) {
      case 'I':  // Insert
      case 'U':  // Update
        workload_file >> key >> value;
        s = db->Put(write_options, key, value);
        ASSERT(s.ok(), s.ToString() + " \nWorkload line: " + std::to_string(line_num));
        break;

      case 'D':  // Delete
        workload_file >> key;
        s = db->Delete(write_options, key);
        ASSERT(s.ok(), s.ToString() + " \nWorkload line: " + std::to_string(line_num));
        break;

      case 'Q':  // Query
        workload_file >> key;
        s = db->Get(read_options, key, &value);
        ASSERT(s.ok(), s.ToString() + " \nWorkload line: " + std::to_string(line_num));
        break;

      case 'S':  // Scan
        workload_file >> start_key >> end_key;
        it->Refresh();
        ASSERT(it->status().ok(), it->status().ToString() + " \nWorkload line: " + std::to_string(line_num));

        for (it->Seek(start_key); it->Valid(); it->Next()) {
          if (it->key().ToString() >= end_key) {
            break;
          }
        }

        ASSERT(it->status().ok(), it->status().ToString() + " \nWorkload line: " + std::to_string(line_num));

        break;

      default:
        std::cerr << "ERROR: Unknown workload instruction. Workload line: " << line_num << std::endl;
        break;
    }

    line_num++;
  }

  delete it;
  workload_file.close();

  std::vector<std::string> live_files;
  uint64_t manifest_size;
  db->GetLiveFiles(live_files, &manifest_size, true);
  WaitForCompactions(db);

  s = db->Close();
  ASSERT(s.ok(), s.ToString());

  std::cout << " End of experiment - TEST!!" << std::endl;

  if (env.enable_perf_iostat) {
    std::ofstream output_file(env.output_file_path, std::ios::out | std::ios::trunc);
    ASSERT(output_file.is_open(), "Failed to open output file " + env.output_file_path);
    output_file << get_perf_context()->ToString() << std::endl;
    output_file << std::endl;
    output_file << get_iostats_context()->ToString() << std::endl;
    output_file << std::endl;
    output_file << options.statistics->ToString();

    std::cout << "Results written to " << env.output_file_path << std::endl;
  }

  return true;
}

/** Prints the experimental setup to out. */
inline void PrintExperimentalSetup(const DBEnv& env) {
  constexpr int l = 10;
  std::cout << std::setw(l) << "cmpt_sty"
    << std::setw(l) << "cmpt_pri"
    << std::setw(4) << "T"
    << std::setw(l) << "P"
    << std::setw(l) << "B"
    << std::setw(l) << "E"
    << std::setw(l) << "M"
    << std::setw(l) << "L1_size"
    << std::setw(l) << "blk_cch"
    << std::setw(l) << "bpk"
    << "\n";

  std::cout << std::setw(l) << env.compaction_style
    << std::setw(l) << env.compaction_pri
    << std::setw(4) << env.size_ratio
    << std::setw(l) << env.buffer_size_in_pages
    << std::setw(l) << env.entries_per_page
    << std::setw(l) << env.entry_size
    << std::setw(l) << env.GetBufferSize()
    << std::setw(l) << env.GetMaxBytesForLevelBase()
    << std::setw(l) << env.capacity
    << std::setw(l) << env.bits_per_key
    << std::endl;
}
