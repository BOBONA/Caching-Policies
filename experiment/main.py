from experiment.run_workload import run_workload


workload = 'write_heavy_small.txt'
output_file = 'output/results.json'

run_workload(workload, output_file)
