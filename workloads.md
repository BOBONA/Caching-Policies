## Write Heavy Workloads
bin\load_gen.exe -I 90000000 -Q 10000000 -U 0 -D 0 -R 0 --output-path=workload_write_heavy.txt

## Read Heavy Worklaods
bin\load_gen.exe -I 10000000 -Q 90000000 -U 0 -D 0 -R 0 --output-path=workload_read_heavy.txt

## Balanced Workload
bin\load_gen.exe -I 50000000 -Q 50000000 -U 0 -D 0 -R 0 --output-path=workload_read_write_balanced.txt

 ## Mixed Workloads
| Workload Type               | Point Lookups (%) | Range Lookups (%) | Updates (%) | Inserts (%) | R/W Ratio | Table Size (Records) | Skewness (Zipf Factor) |
|-----------------------------|-------------------|-------------------|-------------|-------------|-----------|----------------------|------------------------|
| Default Synthetic Workload  | 75%              | 0%               | 0%          | 25%         | 3:1       | 20m                 | 0.5 |
| E-commerce Workload         | 75%              | 10%              | 10%         | 5%          | 6:1       | 10m                 | 0.3|
| Instant Messaging Workload  | 40%              | 35%              | 5%          | 20%         | 2:3       | 8m                  | 0.9 |

bin\load_gen.exe -I 20000000 -Q 7500000 -U 0 -D 0 -R 0 --range_query_selectivity=0 --output-path=workload_default_synthetic.txt
bin\load_gen.exe -I 10000000 -Q 7500000 -U 1000000 -D 0 -R 1000000 --range_query_selectivity=10 --output-path=workload_ecommerce.txt
bin\load_gen.exe -I 8000000 -Q 4000000 -U 3500000 -D 0 -R 2000000 --range_query_selectivity=35 --output-path=workload_instant_messaging.txt

