#pragma once
#include <iostream>

#include "args.hxx"
#include "db_env.h"

/** Parses the command line arguments and updates the DBEnv object accordingly. */
inline void ParseArguments(const int argc, char *argv[], DBEnv& env) {
  args::ArgumentParser parser("RocksDB_parser.", "");
  args::Group group(parser, "This group is all exclusive: ", args::Group::Validators::DontCare);

  args::ValueFlag<std::string> workload_file(group, "workload", "The workload file to run [default: workload.txt]",
    {'w', "workload"});
  args::ValueFlag<std::string> output_file(group, "output", "The output file to write results to [default: output.txt]",
    {'o', "output"});
  args::ValueFlag<std::string> db_path(group, "path", "The path to the database [default: ./db]",
    {"path"});
  args::ValueFlag<int> log_interval_cmd(group, "interval", "The interval at which to log [default: 100000]",
    {"interval"});
  args::ValueFlag<int> destroy_database_cmd(group, "d", "Destroy and recreate the database [def: 1]",
    {'d', "destroy"});
  args::ValueFlag<int> clear_system_cache_cmd(group, "cc", "Clear system cache [def: 1]",
    {"cc"});
  args::ValueFlag<int> enable_perf_iostat_cmd(group, "enable_perf_iostat", "Enable RocksDB's internal Perf and IOstat [default: 1]",
    {"stat"});

  args::ValueFlag<int> size_ratio_cmd(group, "T", "The size ratio for the LSM [default: 10]",
    {'T', "size_ratio"});
  args::ValueFlag<int> buffer_size_in_pages_cmd(group, "P", "The number of pages in memory buffer [default: 4096]",
    {'P', "buffer_size_in_pages"});
  args::ValueFlag<int> entries_per_page_cmd(group, "B", "The number of entries in one page [default: 4]",
    {'B', "entries_per_page"});
  args::ValueFlag<int> entry_size_cmd(group, "E", "The size of one entry you have in workload.txt [default: 1024 B]",
    {'E', "entry_size"});
  args::ValueFlag<long> buffer_size_cmd(group, "M", "Overrides the calculated buffer size [default: 0 B]",
    {'M', "memory_size"});
  args::ValueFlag<int> file_to_memtable_size_ratio_cmd(group, "file_to_memtable_size_ratio", "The ratio between files and memtable [default: 1]",
    {'f', "file_to_memtable_size_ratio"});

  args::ValueFlag<int> compaction_pri_cmd(group, "compaction_pri", "Compaction priority [1: kMinOverlappingRatio, 2: kByCompensatedSize, 3: kOldestLargestSeqFirst, 4: kOldestSmallestSeqFirst; default: 1]",
    {'c', "compaction_pri"});
  args::ValueFlag<int> compaction_style_cmd(group, "compaction_style", "Compaction priority [1: kCompactionStyleLevel, 2: kCompactionStyleUniversal, 3: kCompactionStyleFIFO, 4: kCompactionStyleNone; default: 1]",
    {'C', "compaction_style"});
  args::ValueFlag<int> bits_per_key_cmd(group, "bits_per_key", "The number of bits per key assigned to Bloom filter [default: 10]",
    {'b', "bits_per_key"});

  args::ValueFlag<int> block_cache_cmd(group, "bb", "Block cache size in MB [default: 32 MB]",
    {"bb"});
  args::ValueFlag<int> strict_capacity_limit_cmd(group, "bb_strict", "Strict capacity limit [default: 1]",
    {"bb_strict"});
  args::ValueFlag<int> cache_metadata_with_high_priority_cmd(group, "cache_metadata_with_high_priority", "Cache metadata with high priority [default: 1]",
    {"cache_metadata_high_pri"});
  args::ValueFlag<int> metadata_pinning_cmd(group, "metadata_pinning", "Metadata pinning [1: kNone, 2: kFlushedAndSimilar, 3: kAll; default: 1]",
    {"metadata_pinning"});
  args::ValueFlag<float> cache_high_priority_ratio_cmd(group, "cache_high_priority_ratio", "Cache high priority ratio [default: 0.5]",
    {"cache_high_priority_ratio"});

  parser.ParseCLI(argc, argv);

  if (workload_file)
    env.workload_file_path = get(workload_file);

  if (output_file)
    env.output_file_path = get(output_file);

  if (db_path)
    env.db_path = get(db_path);

  if (log_interval_cmd)
    env.log_interval = get(log_interval_cmd);

  if (destroy_database_cmd)
    env.destroy_database = get(destroy_database_cmd);

  if (clear_system_cache_cmd)
    env.clear_system_cache = get(clear_system_cache_cmd);

  if (enable_perf_iostat_cmd)
    env.enable_perf_iostat = get(enable_perf_iostat_cmd);

  if (size_ratio_cmd)
    env.size_ratio = get(size_ratio_cmd);

  if (buffer_size_in_pages_cmd)
    env.buffer_size_in_pages = get(buffer_size_in_pages_cmd);

  if (entries_per_page_cmd)
    env.entries_per_page = get(entries_per_page_cmd);

  if (entry_size_cmd)
    env.entry_size = get(entry_size_cmd);

  if (buffer_size_cmd)
    env.buffer_size = get(buffer_size_cmd);

  if (file_to_memtable_size_ratio_cmd)
    env.file_to_memtable_size_ratio = get(file_to_memtable_size_ratio_cmd);

  constexpr rocksdb::CompactionPri compaction_priorities[5] = {rocksdb::kMinOverlappingRatio, rocksdb::kByCompensatedSize,
    rocksdb::kOldestLargestSeqFirst, rocksdb::kOldestSmallestSeqFirst, rocksdb::kRoundRobin};

  constexpr rocksdb::CompactionStyle compaction_styles[4] = {rocksdb::kCompactionStyleLevel, rocksdb::kCompactionStyleUniversal,
    rocksdb::kCompactionStyleFIFO, rocksdb::kCompactionStyleNone};

  if (compaction_pri_cmd)
    env.compaction_pri = compaction_priorities[get(compaction_pri_cmd) - 1];

  if (compaction_style_cmd)
    env.compaction_style = compaction_styles[get(compaction_style_cmd) - 1];

  if (bits_per_key_cmd)
    env.bits_per_key = get(bits_per_key_cmd);

  if (block_cache_cmd)
    env.capacity = get(block_cache_cmd) * 1024 * 1024;

  if (strict_capacity_limit_cmd)
    env.strict_capacity_limit = get(strict_capacity_limit_cmd);

  if (cache_metadata_with_high_priority_cmd)
    env.cache_index_and_filter_blocks_with_high_priority = get(cache_metadata_with_high_priority_cmd);

  constexpr rocksdb::PinningTier metadata_pinning_tiers[3] = {rocksdb::PinningTier::kNone, rocksdb::PinningTier::kFlushedAndSimilar,
    rocksdb::PinningTier::kAll};
  if (metadata_pinning_cmd)
    env.unpartitioned_pinning = metadata_pinning_tiers[get(metadata_pinning_cmd) - 1];

  if (cache_high_priority_ratio_cmd)
    env.cache_high_priority_ratio = get(cache_high_priority_ratio_cmd);
}
