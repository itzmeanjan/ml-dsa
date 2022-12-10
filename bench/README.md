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
2022-12-10T17:57:21+04:00
Running ./bench/a.out
Run on (8 X 2400 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB
  L1 Instruction 32 KiB
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB
Load Average: 2.07, 1.98, 1.80
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Benchmark                                                                Time             CPU   Iterations average_cpu_cycles items_per_second max_exec_time (ns) median_exec_time (ns) min_exec_time (ns)
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
keygen<4, 4, 13, 2>/manual_time                                        119 us          133 us         5804           286.448k       8.37454k/s           606.365k              115.862k           113.118k
sign<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32/manual_time            751 us          897 us         1003           1.80212M       1.33156k/s           3.83816M              544.281k            244.15k
verify<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32/manual_time          140 us         1034 us         5035           336.488k       7.12887k/s           4.13299M              134.844k           132.459k
keygen<6, 5, 13, 4>/manual_time                                        199 us          212 us         3458           476.573k       5.03402k/s             474.5k              194.794k           192.144k
sign<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32/manual_time         1251 us         1479 us          586           3.00152M        799.525/s           6.92701M              969.714k           368.386k
verify<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32/manual_time        220 us         1701 us         3211           526.696k       4.55494k/s           1.63325M              212.599k           208.779k
keygen<8, 7, 13, 2>/manual_time                                        312 us          327 us         2221           748.944k       3.20355k/s           632.433k              304.003k            298.45k
sign<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32/manual_time         1375 us         1715 us          552           3.30048M        727.104/s           6.56857M              1.10724M           550.766k
verify<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32/manual_time        353 us         2105 us         2026           848.102k        2.8291k/s           4.89657M              338.623k           334.226k
```

### On ARM Cortex-A72 **[ compiled with GCC ]**

```bash
2022-12-10T13:59:18+00:00
Running ./bench/a.out
Run on (16 X 166.66 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x16)
  L1 Instruction 48 KiB (x16)
  L2 Unified 2048 KiB (x4)
Load Average: 0.22, 0.05, 0.02
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Benchmark                                                                Time             CPU   Iterations items_per_second max_exec_time (ns) median_exec_time (ns) min_exec_time (ns)
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
keygen<4, 4, 13, 2>/manual_time                                        453 us          457 us         1545       2.20735k/s           472.118k              452.678k           448.802k
sign<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32/manual_time           3859 us         4320 us          190        259.153/s           20.7134M              2.77766M           1.21426M
verify<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32/manual_time          580 us         4915 us         1206       1.72309k/s           597.543k              579.687k           578.559k
keygen<6, 5, 13, 4>/manual_time                                        733 us          737 us          955       1.36401k/s            754.06k              732.388k           729.903k
sign<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32/manual_time         6736 us         7478 us          125        148.457/s           30.8519M              5.14322M           1.78767M
verify<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32/manual_time        890 us         7533 us          786       1.12301k/s           907.456k              889.456k           888.652k
keygen<8, 7, 13, 2>/manual_time                                       1131 us         1135 us          619        884.176/s           1.13793M              1.13042M           1.12564M
sign<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32/manual_time         8251 us         9390 us          100        121.203/s           32.0286M              5.68044M            2.5726M
verify<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32/manual_time       1356 us         9750 us          516        737.683/s           1.37342M              1.35426M           1.35366M
```

### On ARM Cortex-A72 **[ compiled with Clang ]**

```bash
2022-12-10T14:01:17+00:00
Running ./bench/a.out
Run on (16 X 166.66 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x16)
  L1 Instruction 48 KiB (x16)
  L2 Unified 2048 KiB (x4)
Load Average: 0.33, 0.14, 0.05
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Benchmark                                                                Time             CPU   Iterations items_per_second max_exec_time (ns) median_exec_time (ns) min_exec_time (ns)
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
keygen<4, 4, 13, 2>/manual_time                                        381 us          385 us         1836       2.62301k/s           406.165k              380.893k            377.51k
sign<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32/manual_time           2976 us         3367 us          235        336.019/s           12.6363M              2.19708M           962.836k
verify<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32/manual_time          462 us         3853 us         1515       2.16275k/s            481.73k              461.606k           460.862k
keygen<6, 5, 13, 4>/manual_time                                        623 us          627 us         1124       1.60622k/s           630.579k              621.879k           619.683k
sign<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32/manual_time         5127 us         5759 us          148        195.058/s           30.3119M              4.05037M           1.42105M
verify<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32/manual_time        720 us         6381 us          973       1.38965k/s           740.692k              718.576k           717.963k
keygen<8, 7, 13, 2>/manual_time                                        972 us          977 us          720        1028.29/s           994.277k              971.657k           966.148k
sign<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32/manual_time         5497 us         6479 us          109        181.934/s           21.5282M              4.50217M           2.06528M
verify<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32/manual_time       1109 us         7861 us          631        901.771/s           1.11748M              1.10755M           1.10642M
```

### On ARM Neoverse-V1 **[ compiled with GCC ]**

```bash
2022-12-10T14:03:38+00:00
Running ./bench/a.out
Run on (64 X 2100 MHz CPU s)
CPU Caches:
  L1 Data 64 KiB (x64)
  L1 Instruction 64 KiB (x64)
  L2 Unified 1024 KiB (x64)
  L3 Unified 32768 KiB (x1)
Load Average: 0.33, 0.09, 0.03
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Benchmark                                                                Time             CPU   Iterations items_per_second max_exec_time (ns) median_exec_time (ns) min_exec_time (ns)
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
keygen<4, 4, 13, 2>/manual_time                                        148 us          150 us         4715       6.73746k/s           159.589k              148.222k           146.119k
sign<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32/manual_time            846 us          996 us          898       1.18202k/s           4.47619M              615.749k           290.894k
verify<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32/manual_time          167 us         1165 us         4186       5.97836k/s           182.236k              166.938k           166.068k
keygen<6, 5, 13, 4>/manual_time                                        251 us          252 us         2790       3.98626k/s           260.158k              250.377k            248.72k
sign<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32/manual_time         1353 us         1608 us          547        739.283/s           7.74467M              1.12579M             442.1k
verify<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32/manual_time        269 us         1912 us         2598       3.71141k/s           279.115k               268.98k           268.204k
keygen<8, 7, 13, 2>/manual_time                                        408 us          409 us         1716       2.45178k/s            414.05k               407.46k           404.426k
sign<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32/manual_time         1517 us         1932 us          410        659.062/s           6.96834M              1.30627M           677.523k
verify<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32/manual_time        438 us         2473 us         1599       2.28536k/s           443.934k              436.902k           435.677k
```

### On ARM Neoverse-V1 **[ compiled with Clang ]**

```bash
2022-12-10T14:04:37+00:00
Running ./bench/a.out
Run on (64 X 2100 MHz CPU s)
CPU Caches:
  L1 Data 64 KiB (x64)
  L1 Instruction 64 KiB (x64)
  L2 Unified 1024 KiB (x64)
  L3 Unified 32768 KiB (x1)
Load Average: 0.47, 0.18, 0.07
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Benchmark                                                                Time             CPU   Iterations items_per_second max_exec_time (ns) median_exec_time (ns) min_exec_time (ns)
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
keygen<4, 4, 13, 2>/manual_time                                        197 us          199 us         3546       5.06428k/s            203.65k              197.218k            194.84k
sign<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32/manual_time           1606 us         1807 us          404        622.569/s           9.43351M              1.21882M           531.318k
verify<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32/manual_time          246 us         2148 us         2848       4.06927k/s            252.37k              245.361k            244.56k
keygen<6, 5, 13, 4>/manual_time                                        327 us          329 us         2141       3.05735k/s            337.32k              326.605k           325.233k
sign<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32/manual_time         2638 us         2968 us          242        379.127/s           10.5144M              2.15801M           764.136k
verify<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32/manual_time        377 us         3325 us         1857        2.6524k/s           388.282k              376.423k           375.206k
keygen<8, 7, 13, 2>/manual_time                                        506 us          508 us         1383       1.97598k/s           513.194k              505.544k           502.862k
sign<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32/manual_time         2848 us         3357 us          250        351.136/s           11.9661M              2.38615M           1.10869M
verify<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32/manual_time        583 us         4209 us         1201       1.71498k/s           593.927k              582.183k           581.076k
```

### On Intel(R) Xeon(R) CPU E5-2686 v4 @ 2.30GHz **[ compiled with GCC ]**

```bash
2022-12-10T14:06:39+00:00
Running ./bench/a.out
Run on (4 X 2300.25 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x2)
  L1 Instruction 32 KiB (x2)
  L2 Unified 256 KiB (x2)
  L3 Unified 46080 KiB (x1)
Load Average: 0.08, 0.02, 0.01
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Benchmark                                                                Time             CPU   Iterations average_cpu_cycles items_per_second max_exec_time (ns) median_exec_time (ns) min_exec_time (ns)
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
keygen<4, 4, 13, 2>/manual_time                                        243 us          247 us         2883           556.599k       4.11992k/s           304.754k              242.036k            238.43k
sign<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32/manual_time           1574 us         1825 us          465           3.61897M        635.152/s           6.22587M              1.13769M           525.221k
verify<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32/manual_time          288 us         2013 us         2430           660.785k       3.47049k/s           319.432k              287.216k           286.216k
keygen<6, 5, 13, 4>/manual_time                                        412 us          416 us         1687           945.582k       2.42784k/s           522.518k               410.36k           408.874k
sign<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32/manual_time         2395 us         2815 us          259           5.50504M        417.598/s           10.2004M              2.07116M           793.216k
verify<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32/manual_time        459 us         3482 us         1525           1053.12k       2.17977k/s           482.528k              457.264k           456.297k
keygen<8, 7, 13, 2>/manual_time                                        654 us          659 us         1070           1.50272M       1.52867k/s           713.359k              652.206k           647.727k
sign<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32/manual_time         2863 us         3527 us          230           6.58143M        349.315/s           15.7766M              2.36081M           1.18844M
verify<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32/manual_time        730 us         4349 us          958           1.67768M       1.36907k/s            795.16k              727.551k             726.2k
```

### On Intel(R) Xeon(R) CPU E5-2686 v4 @ 2.30GHz **[ compiled with Clang ]**

```bash
2022-12-10T14:07:50+00:00
Running ./bench/a.out
Run on (4 X 2300.25 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x2)
  L1 Instruction 32 KiB (x2)
  L2 Unified 256 KiB (x2)
  L3 Unified 46080 KiB (x1)
Load Average: 0.38, 0.13, 0.05
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Benchmark                                                                Time             CPU   Iterations average_cpu_cycles items_per_second max_exec_time (ns) median_exec_time (ns) min_exec_time (ns)
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
keygen<4, 4, 13, 2>/manual_time                                        179 us          183 us         3918           409.049k       5.59948k/s           239.969k              178.092k           175.227k
sign<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32/manual_time           1296 us         1484 us          715           2.97976M        771.362/s           6.25318M              912.628k           416.912k
verify<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32/manual_time          216 us         1633 us         3235           494.949k       4.62729k/s           277.285k              215.315k           214.344k
keygen<6, 5, 13, 4>/manual_time                                        300 us          304 us         2337           687.235k       3.33845k/s           343.423k              298.559k           296.678k
sign<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32/manual_time         2095 us         2403 us          362           4.81671M        477.268/s           9.52159M              1.64576M           619.648k
verify<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32/manual_time        338 us         2741 us         2070           775.202k       2.95815k/s           532.831k              336.588k           335.695k
keygen<8, 7, 13, 2>/manual_time                                        470 us          475 us         1487           1079.51k       2.12669k/s            510.79k              468.824k           464.456k
sign<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32/manual_time         2200 us         2680 us          299             5.057M        454.583/s           13.0877M               1.8499M           915.265k
verify<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32/manual_time        533 us         3309 us         1309            1.2238M       1.87553k/s           590.911k              531.139k           529.431k
```
