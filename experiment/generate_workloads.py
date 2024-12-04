import os
import random
import string
import subprocess

from tqdm import tqdm


ZIPF_ALPHAS = [0.1 * i for i in range(1, 11)]

KEY_SIZE = 8
VALUE_SIZE = 1016

NUM_OPERATIONS = 10000000


def random_string(size):
    characters = string.ascii_letters + string.digits
    return ''.join(random.choices(characters, k=size))


def generate_insertions(insertions_file: str, num_operations: int):
    """
    Since the workload generation program tries to keep all the values in memory, it has a very hard time
    with larger value sizes. In addition, there seems to be some confusing issues that only arise with certain
    key/value sizes. This function generates insertions in a more straightforward way, since our case is simple.

    :param insertions_file: The file to write the insertions to
    :param num_operations: The number of insertions to generate
    """

    print()
    with open(insertions_file, 'w') as f:
        for _ in tqdm(range(num_operations), desc='Generating insertions', leave=False):
            key = random_string(KEY_SIZE)
            value = random_string(VALUE_SIZE)
            f.write(f'I {key} {value}\n')


def execute_workload_gen(output_file: str, args: list):
    path = os.path.dirname(output_file)
    if not os.path.exists(path):
        os.makedirs(path)

    run_command = ['../bin/load_gen.exe', '--output-path', output_file, '-E', str(KEY_SIZE + VALUE_SIZE),
                   '-L', f'{KEY_SIZE / (KEY_SIZE + VALUE_SIZE):.7f}'] + args

    print()
    with tqdm(total=100, desc=f'Generating {output_file}', leave=False) as pbar:
        with subprocess.Popen(run_command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, bufsize=1) as process:
            for char in iter(lambda: process.stdout.read(1), ''):
                if char == '#':
                    pbar.update(1)
            process.wait()


def generate_workloads():
    """Conditionally generates workloads"""

    insertion_workload = 'workloads/insertions.txt'
    if not os.path.exists(insertion_workload):
        generate_insertions(insertion_workload, NUM_OPERATIONS)

    print('Query workloads take 30-60 seconds to preload the insertions')

    uniform_workload = 'workloads/uniform.txt'
    if not os.path.exists(uniform_workload):
        execute_workload_gen(uniform_workload, ['--preloading', '--preload-filename', insertion_workload,
                                                '-Q', '10000000'])

    for alpha in ZIPF_ALPHAS:
        workload = f'workloads/zipf_{alpha:.2f}.txt'
        if not os.path.exists(workload):
            execute_workload_gen(workload, ['--preloading', '--preload-filename', insertion_workload,
                                            '-Q', '10000000', '--ED=3', '--ED_ZALPHA', str(alpha)])


if __name__ == '__main__':
    generate_workloads()
