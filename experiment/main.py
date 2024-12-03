from experiment.workload import run_workload
from workload import compile_cmd

compile_cmd()

workload = 'write_heavy.txt'
output_file = 'output/write_heavy.txt'

run_workload(workload, output_file)
