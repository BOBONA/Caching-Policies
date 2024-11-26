#pragma once

#include <rocksdb/filter_policy.h>
#include <rocksdb/options.h>
#include <rocksdb/table.h>

#include <memory>

#include "db_env.h"

using namespace rocksdb;

inline void configureOptions(const DBEnv & env, Options& options) {
  /* DBOptions */

  options.level0_file_num_compaction_trigger = env.level0_file_num_compaction_trigger;
  options.disable_auto_compactions = env.disable_auto_compactions;

  options.create_if_missing = env.create_if_missing;
  options.max_open_files = env.max_open_files;
  options.max_background_jobs = env.max_background_jobs;

  options.allow_mmap_reads = env.allow_mmap_reads;
  options.allow_mmap_writes = env.allow_mmap_writes;
  options.use_direct_reads = env.use_direct_reads;
  options.use_direct_io_for_flush_and_compaction = env.use_direct_io_for_flush_and_compaction;

  options.stats_history_buffer_size = env.stats_history_buffer_size;
  options.advise_random_on_open = env.advise_random_on_open;
  options.enable_thread_tracking = env.enable_thread_tracking;
  options.dump_malloc_stats = env.dump_malloc_stats;

  /* ColumnFamilyOptions */

  if (!env.use_default_compression)
    options.compression = env.compression;
  options.write_buffer_size = env.GetBufferSize();
  options.max_bytes_for_level_base = env.GetMaxBytesForLevelBase();

  /* AdvancedColumnFamilyOptions */

  options.num_levels = env.num_levels;
  options.level_compaction_dynamic_level_bytes = env.level_compaction_dynamic_level_bytes;
  options.compaction_style = env.compaction_style;
  options.compaction_pri = env.compaction_pri;
  options.optimize_filters_for_hits = env.optimize_filters_for_hits;
  options.report_bg_io_stats = env.report_bg_io_stats;
  options.periodic_compaction_seconds = env.periodic_compaction_seconds;

  options.target_file_size_base = env.GetBufferSize();
  options.max_bytes_for_level_multiplier = env.size_ratio;

  options.memtable_factory = std::make_shared<DBEnv::memtable_factory>();
}

inline void configureTableOptions(DBEnv& env, BlockBasedTableOptions& table_options) {
  if (env.bits_per_key > 0) {
    table_options.filter_policy.reset(NewBloomFilterPolicy(env.bits_per_key, false));
  }

  table_options.cache_index_and_filter_blocks = env.cache_index_and_filter_blocks;
  table_options.cache_index_and_filter_blocks_with_high_priority = env.cache_index_and_filter_blocks_with_high_priority;

  table_options.index_type = env.index_type;
  table_options.data_block_index_type = env.data_block_index_type;
  table_options.no_block_cache = env.no_block_cache;

  if (env.capacity > 0) {
    const std::shared_ptr<Cache> cache = NewLRUCache(
      env.capacity, env.num_shard_bits,
      env.strict_capacity_limit, env.cache_high_priority_ratio);
    table_options.block_cache = cache;
  }

  table_options.block_size = env.GetBlockSize();

  table_options.read_amp_bytes_per_bit = env.read_amp_bytes_per_bit;
  table_options.enable_index_compression = env.enable_index_compression;

  MetadataCacheOptions metadata_cache_options;
  metadata_cache_options.top_level_index_pinning = env.top_level_index_pinning;
  metadata_cache_options.partition_pinning = env.partition_pinning;
  metadata_cache_options.unpartitioned_pinning = env.unpartitioned_pinning;
  table_options.metadata_cache_options = metadata_cache_options;
}

inline void configureReadOptions(DBEnv& env, ReadOptions& read_options) {
  read_options.verify_checksums = env.verify_checksums;
  read_options.fill_cache = env.fill_cache;
  read_options.ignore_range_deletions = env.ignore_range_deletions;
}

inline void configureWriteOptions(DBEnv& env, WriteOptions& write_options) {
  write_options.sync = env.sync;
  write_options.disableWAL = env.disableWAL;
  write_options.no_slowdown = env.no_slowdown;
}