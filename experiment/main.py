import os
import subprocess

import jsonpickle
from tqdm import tqdm

from statistics import parse_output

subprocess.run(['cmake', '--build', '/cmake-build-debug', '--target', 'working_version', '-j', '14'], cwd='../')

workload = 'write_heavy.txt'
with open(f'../workloads/{workload}', 'r') as f:
    num_lines = len(f.readlines())
num_logs = 100
log_interval = num_lines // num_logs

output_dir = 'output'
if not os.path.exists(output_dir):
    os.makedirs(output_dir)
output_file = 'write_heavy.txt'
run_command = ['../bin/working_version', '-w', f'../workloads/{workload}', '--interval', f'{log_interval}', '-o', f'{output_dir}/{output_file}']

with tqdm(total=num_logs) as pbar:
    with subprocess.Popen(run_command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, bufsize=1) as process:
        for char in iter(lambda: process.stdout.read(1), ''):
            if char == '#':
                pbar.update(1)
        process.wait()

statistics = parse_output(f'{output_dir}/{output_file}')
os.remove(f'{output_dir}/{output_file}')
with open(f'{output_dir}/{output_file}.json', 'w') as f:
    f.write(jsonpickle.encode(statistics))
