import matplotlib.pyplot as plt
from collections import Counter
import os

import numpy as np
from scipy.interpolate import make_interp_spline

workload_path = '../workloads'
workloads = ['insertions.txt', 'uniform.txt', 'zipf_0.10.txt', 'zipf_0.40.txt', 'zipf_0.70.txt', 'zipf_1.00.txt']

def read_keys(file_path):
    keys = []
    with open(os.path.join(workload_path, file_path), 'r') as f:
        for line in f:
            parts = line.strip().split()
            if len(parts) > 1:
                keys.append(parts[1])
    return keys

plt.figure(figsize=(12, 8))

cmap = plt.get_cmap('viridis')

for i, workload in enumerate(workloads):
    keys = read_keys(workload)
    counter = Counter(keys)
    frequencies = sorted(counter.values(), reverse=True)

    x = np.arange(len(frequencies))
    y = np.array(frequencies)

    spline = make_interp_spline(x, y, k=3)
    x_smooth = np.linspace(x.min(), x.max(), 500)
    y_smooth = spline(x_smooth)

    color = cmap(i / len(workloads))

    plt.plot(x_smooth, y_smooth, label=workload, color=color, linewidth=2)

plt.title('Workload Distributions')
plt.xlabel('Key')
plt.ylabel('Frequency')
plt.yscale('log')
plt.legend(loc='upper right')
plt.grid(True)
plt.savefig('workload_distributions.png')
plt.show()
