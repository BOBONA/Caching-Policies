#pragma once
#include <iostream>

#include "args.hxx"
#include "db_env.h"

inline int parse_arguments(int argc, char *argv[], DBEnv *env) {
  args::ArgumentParser parser("RocksDB_parser.", "");
  args::Group group1(parser, "This group is all exclusive:",
                     args::Group::Validators::DontCare);

  args::ValueFlag<int> destroy_database_cmd(
      group1, "d", "Destroy and recreate the database [def: 1]",
      {'d', "destroy"});
  args::ValueFlag<int> clear_system_cache_cmd(
      group1, "cc", "Clear system cache [def: 1]",
      {"cc"});
  args::ValueFlag<int> size_ratio_cmd(
      group1, "T",
      "The size ratio for the LSM [default: 10]",
      {'T', "size_ratio"});
  args::ValueFlag<int> buffer_size_in_pages_cmd(
      group1, "P",
      "The number of pages in memory buffer [default: 4096]",
      {'P', "buffer_size_in_pages"});
  args::ValueFlag<int> entries_per_page_cmd(
      group1, "B",
      "The number of entries in one page [default: 4]",
      {'B', "entries_per_page"});
  args::ValueFlag<int> entry_size_cmd(
      group1, "E",
      "The size of one entry you have in workload.txt [default: 1024 B]",
      {'E', "entry_size"});
  args::ValueFlag<long> buffer_size_cmd(
      group1, "M",
      "The memory buffer size in bytes [default: 16 MB]",
      {'M', "memory_size"});
  args::ValueFlag<int> file_to_memtable_size_ratio_cmd(
      group1, "file_to_memtable_size_ratio",
      "The ratio between files and memtable [default: 1]",
      {'f', "file_to_memtable_size_ratio"});
  args::ValueFlag<long> file_size_cmd(
      group1, "file_size",
      "The size of one SST file [default: 256 KB]",
      {'F', "file_size"});
  args::ValueFlag<int> verbosity_cmd(
      group1, "verbosity", "The verbosity level of execution [0,1,2; def: 0]",
      {'V', "verbosity"});
  args::ValueFlag<int> compaction_pri_cmd(
      group1, "compaction_pri",
      "Compaction priority [1: kMinOverlappingRatio, 2: kByCompensatedSize, "
      "3: kOldestLargestSeqFirst, 4: kOldestSmallestSeqFirst; default: 1]",
      {'c', "compaction_pri"});
  args::ValueFlag<int> compaction_style_cmd(
      group1, "compaction_style",
      "Compaction priority [1: kCompactionStyleLevel, 2: kCompactionStyleUniversal, "
      "3: kCompactionStyleFIFO, 4: kCompactionStyleNone; default: 1]",
      {'C', "compaction_style"});
  args::ValueFlag<int> bits_per_key_cmd(
      group1, "bits_per_key",
      "The number of bits per key assigned to Bloom filter [default: 10]",
      {'b', "bits_per_key"});
  args::ValueFlag<int> block_cache_cmd(
      group1, "bb", "Block cache size in MB [default: 8 MB]", {"bb"});
  args::ValueFlag<int> enable_perf_iostat_cmd(
      group1, "enable_perf_iostat",
      "Enable RocksDB's internal Perf and IOstat [default: 0]", {"stat"});

  try {
    parser.ParseCLI(argc, argv);
  } catch (args::Help &) {
    std::cout << parser;
    exit(0);
  } catch (args::ParseError &e) {
    std::cerr << e.what() << std::endl;
    std::cerr << parser;
    return 1;
  } catch (args::ValidationError &e) {
    std::cerr << e.what() << std::endl;
    std::cerr << parser;
    return 1;
  }

  env->SetDestroyDatabase(destroy_database_cmd
                              ? args::get(destroy_database_cmd)
                              : env->IsDestroyDatabaseEnabled());
  env->clear_system_cache = clear_system_cache_cmd
                                ? args::get(clear_system_cache_cmd)
                                : env->clear_system_cache;
  env->size_ratio =
      size_ratio_cmd ? args::get(size_ratio_cmd) : env->size_ratio;
  env->buffer_size_in_pages = buffer_size_in_pages_cmd
                                  ? args::get(buffer_size_in_pages_cmd)
                                  : env->buffer_size_in_pages;
  env->entries_per_page = entries_per_page_cmd ? args::get(entries_per_page_cmd)
                                               : env->entries_per_page;
  env->entry_size =
      entry_size_cmd ? args::get(entry_size_cmd) : env->entry_size;
  env->SetBufferSize(buffer_size_cmd ? args::get(buffer_size_cmd) : 0);
  env->file_to_memtable_size_ratio =
      file_to_memtable_size_ratio_cmd
          ? args::get(file_to_memtable_size_ratio_cmd)
          : env->file_to_memtable_size_ratio;
  env->compaction_pri =
      compaction_pri_cmd ? args::get(compaction_pri_cmd) : env->compaction_pri;
  env->compaction_style = compaction_style_cmd ? args::get(compaction_style_cmd)
                                               : env->compaction_style;
  env->bits_per_key =
      bits_per_key_cmd ? args::get(bits_per_key_cmd) : env->bits_per_key;
  env->block_cache =
      block_cache_cmd ? args::get(block_cache_cmd) : env->block_cache;
  env->SetPerfIOStat(enable_perf_iostat_cmd ? args::get(enable_perf_iostat_cmd)
                                            : env->IsPerfIOStatEnabled());
  return 0;
}
