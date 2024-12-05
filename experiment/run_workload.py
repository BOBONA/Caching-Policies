import os
import shutil
import subprocess

import jsonpickle
from tqdm import tqdm

from experiment.generate_workloads import VALUE_SIZE, KEY_SIZE, PAGE_SIZE
from experiment.statistics import parse_output, RocksDBStatistics


def run_workload(workload: str, path: str, output_file: str | None = None, additional_args: list | None = None, progress_bar: bool=True) -> RocksDBStatistics | None:
    """
    Run a workload and return the parsed statistics.

    :param workload: The workload file to run.
    :param path: The path for the database.
    :param output_file: The file to write the statistics to (if not None).
    :param additional_args: Additional arguments to pass to the workload running program.
    :param progress_bar: Whether to show a progress bar.
    :return: The parsed statistics or None if something went wrong.
    """

    with open(workload, 'r') as f:
        num_lines = len(f.readlines())
    num_logs = 100
    log_interval = num_lines // num_logs

    out_dir = os.path.dirname(output_file)
    if out_dir and not os.path.exists(out_dir):
        os.makedirs(out_dir)
    if not os.path.exists(path):
        os.makedirs(path)

    run_command = ['../bin/working_version', '--path', path, '-w', workload,
                   '--interval', f'{log_interval}', '-E', str(VALUE_SIZE + KEY_SIZE),
                   '-B', str(round(PAGE_SIZE / (PAGE_SIZE + VALUE_SIZE))),
                   '-o', output_file] + (additional_args if additional_args else [])

    print(f'\nRunning {workload} on {path}')
    with tqdm(total=num_logs, desc=f'Running workload', disable=not progress_bar) as pbar:
        output = ''
        with subprocess.Popen(run_command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, bufsize=1) as process:
            if progress_bar:
                for char in iter(lambda: process.stdout.read(1), ''):
                    output += char
                    if char == '#':
                        pbar.update(1)
                for char in iter(lambda: process.stderr.read(1), ''):
                    output += char
            process.wait()

    if process.returncode != 0 or not os.path.exists(output_file):
        with open(output_file, 'w') as f:
            f.write(output)
        print(f'Error running workload, wrote output to {output_file}')
        return None

    statistics = parse_output(output_file)
    os.remove(output_file)
    if output_file is not None:
        with open(output_file, 'w') as f:
            f.write(jsonpickle.encode(statistics))

        print(f'Wrote statistics to {output_file}')

    return statistics


def run_workload_from_base(base_db: str, new_path: str, workload: str, output_file: str | None = None, additional_args: list | None = None, progress_bar: bool=True) -> RocksDBStatistics:
    """
    Run a workload on a copy of a base database and returns the parsed statistics.

    :param base_db: The base database to copy.
    :param new_path: The path to copy the base database to.
    :param workload: The workload file to run.
    :param output_file: The file to write the statistics to (if not None).
    :param additional_args: Additional arguments to pass to the workload running program.
    :param progress_bar: Whether to show a progress bar.
    :return: The parsed statistics.
    """

    if os.path.exists(new_path):
        shutil.rmtree(new_path)
    shutil.copytree(base_db, new_path)

    return run_workload(workload, new_path, output_file, ['-d', '0'] + additional_args if additional_args else [], progress_bar)