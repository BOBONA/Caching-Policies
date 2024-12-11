import os
import json
import matplotlib.pyplot as plt
from matplotlib.cm import get_cmap
from matplotlib.colors import Normalize
from collections import defaultdict
import numpy as np

directory = "experiment/experiment1_skew_over_bb"
results = defaultdict(lambda: defaultdict(float))

for filename in os.listdir(directory):
    if filename.endswith(".json"):
        parts = filename.split("_")
        if "zipf" in filename:
            skew = parts[1].replace("zipf_", "").replace(".txt", "")  # Get skew from filename
        else:
            skew = "uniform"
        bb = float(parts[-1].replace("bb-", "").replace(".json", ""))  # Get block cache size from filename

        with open(os.path.join(directory, filename), 'r') as file:
            data = json.load(file)
            performance = data["performance"]

            # Sum block cache for levels
            metric = sum(performance.get("block_cache_hit_count", []))
            results[skew][bb] = metric

# Makes uniform the first key so that the keys line up with the results
def sort_key(item):
    skew = item[0]
    if skew == "uniform":
        return (-1, "uniform") 
    try:
        return (0, float(skew))
    except ValueError:
        return (1, skew) 

sorted_results = sorted(results.items(), key=sort_key)

# Create a colormap for easy differentiation of skew trends
num_skews = len(results)
cmap = get_cmap("copper")
norm = Normalize(vmin=0, vmax=num_skews - 1)

# Plot
plt.figure(figsize=(12, 8))

for idx, (skew, cache_data) in enumerate(sorted_results):
    x = np.array(sorted(cache_data.keys()))  # block cache size
    y = np.array([cache_data[size] for size in x])  # sum of hits

    slopes = np.gradient(y, x)  # Get the derivative

    color = cmap(norm(idx))
    plt.plot(x, slopes, marker='o', label=f"Skew: {skew}", color=color, linewidth=2)

plt.xlabel("Block Cache Size")
plt.ylabel("Slope of Block Cache Hit Count")
plt.title("Slope of Skew vs Block Cache Size (Block Cache Hits)")
plt.legend(title="Skewness", loc="best")
plt.grid()
plt.tight_layout()
plt.show()
