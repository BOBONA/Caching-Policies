# RocksDB-Wrapper

This repository provides a simple wrapper module for **RocksDB**, designed to facilitate database operations, workload generation, and performance testing. The wrapper leverages [RocksDB-SSD](https://github.com/SSD-Brandeis/RocksDB-SSD) for database storage and [KV-WorkloadGenerator](https://github.com/SSD-Brandeis/KV-WorkloadGenerator) for generating customizable key-value workloads.

## Prerequisites

Before using this module, ensure you have installed the following dependencies:

- Git
- CMake
- A C++ compiler (e.g., GCC, Clang)

## Cloning the Repository and Submodules

To clone the repository along with its submodules in one step, use the following command:

```bash
git clone --recurse-submodules https://github.com/BOBONA/Caching-Policies
```

This command will clone the repository and automatically initialize and update the submodules, which include RocksDB-SSD and KV-WorkloadGenerator. If you've already cloned the repository without initializing the submodules, you can run:

```bash
git submodule update --init --recursive
```

This will initialize and update the submodules in your existing clone.

## Setup Instructions

### 1. **Generate Workload**

The first step is to generate a workload using the `load_gen` tool from the `KV-WorkloadGenerator` submodule. You will find an executable named `load_gen` inside the `bin` folder. This tool generates a `workload.txt` file, which will be used in experiments.

For detailed instructions on how to use the `load_gen` tool, refer to the [KV-WorkloadGenerator repository](https://github.com/SSD-Brandeis/KV-WorkloadGenerator).

### 2. **Run RocksDB-Wrapper**

Once you have the `workload.txt` file in the project root directory, you're ready to run experiments. Use the `./bin/working_version <ARGS>` executable with the desired options.

### Example Command:

```bash
./bin/working_version --file_size 512
```

This example runs the experiment and sets the SST file size to 512 KB.

## Available Options

See [parse_arguments.h](include/parse_arguments.h) for the supported options.


