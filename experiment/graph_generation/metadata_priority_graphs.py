import os
import json
import matplotlib.pyplot as plt
import numpy as np

directory = "experiment/experiment2_metadata_priority_matters"
files = {
    "False": "high_priority-False.json",
    "True": "high_priority-True.json"
}

block_cache_hit_counts = {}

for priority, filename in files.items():
    with open(os.path.join(directory, filename), 'r') as file:
        data = json.load(file)
        block_cache_hit_counts[priority] = data["performance"]["block_cache_hit_count"]

# get hitcounts for levels
levels = [f"Level {i}" for i in range(1, len(block_cache_hit_counts["False"]) + 1)]
hit_counts_false = block_cache_hit_counts["False"]
hit_counts_true = block_cache_hit_counts["True"]

#plotting
x = np.arange(len(levels))
width = 0.35
plt.figure(figsize=(10, 6))
plt.bar(x - width / 2, hit_counts_false, width, label="High Priority: False", color="skyblue")
plt.bar(x + width / 2, hit_counts_true, width, label="High Priority: True", color="salmon")
plt.xlabel("LSM Levels")
plt.ylabel("Block Cache Hit Count")
plt.title("Block Cache Hit Rates Comparison Across Levels")
plt.xticks(x, levels)
plt.legend(loc="best")
plt.grid(axis="y", linestyle="--", alpha=0.7)
plt.tight_layout()
plt.show()
