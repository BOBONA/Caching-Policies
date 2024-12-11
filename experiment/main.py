import os
import shutil

from experiment.generate_workloads import NUM_INSERTIONS, KEY_SIZE, VALUE_SIZE, ZIPF_ALPHAS
from experiment.run_workload import run_workload, run_workload_from_base


def generate_filled_db():
    workload = 'workloads/insertions.txt'
    output_file = 'output/filled_db.json'

    run_workload(workload, './filled_db', output_file, ['-T', '4'])


def run_tests():
    """A few experiments are important to understanding RocksDB's cache performance"""

    if not os.path.exists('filled_db'):
        generate_filled_db()

    total_size_mb = NUM_INSERTIONS * (KEY_SIZE + VALUE_SIZE) / 1024**2

    distributions = ['uniform.txt'] + [f'zipf_{f:.2f}.txt' for f in ZIPF_ALPHAS]
    all_cache_sizes = [0.02, 0.05, 0.1, 0.15, 0.2, 0.3, 0.4, 0.5, 0.65, 0.8, 0.95, 1.1]
    subset_cache_sizes = [0.02, 0.05, 0.1, 0.2, 0.4, 0.8, 1.1]
    high_priority_ratios = [0, 0.1, 0.3, 0.5, 0.7, 0.9]
    cache_metadata_options = [True, False]
    pinning_options = {'kNone': 1, 'kFlushedOrSimilar': 2, 'kAll': 3}

    # Experiment 1: We test different cache sizes against different levels of skew
    # Pinning is kNone. Priority ratio is 0.5. Metadata is cached with high priority.

    experiment_path = 'experiment1_skew_over_bb'
    for distribution in distributions:
        for cache_size in all_cache_sizes:
            name = f'{distribution}_bb-{cache_size}'
            db_path = f'{experiment_path}/{name}'
            workload_path = f'workloads/{distribution}'
            actual_size = int(total_size_mb * cache_size)
            if os.path.exists(f'{experiment_path}/{name}.json'):
                continue
            run_workload_from_base('filled_db', db_path, workload_path, f'{experiment_path}/{name}.json',
                                   ['-T', '4', '--bb', str(actual_size), '--metadata_pinning', str(pinning_options['kNone']),
                                    '--cache_high_priority_ratio', '0.5', '--cache_metadata_high_pri', '1'])
            shutil.rmtree(db_path)

    # Experiment 2: We sanity check that caching metadata with high priority is better than not
    # Pinning is kNone. Priority ratio is 0.5. Cache size is 0.2. We use zipf_0.30.txt as the workload.

    experiment_path = 'experiment2_metadata_priority_matters'
    for option in cache_metadata_options:
        name = f'high_priority-{option}'
        db_path = f'{experiment_path}/{name}'
        workload_path = 'workloads/zipf_0.30.txt'
        actual_size = int(total_size_mb * 0.2)
        if os.path.exists(f'{experiment_path}/{name}.json'):
            continue
        run_workload_from_base('filled_db', db_path, workload_path, f'{experiment_path}/{name}.json',
                               ['-T', '4', '--bb', str(actual_size), '--metadata_pinning', str(pinning_options['kNone']),
                                '--cache_high_priority_ratio', '0.5', '--cache_metadata_high_pri', str(int(option))])
        shutil.rmtree(db_path)

    # Experiment 3: We test the effects of the different pinning policies against the subset of cache sizes
    # Priority ratio is 0.5. Metadata is cached with high priority. We use zipf_0.30.txt as the workload.

    experiment_path = 'experiment3_pinning_policies'
    for policy, choice in pinning_options.items():
        for cache_size in subset_cache_sizes:
            for workload in ['uniform.txt', 'zipf_0.30.txt', 'zipf_1.00.txt']:
                name = f'pin-{policy}_bb-{cache_size}_{workload}'
                db_path = f'{experiment_path}/{name}'
                workload_path = f'workloads/{workload}'
                actual_size = int(total_size_mb * cache_size)
                if os.path.exists(f'{experiment_path}/{name}.json'):
                    continue
                run_workload_from_base('filled_db', db_path, workload_path, f'{experiment_path}/{name}.json',
                                       ['-T', '4', '--bb', str(actual_size), '--metadata_pinning', str(choice),
                                        '--cache_high_priority_ratio', '0.5', '--cache_metadata_high_pri', '1'])
                shutil.rmtree(db_path)

    # Experiment 4: We test high priority ratios against different cache sizes for kNone and kFlushedOrSimilar
    # Metadata is cached with high priority. We use zipf_0.30.txt as the workload.

    experiment_path = 'experiment4_high_priority_ratios'
    for choice in high_priority_ratios:
        for cache_size in subset_cache_sizes:
            for policy_choice in ['kNone', 'kFlushedOrSimilar']:
                name = f'high_pri-{choice}_pin-{policy_choice}_bb-{cache_size}'
                db_path = f'{experiment_path}/{name}'
                workload_path = 'workloads/zipf_0.30.txt'
                actual_size = int(total_size_mb * cache_size)
                if os.path.exists(f'{experiment_path}/{name}.json'):
                    continue
                run_workload_from_base('filled_db', db_path, workload_path, f'{experiment_path}/{name}.json',
                                       ['-T', '4', '--bb', str(actual_size), '--metadata_pinning', str(pinning_options[policy_choice]),
                                        '--cache_high_priority_ratio', str(int(choice)), '--cache_metadata_high_pri', '1'])
                shutil.rmtree(db_path)


if __name__ == '__main__':
    run_tests()