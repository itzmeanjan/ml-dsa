## Benchmarking Dilithium DSA

Benchmarking of Dilithium Digital Signature Algorithm, along with its components, can be done, by issuing

```bash
make benchmark
```

> **Note**

> Benchmarking expects presence of `google-benchmark` library in global namespace ( can be found by compiler ).

> **Warning**

> CPUs generally have frequency boosting technology, which should be disabled during benchmarking. You may use [this](https://github.com/google/benchmark/blob/60b16f1/docs/user_guide.md#disabling-cpu-frequency-scaling) as your guide.

> **Note**

> On x86_64 architecture average CPU cycle spent in keygen/ sign/ verify routine is measured using compiler built-in `__rdtsc`. I've tested it with both Clang and GCC.

### On Intel(R) Core(TM) i5-8279U CPU @ 2.40GHz **[ compiled with Clang ]**

```bash
2022-12-10T13:47:17+04:00
Running ./bench/a.out
Run on (8 X 2400 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB
  L1 Instruction 32 KiB
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB
Load Average: 2.55, 2.18, 2.36
----------------------------------------------------------------------------------------------------------------------------------
Benchmark                                                    Time             CPU   Iterations average_cpu_cycles items_per_second
----------------------------------------------------------------------------------------------------------------------------------
keygen<4, 4, 13, 2>/32                                     122 us          121 us         5932           292.139k       8.23964k/s
sign<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32            832 us          829 us         1000           1.99718M       1.20582k/s
verify<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32          135 us          135 us         5109           323.134k       7.43134k/s
keygen<6, 5, 13, 4>/32                                     198 us          197 us         3534           474.257k       5.06768k/s
sign<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32         2205 us         2204 us         1175           5.29224M        453.814/s
verify<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32        215 us          215 us         3207           516.182k       4.65119k/s
keygen<8, 7, 13, 2>/32                                     315 us          315 us         2206           755.644k       3.17797k/s
sign<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32         4200 us         4197 us          600           10.0806M        238.275/s
verify<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32        353 us          352 us         1983           848.065k       2.83704k/s
```

### On ARM Cortex-A72 **[ compiled with GCC ]**

```bash
2022-12-10T09:35:15+00:00
Running ./bench/a.out
Run on (16 X 166.66 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x16)
  L1 Instruction 48 KiB (x16)
  L2 Unified 2048 KiB (x4)
Load Average: 0.23, 0.08, 0.02
---------------------------------------------------------------------------------------------------------------
Benchmark                                                    Time             CPU   Iterations items_per_second
---------------------------------------------------------------------------------------------------------------
keygen<4, 4, 13, 2>/32                                     440 us          440 us         1590       2.27467k/s
sign<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32           6577 us         6577 us          100        152.049/s
verify<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32          565 us          565 us         1239       1.77033k/s
keygen<6, 5, 13, 4>/32                                     712 us          712 us          985       1.40508k/s
sign<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32         1766 us         1766 us          396        566.116/s
verify<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32        870 us          870 us          805       1.14886k/s
keygen<8, 7, 13, 2>/32                                    1097 us         1097 us          639         911.43/s
sign<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32         8668 us         8668 us          100        115.362/s
verify<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32       1322 us         1322 us          529        756.186/s
```

### On ARM Cortex-A72 **[ compiled with Clang ]**

```bash
2022-12-10T09:36:35+00:00
Running ./bench/a.out
Run on (16 X 166.66 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x16)
  L1 Instruction 48 KiB (x16)
  L2 Unified 2048 KiB (x4)
Load Average: 0.38, 0.15, 0.05
---------------------------------------------------------------------------------------------------------------
Benchmark                                                    Time             CPU   Iterations items_per_second
---------------------------------------------------------------------------------------------------------------
keygen<4, 4, 13, 2>/32                                     381 us          381 us         1827       2.62176k/s
sign<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32           2807 us         2807 us          726        356.199/s
verify<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32          460 us          460 us         1519       2.17158k/s
keygen<6, 5, 13, 4>/32                                     622 us          622 us         1126       1.60825k/s
sign<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32         6675 us         6675 us          100        149.819/s
verify<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32        718 us          718 us          975       1.39233k/s
keygen<8, 7, 13, 2>/32                                     970 us          970 us          725        1030.76/s
sign<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32        13001 us        13001 us          123         76.919/s
verify<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32       1107 us         1107 us          632        903.259/s
```

### On ARM Neoverse-V1 **[ compiled with GCC ]**

```bash
2022-12-10T09:42:11+00:00
Running ./bench/a.out
Run on (64 X 2100 MHz CPU s)
CPU Caches:
  L1 Data 64 KiB (x64)
  L1 Instruction 64 KiB (x64)
  L2 Unified 1024 KiB (x64)
  L3 Unified 32768 KiB (x1)
Load Average: 0.11, 0.03, 0.01
---------------------------------------------------------------------------------------------------------------
Benchmark                                                    Time             CPU   Iterations items_per_second
---------------------------------------------------------------------------------------------------------------
keygen<4, 4, 13, 2>/32                                     137 us          137 us         5056        7.2838k/s
sign<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32            428 us          428 us         2532        2.3346k/s
verify<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32          155 us          155 us         4514       6.44885k/s
keygen<6, 5, 13, 4>/32                                     235 us          235 us         2979       4.25399k/s
sign<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32          427 us          427 us         1641       2.34436k/s
verify<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32        252 us          252 us         2778       3.96768k/s
keygen<8, 7, 13, 2>/32                                     383 us          383 us         1833       2.61194k/s
sign<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32         1843 us         1843 us         1089        542.639/s
verify<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32        410 us          410 us         1710       2.44163k/s
```

### On ARM Neoverse-V1 **[ compiled with Clang ]**

```bash
2022-12-10T09:43:25+00:00
Running ./bench/a.out
Run on (64 X 2100 MHz CPU s)
CPU Caches:
  L1 Data 64 KiB (x64)
  L1 Instruction 64 KiB (x64)
  L2 Unified 1024 KiB (x64)
  L3 Unified 32768 KiB (x1)
Load Average: 0.24, 0.08, 0.02
---------------------------------------------------------------------------------------------------------------
Benchmark                                                    Time             CPU   Iterations items_per_second
---------------------------------------------------------------------------------------------------------------
keygen<4, 4, 13, 2>/32                                     198 us          198 us         3537       5.05759k/s
sign<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32           2930 us         2930 us          575        341.336/s
verify<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32          245 us          245 us         2857        4.0814k/s
keygen<6, 5, 13, 4>/32                                     326 us          326 us         2145       3.06364k/s
sign<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32         5408 us         5408 us          324        184.906/s
verify<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32        377 us          377 us         1858       2.65426k/s
keygen<8, 7, 13, 2>/32                                     506 us          506 us         1384       1.97471k/s
sign<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32         1752 us         1751 us          400        570.951/s
verify<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32        583 us          583 us         1201       1.71646k/s
```

### On Intel(R) Xeon(R) CPU E5-2686 v4 @ 2.30GHz **[ compiled with GCC ]**

```bash
2022-12-10T09:45:19+00:00
Running ./bench/a.out
Run on (4 X 2300.25 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x2)
  L1 Instruction 32 KiB (x2)
  L2 Unified 256 KiB (x2)
  L3 Unified 46080 KiB (x1)
Load Average: 0.15, 0.03, 0.01
----------------------------------------------------------------------------------------------------------------------------------
Benchmark                                                    Time             CPU   Iterations average_cpu_cycles items_per_second
----------------------------------------------------------------------------------------------------------------------------------
keygen<4, 4, 13, 2>/32                                     231 us          231 us         3032           530.523k       4.33569k/s
sign<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32           1705 us         1705 us          411           3.92159M        586.534/s
verify<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32          270 us          270 us         2594           620.563k       3.70645k/s
keygen<6, 5, 13, 4>/32                                     392 us          392 us         1786           900.618k       2.55385k/s
sign<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32         1204 us         1204 us          430           2.77001M         830.44/s
verify<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32        435 us          435 us         1611           1000.33k       2.29936k/s
keygen<8, 7, 13, 2>/32                                     633 us          633 us         1105           1.45588M       1.57982k/s
sign<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32         3434 us         3434 us          604            7.8992M        291.182/s
verify<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32        696 us          696 us         1005           1.60183M       1.43579k/s
```

### On Intel(R) Xeon(R) CPU E5-2686 v4 @ 2.30GHz **[ compiled with Clang ]**

```bash
2022-12-10T09:46:27+00:00
Running ./bench/a.out
Run on (4 X 2300.25 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x2)
  L1 Instruction 32 KiB (x2)
  L2 Unified 256 KiB (x2)
  L3 Unified 46080 KiB (x1)
Load Average: 0.27, 0.09, 0.03
----------------------------------------------------------------------------------------------------------------------------------
Benchmark                                                    Time             CPU   Iterations average_cpu_cycles items_per_second
----------------------------------------------------------------------------------------------------------------------------------
keygen<4, 4, 13, 2>/32                                     179 us          179 us         3897           412.497k       5.57626k/s
sign<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32            658 us          658 us         1064           1.51226M       1.52089k/s
verify<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32          213 us          213 us         3292           489.305k        4.7003k/s
keygen<6, 5, 13, 4>/32                                     299 us          299 us         2345           686.992k       3.34794k/s
sign<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32         7108 us         7107 us         1130           16.3483M        140.698/s
verify<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32        335 us          335 us         2089            770.28k       2.98576k/s
keygen<8, 7, 13, 2>/32                                     470 us          470 us         1483           1080.05k       2.12949k/s
sign<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32         2794 us         2794 us          762            6.4254M        357.971/s
verify<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32        530 us          530 us         1318           1.21981M       1.88552k/s
```
