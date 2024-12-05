# Alias for readability
type LevelByLevelStat = list[int]

class AggregateStat:
    """Class to store RocksDB aggregate statistics"""

    def __init__(self):
        self.median: float = 0
        self.p95: float = 0
        self.p99: float = 0
        self.max: float = 0
        self.count: int = 0
        self.sum: int = 0

class RocksDBStatistics:
    """Class to store RocksDB statistics"""

    def __init__(self):
        self.performance: dict[str, int | LevelByLevelStat] = {}
        self.io: dict[str, int] = {}
        self.count_stats: dict[str, int] = {}
        self.aggregate_stats: dict[str, AggregateStat] = {}


def parse_output(output_file) -> RocksDBStatistics:
    """
    Parse the output file from RocksDB and return the statistics.

    :param output_file: The output file to parse
    :return: The parsed statistics
    """

    statistics = RocksDBStatistics()
    with open(output_file, 'r') as f:
        # Parse the performance context
        perf_context = f.readline().strip()
        perf_context_stats = perf_context.split(', ')
        last_key = None
        for stat in perf_context_stats:
            if '=' in stat:
                key, value = stat.split(' = ')
                if '@' in value:
                    parsed = int(value.split('@')[0])
                    statistics.performance[key] = [parsed]
                else:
                    statistics.performance[key] = int(value)
                last_key = key
            else:
                parsed = int(stat.split('@')[0])
                statistics.performance[last_key].append(parsed)

        f.readline()

        # Parse the io context
        io_context = f.readline().strip()
        io_context_stats = io_context.split(', ')
        for stat in io_context_stats:
            key, value = stat.split(' = ')
            statistics.io[key] = int(value)

        f.readline()

        # Parse the count stats
        while (next_line := f.readline().strip()).split(' ')[1] == 'COUNT':
            key_part, value = next_line.split(' : ')
            key = key_part.split(' ')[0]
            statistics.count_stats[key] = int(value)

        # Parse the aggregate stats
        while 'P50' in (next_line := f.readline().strip()):
            line_parts = next_line.split(' ')
            key = line_parts[0]
            stat = AggregateStat()
            stat.median = float(line_parts[3])
            stat.p95 = float(line_parts[6])
            stat.p99 = float(line_parts[9])
            stat.max = float(line_parts[12])
            stat.count = int(line_parts[15])
            stat.sum = int(line_parts[18])
            statistics.aggregate_stats[key] = stat

    return statistics
