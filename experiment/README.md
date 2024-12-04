### Effects of the Cache Policy
We want to test performance over a range of cache policies and beta distributions. We'd like to 
test the effects of the cache size as well as a range of beta distributions. 

Additionally, we'd like to test some of the other cache settings against the beta distributions.

### Effects of Compactions
We first clarify,
1. Does RocksDB use the cache during compaction?
2. What does RocksDB do to the cache during compaction?

We'd also like to confirm that there are latency spikes after compactions. We can do this by measuring 
performance query by query, while setting compactions to high priority.