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
            skew = parts[1].replace("zipf_", "").replace(".txt", "")  #Get zipf from file name
        else:
            skew = "uniform"
        bb = float(parts[-1].replace("bb-", "").replace(".json", ""))  # Get block cache size from file name

        with open(os.path.join(directory, filename), 'r') as file:
            data = json.load(file)
            performance = data["performance"]

            # There was an outlier due to the computer falling asleep so we excluded it from the data
            metric = performance.get("block_read_time", 0)
            if metric <= 127446006701:
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

num_skews = len(results)
cmap = get_cmap("copper")
norm = Normalize(vmin=0, vmax=num_skews - 1)

# Plot
plt.figure(figsize=(12, 8))

for idx, (skew, cache_data) in enumerate(sorted_results):
    x = np.array(sorted(cache_data.keys()))  # Block cache size
    y = np.array([cache_data[size] for size in x])  # Block read time
    color = cmap(norm(idx))
    plt.plot(x, y, marker='o', label=f"Skew: {skew}", color=color, linewidth=2)

plt.xlabel("Block Cache Size")
plt.ylabel("Block Read Time")
plt.title("Skew vs Block Cache Size (Block Read Time)")
plt.legend(title="Skewness", loc="best")
plt.grid()
plt.tight_layout()
plt.show()
