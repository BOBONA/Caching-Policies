import os
import json
import matplotlib.pyplot as plt
from matplotlib.cm import get_cmap
from matplotlib.colors import Normalize
from collections import defaultdict
import numpy as np

directory = "../experiment1_skew_over_bb"
results = defaultdict(lambda: defaultdict(float))

for filename in os.listdir(directory):
    if filename.endswith(".json"):
        parts = filename.split("_")
        if "zipf" in filename:
            skew = parts[1].replace("zipf_", "") # get skew
        else:
            skew = "uniform"
        bb = float(parts[-1].replace("bb-", "").replace(".json", ""))  # get block cache size

        with open(os.path.join(directory, filename), 'r') as file:
            data = json.load(file)
            performance = data["performance"]

            # sum block cache fore levels
            metric = sum(performance.get("block_cache_hit_count", []))
            results[skew][bb] = metric

# create a colormap for easy differentiation of skew trend
num_skews = len(results)
cmap = get_cmap("copper")
norm = Normalize(vmin=0, vmax=num_skews - 1)
# plot
plt.figure(figsize=(12, 8))

for idx, (skew, cache_data) in enumerate(sorted(results.items())): 
    x = np.array(sorted(cache_data.keys()))  # block cache size
    y = np.array([cache_data[size] for size in x])  # sum of hits
    color = cmap(norm(idx))
    plt.plot(x, y, marker='o', label=f"Skew: {skew}", color=color, linewidth=2)

plt.xlabel("Block Cache Size")
plt.ylabel("Aggregated Block Cache Hit Count")
plt.title("Skew vs Block Cache Size with Gradient-Colored Lines")
plt.legend(title="Skewness", loc="best")
plt.grid()
plt.tight_layout()
plt.show()
