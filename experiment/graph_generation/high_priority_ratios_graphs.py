import os
import json
import matplotlib.pyplot as plt

directory = "experiment/experiment4_high_priority_ratios"

flushed_or_similar_data = {}
none_data = {}

for filename in os.listdir(directory):
    if filename.endswith(".json"):
        parts = filename.split("_")
        high_pri = float(parts[1].replace("pri-", ""))  #Get priority ration
        policy = parts[2].replace("pin-k", "") #Get policy name
        bb = float(parts[-1].replace("bb-", "").replace(".json", ""))  #Gets block cache size
        
        with open(os.path.join(directory, filename), "r") as f:
            content = json.load(f)
            metric = sum(content.get("performance", {}).get("block_cache_hit_count", []))
            #metric = content.get("performance", {}).get("block_read_time", [])
        
        
        # Adds data based on policy
        if policy == "FlushedOrSimilar":
            if high_pri not in flushed_or_similar_data:
                flushed_or_similar_data[high_pri] = []
            flushed_or_similar_data[high_pri].append((bb, metric))
        elif policy == "None":
            if high_pri not in none_data:
                none_data[high_pri] = []
            none_data[high_pri].append((bb, metric))

# Sort the data by block size
for high_pri, values in flushed_or_similar_data.items():
    flushed_or_similar_data[high_pri] = sorted(values, key=lambda x: x[0])
for high_pri, values in none_data.items():
    none_data[high_pri] = sorted(values, key=lambda x: x[0])

# Plot for FlushedOrSimilar policy
plt.figure(figsize=(10, 6))
for high_pri, values in flushed_or_similar_data.items():
    x, y = zip(*values)
    plt.plot(x, y, marker='o', label=f"HighPri={high_pri}")
plt.title("Block Cache Hit Count (FlushedOrSimilar Policy)")
plt.xlabel("BB Value")
plt.ylabel("Block Cache Hit Count")
plt.legend(title="High Priority Ratio")
plt.grid()
plt.tight_layout()
plt.show()

# Plot for having None policy (having no policy)
plt.figure(figsize=(10, 6))
for high_pri, values in none_data.items():
    x, y = zip(*values)
    plt.plot(x, y, marker='o', label=f"HighPri={high_pri}")
plt.title("Block Cache Hit Count (None Policy)")
plt.xlabel("BB Value")
plt.ylabel("Block Cache Hit Count")
plt.legend(title="High Priority Ratio")
plt.grid()
plt.tight_layout()
plt.show()
