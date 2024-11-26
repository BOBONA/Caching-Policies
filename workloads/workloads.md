# RusKey
#### Write Heavy
```shell
..\bin\load_gen.exe -I 9000000 -Q 1000000 -U 0 -D 0 -S 0 --output-path="write_heavy.txt"
```
#### Read Heavy
```shell
..\bin\load_gen.exe -I 1000000 -Q 9000000 -U 0 -D 0 -S 0 --output-path="read_heavy.txt"
```
#### Balanced
```shell
..\bin\load_gen.exe -I 5000000 -Q 5000000 -U 0 -D 0 -S 0 --output-path="read_write_balanced.txt"
```
# Leaper

| Workload Type              | Point Lookups (%) | Range Lookups (%) | Updates (%) | Inserts (%) | R/W Ratio | Table Size (Records) | Skewness (Zipf Factor) |
|----------------------------|-------------------|-------------------|-------------|-------------|-----------|----------------------|------------------------|
| Default Synthetic Workload | 75%               | 0%                | 0%          | 25%         | 3:1       | 20m                  | 0.5                    |
| E-commerce Workload        | 75%               | 10%               | 10%         | 5%          | 6:1       | 10m                  | 0.3                    |
| Instant Messaging Workload | 40%               | 35%               | 5%          | 20%         | 2:3       | 8m                   | 0.9                    |

#### Synthetic
```shell
..\bin\load_gen.exe -I 2500000 -Q 7500000 -U 0 -D 0 -S 0 --ED=3 --ED_ZALPHA=0.5 --output-path="default_synthetic.txt"
```

#### E-commerce
```shell
..\bin\load_gen.exe -I 500000 -Q 7500000 -U 1000000 -D 0 -S 1000000 --range_query_selectivity=0.1 --ED=3 --ED_ZALPHA=0.3 --UD=3 --UD_ZALPHA=0.3 --output-path="ecommerce.txt"
```
#### Instant Messaging
```shell
..\bin\load_gen.exe -I 2000000 -Q 4000000 -U 500000 -D 0 -S 3500000 --range_query_selectivity=0.1 --ED=3 --ED_ZALPHA=0.9 --UD=3 --UD_ZALPHA=0.9 --output-path="instant_messaging.txt"
```
