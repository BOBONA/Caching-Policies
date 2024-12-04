import os
import subprocess

import jsonpickle
from tqdm import tqdm

from experiment.statistics import parse_output, RocksDBStatistics


def run_workload(workload: str, output_file: str | None = None, additional_args: list | None = None, progress_bar: bool=True) -> RocksDBStatistics:
    """
    Run a workload and return the parsed statistics.

    :param workload: The workload file to run.
    :param output_file: The file to write the statistics to (if not None).
    :param additional_args: Additional arguments to pass to the workload running program.
    :param progress_bar: Whether to show a progress bar.
    :return: The parsed statistics.
    """

    with open(f'../workloads/{workload}', 'r') as f:
        num_lines = len(f.readlines())
    num_logs = 100
    log_interval = num_lines // num_logs

    out_dir = os.path.dirname(output_file)
    if out_dir and not os.path.exists(out_dir):
        os.makedirs(out_dir)
    run_command = ['../bin/working_version', '-w', f'../workloads/{workload}', '--interval', f'{log_interval}',
                   '-o', output_file] + (additional_args if additional_args else [])

    print()
    with tqdm(total=num_logs, desc='Running workload', disable=not progress_bar) as pbar:
        with subprocess.Popen(run_command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, bufsize=1) as process:
            if progress_bar:
                for char in iter(lambda: process.stdout.read(1), ''):
                    if char == '#':
                        pbar.update(1)
            process.wait()

    statistics = parse_output(output_file)
    os.remove(output_file)
    if output_file is not None:
        with open(output_file, 'w') as f:
            f.write(jsonpickle.encode(statistics))

    return statistics
