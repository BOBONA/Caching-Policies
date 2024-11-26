#pragma once

#include <rocksdb/advanced_options.h>
#include <rocksdb/table.h>

/** For fields that can be set from the command line, defaults are provided in this namespace */
namespace Default {

  const std::string WORKLOAD_FILE_PATH = "workload.txt";  // [w]
  constexpr bool DESTROY_DATABASE = true; // [d]
  constexpr bool CLEAR_SYSTEM_CACHE = true; // [cc]
  constexpr bool ENABLE_PERF_IOSTAT = true;  // [stat]

  constexpr unsigned int BUFFER_SIZE_IN_PAGES = 4096; // [P]
  constexpr unsigned int ENTRIES_PER_PAGE = 4; // [B]
  constexpr unsigned int ENTRY_SIZE = 1024;  // [E]
  constexpr size_t BUFFER_SIZE = 0; // [M]

  constexpr double SIZE_RATIO = 10;  // [T]
  constexpr unsigned int FILE_TO_MEMTABLE_SIZE_RATIO = 1;  // [f]

  constexpr rocksdb::CompactionPri COMPACT_PRIORITY = rocksdb::kMinOverlappingRatio;  // [c]
  constexpr rocksdb::CompactionStyle COMPACT_STYLE = rocksdb::kCompactionStyleLevel;  // [C]

  constexpr int BLOOM_FILTER_BITS_PER_KEY = 10;  // [b]

  constexpr int BLOCK_CACHE = 8;  // [bb]
}  // namespace Default

/**
 * DBEnv is a wrapper for configuring RocksDB.
 * We expose certain RocksDB options as well was our own options for running workloads.
 * For detailed information, look through the referenced RocksDB files.
 */
class DBEnv {
public:
  [[nodiscard]] uint64_t GetBlockSize() const { return entries_per_page * entry_size; }

  /** buffer_size = num_pages * entries_per_page * entry_size */
  [[nodiscard]] size_t GetBufferSize() const {
    return buffer_size != 0 ? buffer_size : buffer_size_in_pages * entries_per_page * entry_size;
  }

  // Control maximum total data size for level base (i.e. level 1)
  [[nodiscard]] uint64_t GetMaxBytesForLevelBase() const { return GetBufferSize() * size_ratio; }

  //============================================================================
  // Our own configuration options

  /** The path to the workload file */
  std::string workload_file_path = Default::WORKLOAD_FILE_PATH;
  /** Whether to destroy the database on start */
  bool destroy_database = Default::DESTROY_DATABASE;
  /** Whether to clear the system cache on start */
  bool clear_system_cache = Default::CLEAR_SYSTEM_CACHE;
  /** Whether to enable RocksDB's internal Perf and IOstat */
  bool enable_perf_iostat = Default::ENABLE_PERF_IOSTAT;

  unsigned int entry_size = Default::ENTRY_SIZE;
  unsigned int entries_per_page = Default::ENTRIES_PER_PAGE;
  unsigned int buffer_size_in_pages = Default::BUFFER_SIZE_IN_PAGES;
  /** Setting this overrides the calculated buffer size */
  size_t buffer_size = Default::BUFFER_SIZE;  // bytes

  double size_ratio = Default::SIZE_RATIO;
  unsigned int file_to_memtable_size_ratio = Default::FILE_TO_MEMTABLE_SIZE_RATIO;

  double bits_per_key = Default::BLOOM_FILTER_BITS_PER_KEY;

  //============================================================================
  /* See DBOptions in options.h */

  int level0_file_num_compaction_trigger = 4;  // Line 246 in options.h
  bool disable_auto_compactions = false;  // 303

  bool create_if_missing = true;  // 563
  int max_open_files = -1;  // 710
  int max_background_jobs = 1;  // 809

  bool allow_mmap_reads = false;  // 932
  bool allow_mmap_writes = false;  // 937
  bool use_direct_reads = true;  // 949
  bool use_direct_io_for_flush_and_compaction = true;  // 953

  size_t stats_history_buffer_size = 1024 * 1024;  // 995
  bool advise_random_on_open = true;  // 1000
  bool enable_thread_tracking = false;  // 1139
  bool dump_malloc_stats = false;  // 1294

  /* See ColumnFamilyOptions in options.h */

  bool use_default_compression = true;
  rocksdb::CompressionType compression = rocksdb::kNoCompression;  // Line 214 in options.h

  //============================================================================
  /* See AdvancedColumnFamilyOptions in advanced_options.h */

  int num_levels = 10;  // Line 438 in advanced_options.h
  bool level_compaction_dynamic_level_bytes = false;  // 555
  rocksdb::CompactionStyle compaction_style = Default::COMPACT_STYLE;  // 600
  rocksdb::CompactionPri compaction_pri = Default::COMPACT_PRIORITY; // 605
  bool optimize_filters_for_hits = false;  // 696
  bool report_bg_io_stats = true;  // 722
  uint64_t periodic_compaction_seconds = 0;  // 804

  using memtable_factory = rocksdb::SkipListFactory;  // 634

  //============================================================================
  /* See BlockBasedTableOptions in table.h */

  using TableOptions = rocksdb::BlockBasedTableOptions;

  bool cache_index_and_filter_blocks = true; // Line 149 in table.h
  bool cache_index_and_filter_blocks_with_high_priority = true;  // 155

  TableOptions::IndexType index_type = TableOptions::kBinarySearch;  // 237
  TableOptions::DataBlockIndexType data_block_index_type = TableOptions::kDataBlockBinarySearch;  // 245
  bool no_block_cache = false;  // 262

  uint32_t read_amp_bytes_per_bit = 0;  // 491

  bool enable_index_compression = true;  // 534

  /* See MetadataCacheOptions in table.h */

  rocksdb::PinningTier top_level_index_pinning = rocksdb::PinningTier::kAll;  // Line 96 in table.h
  rocksdb::PinningTier partition_pinning = rocksdb::PinningTier::kAll;  // 100
  rocksdb::PinningTier unpartitioned_pinning = rocksdb::PinningTier::kAll;  // 108

  //============================================================================
  /* See ShardedCacheOptions in cache.h */

  /** A capacity of 0 uses RocksDB's default cache */
  int capacity = 1024 * 1024 * Default::BLOCK_CACHE;  // Line 132 in cache.h
  int num_shard_bits = -1;  // 138
  bool strict_capacity_limit = false;  // 145

  /* See LRUCacheOptions in cache.h */

  double cache_high_priority_ratio = 0.5;  // Line 237 in cache.h

  //============================================================================
  /* See ReadOptions in options.h */

  bool verify_checksums = true;  // Line 1704 in options.h
  bool fill_cache = true;  // 1711
  bool ignore_range_deletions = false;  // 1718

  /* See WriteOptions in options.h */

  bool sync = false;  // Line 1900 in options.h
  bool disableWAL = false;  // 1908
  bool no_slowdown = false;  // 1919
};