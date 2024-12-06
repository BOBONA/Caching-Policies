import os
import json
import matplotlib.pyplot as plt

directory = "experiment/experiment3_pinning_policies"
results = {}

for filename in os.listdir(directory):
    if filename.endswith(".json"):
        parts = filename.split("_")
        policy = parts[0] # get policy name
        bb = float(parts[-1].replace("bb-", "").replace(".json", ""))  # get block cache size

        
        try:
            with open(os.path.join(directory, filename), "r") as f:
                content = json.load(f)
                block_cache_hit_count = sum(content.get("performance", {}).get("block_cache_hit_count", []))
                if policy not in results:
                    results[policy] = []
                results[policy].append((bb, block_cache_hit_count))
        except json.JSONDecodeError:
            print(f"Failed {filename}, skipped")

# ensure results has right order
for policy, values in results.items():
    results[policy] = sorted(values, key=lambda x: x[0])

# plot
plt.figure(figsize=(10, 6))
for policy, values in results.items():
    if values:
        x, y = zip(*values)
        plt.plot(x, y, marker='o', label=policy)

plt.title("Block Cache Hit Count across Pinning Policies")
plt.xlabel("BB Value")
plt.ylabel("Block Cache Hit Count")
plt.legend()
plt.grid()
plt.tight_layout()
plt.show()
