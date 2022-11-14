# dilithium
CRYSTALS-Dilithium: Post-Quantum Digital Signature Algorithm

## Motivation

Dilithium is one of those post-quantum digital signature algorithms ( DSA ), which are selected by NIST for standardization purpose. Dilithium's security is based on hardness of finding short vectors in lattice i.e. it's lattice based Post Quantum Cryptographic (PQC) construction.

Dilithium DSA has three main algorithms

- KeyGen
- Sign
- Verify

**KeyGen** takes a 32 -bytes seed, which is used for deterministically computing both public key and secret key.

**Sign** takes secret key and N (>0) -bytes message as input, which is used for deterministically/ randomly signing message, producing signature bytes.

**Verify** takes public key, N (>0) -bytes message and signature, returning boolean value, denoting status of successful signature verification operation.

Here I'm maintaining Dilithium as zero-dependency, header-only & easy-to-use C++ library, offering Dilithium key generation, signing & verification API for three NIST security level ( i.e. 2, 3, 5 ) parameters, as defined in table 2 of Dilithium specification. 

`sha3` is the only dependency of this project, which itself is a zero-dependency, header-only C++ library, implementing SHA3 specification of NIST ( i.e. FIPS PUB 202 ). This is done purposefully, in order to modularize commonly seen symmetric key dependency in post-quantum cryptographic constructions.

> **Note**

> `sha3` is pinned to specific commit, using git submodule. See [usage](#usage) below in order to understand how to use `dilithium` as dependency in your project.

> Find Dilithium specification [here](https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf), which should be referred to when understanding this implementation.

> Follow progress of NIST PQC standardization effort [here](https://csrc.nist.gov/projects/post-quantum-cryptography).

## Prerequisites

- A C++ compiler with C++20 standard library such as `g++`/ `clang++`

```bash
$ clang++ --version
Ubuntu clang version 14.0.0-1ubuntu1
Target: aarch64-unknown-linux-gnu
Thread model: posix
InstalledDir: /usr/bin

$ g++ --version
g++ (Ubuntu 11.2.0-19ubuntu1) 11.2.0
```

- System development utilities such as `make`, `cmake` & `git`

```bash
$ make --version
GNU Make 4.3

$ cmake --version
cmake version 3.22.1

$ git --version
git version 2.34.1
```

- For benchmarking purposes, you must have `google-benchmark` library globally installed. I found [this](https://github.com/google/benchmark/tree/60b16f1#installation) guide to be useful.

- Note, `sha3` is a dependency of this project, which is pinned to a specific commit, using git submodule. Import `sha3` dependency after cloning Dilithium.

```bash
git clone https://github.com/itzmeanjan/dilithium.git
cd dilithium
git submodule update --init

# now you can {test, benchmark, use} `dilithium`
```

## Testing

For testing functional correctness of Dilithium Digital Signature Algorithm, along with its components, issue

```bash
make

[test] Dilithium prime field arithmetic
[test] (i)NTT over degree-255 polynomial
[test] Extraction of high, low order bits using Power2Round
[test] Making and using of hint bits using Decompose
[test] Hashing to a ball
[test] Polynomial encoding/ decoding
[test] Hint bit polynomial encoding/ decoding
[test] Dilithium KeyGen -> Signing -> Verification
```

## Benchmarking

Benchmarking of Dilithium Digital Signature Algorithm, along with its components, can be done, by issuing

```bash
make benchmark
```

> **Note**

> Benchmarking expects presence of `google-benchmark` library in global namespace ( can be found by compiler ).

> **Warning**

> CPUs generally have frequency boosting technology, which should be disabled during benchmarking. You may use [this](https://github.com/google/benchmark/blob/60b16f1/docs/user_guide.md#disabling-cpu-frequency-scaling) as your guide.

### On Intel(R) Core(TM) i5-8279U CPU @ 2.40GHz **[ compiled with Clang ]**

```bash
2022-11-14T12:47:16+04:00
Running ./bench/a.out
Run on (8 X 2400 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB
  L1 Instruction 32 KiB
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB
Load Average: 1.61, 1.52, 1.61
----------------------------------------------------------------------------------------------
Benchmark                                                    Time             CPU   Iterations
----------------------------------------------------------------------------------------------
bench_dilithium::ff_add                                   3.78 ns         3.77 ns    185025124
bench_dilithium::ff_sub                                   3.77 ns         3.77 ns    187359146
bench_dilithium::ff_neg                                  0.750 ns        0.750 ns    922752439
bench_dilithium::ff_mul                                   8.02 ns         8.02 ns     84583968
bench_dilithium::ff_inv                                    108 ns          108 ns      8779301
bench_dilithium::ff_div                                    108 ns          108 ns     10586492
bench_dilithium::ff_exp                                    529 ns          529 ns      1321752
bench_dilithium::ntt                                      4035 ns         4032 ns       169963
bench_dilithium::intt                                     4508 ns         4505 ns       155127
bench_dilithium::power2round                              2.39 ns         2.39 ns    294185631
bench_dilithium::decompose<190464>                        8.36 ns         8.35 ns     82291948
bench_dilithium::make_hint<190464>                        13.7 ns         13.7 ns     50107732
bench_dilithium::use_hint<190464>                         8.03 ns         8.02 ns     85429405
bench_dilithium::decompose<523776>                        9.05 ns         9.04 ns     76208725
bench_dilithium::make_hint<523776>                        14.9 ns         14.9 ns     47561456
bench_dilithium::use_hint<523776>                         8.56 ns         8.56 ns     80953868
bench_dilithium::expand_a<4, 4>                          52601 ns        52577 ns        13064
bench_dilithium::expand_a<6, 5>                         100484 ns       100414 ns         6820
bench_dilithium::expand_a<8, 7>                         183057 ns       182815 ns         3833
bench_dilithium::expand_s<2, 4>                           7402 ns         7395 ns       102887
bench_dilithium::expand_s<2, 8>                          15009 ns        14998 ns        48226
bench_dilithium::expand_s<4, 6>                          16595 ns        16582 ns        40624
bench_dilithium::expand_mask<131072, 4>                  31196 ns        31091 ns        22838
bench_dilithium::expand_mask<524288, 5>                  41612 ns        41581 ns        16757
bench_dilithium::expand_mask<524288, 7>                  58676 ns        58605 ns        11751
bench_dilithium::sample_in_ball<39>                        610 ns          610 ns      1082753
bench_dilithium::sample_in_ball<49>                        703 ns          702 ns       949436
bench_dilithium::sample_in_ball<60>                        807 ns          806 ns       825063
bench_dilithium::encode<3>                                1082 ns         1080 ns       659227
bench_dilithium::decode<3>                                1040 ns         1039 ns       685314
bench_dilithium::encode<4>                                 840 ns          840 ns       823036
bench_dilithium::decode<4>                                 910 ns          910 ns       755344
bench_dilithium::encode<6>                                1722 ns         1721 ns       407757
bench_dilithium::decode<6>                                1856 ns         1855 ns       378317
bench_dilithium::encode<10>                               2939 ns         2936 ns       238468
bench_dilithium::decode<10>                               3144 ns         3142 ns       221645
bench_dilithium::encode<13>                               4339 ns         4336 ns       159005
bench_dilithium::decode<13>                               4720 ns         4716 ns       148623
bench_dilithium::encode<18>                               5083 ns         5079 ns       132058
bench_dilithium::decode<18>                               5857 ns         5854 ns       113871
bench_dilithium::encode<20>                               5625 ns         5621 ns       123036
bench_dilithium::decode<20>                               6540 ns         6534 ns       104548
bench_dilithium::encode_hint_bits<4, 80>                  1614 ns         1613 ns       437115
bench_dilithium::decode_hint_bits<4, 80>                   103 ns          103 ns      6590406
bench_dilithium::encode_hint_bits<6, 55>                  2868 ns         2650 ns       254156
bench_dilithium::decode_hint_bits<6, 55>                   109 ns          109 ns      6404157
bench_dilithium::encode_hint_bits<8, 75>                  3517 ns         3515 ns       197362
bench_dilithium::decode_hint_bits<8, 75>                   145 ns          145 ns      4778712
keygen<4, 4, 13, 2>/32                                  152966 ns       152832 ns         4559 items_per_second=6.54314k/s
sign<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32        2050664 ns      2048945 ns          815 items_per_second=488.056/s
verify<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32       178487 ns       178301 ns         3910 items_per_second=5.60848k/s
keygen<6, 5, 13, 4>/32                                  253956 ns       253775 ns         2729 items_per_second=3.94049k/s
sign<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32      1438588 ns      1437645 ns          408 items_per_second=695.582/s
verify<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32     274199 ns       274054 ns         2504 items_per_second=3.64892k/s
keygen<8, 7, 13, 2>/32                                  384665 ns       381631 ns         1815 items_per_second=2.62033k/s
sign<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32      1044969 ns      1044152 ns          677 items_per_second=957.715/s
verify<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32     420790 ns       420301 ns         1653 items_per_second=2.37925k/s
```

### On ARM Cortex-A72 **[ compiled with GCC ]**

```bash
2022-11-14T08:54:10+00:00
Running ./bench/a.out
Run on (16 X 166.66 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x16)
  L1 Instruction 48 KiB (x16)
  L2 Unified 2048 KiB (x4)
Load Average: 0.22, 0.05, 0.02
----------------------------------------------------------------------------------------------
Benchmark                                                    Time             CPU   Iterations
----------------------------------------------------------------------------------------------
bench_dilithium::ff_add                                   6.09 ns         6.09 ns    114807791
bench_dilithium::ff_sub                                   6.09 ns         6.09 ns    114912029
bench_dilithium::ff_neg                                   1.75 ns         1.75 ns    398798582
bench_dilithium::ff_mul                                   16.1 ns         16.1 ns     43482489
bench_dilithium::ff_inv                                   80.9 ns         80.9 ns     10761214
bench_dilithium::ff_div                                   66.6 ns         66.6 ns     10946367
bench_dilithium::ff_exp                                   1482 ns         1482 ns       489292
bench_dilithium::ntt                                     18151 ns        18150 ns        38567
bench_dilithium::intt                                    19226 ns        19226 ns        36412
bench_dilithium::power2round                              8.75 ns         8.75 ns     79993359
bench_dilithium::decompose<190464>                        32.7 ns         32.7 ns     21402509
bench_dilithium::make_hint<190464>                        46.3 ns         46.3 ns     15106041
bench_dilithium::use_hint<190464>                         32.1 ns         32.1 ns     22141716
bench_dilithium::decompose<523776>                        32.6 ns         32.6 ns     21451206
bench_dilithium::make_hint<523776>                        48.3 ns         48.3 ns     14493629
bench_dilithium::use_hint<523776>                         32.2 ns         32.2 ns     21741065
bench_dilithium::expand_a<4, 4>                         184045 ns       184040 ns         3804
bench_dilithium::expand_a<6, 5>                         344965 ns       344963 ns         2029
bench_dilithium::expand_a<8, 7>                         644044 ns       644012 ns         1087
bench_dilithium::expand_s<2, 4>                          26071 ns        26070 ns        28616
bench_dilithium::expand_s<2, 8>                          48775 ns        48772 ns        13414
bench_dilithium::expand_s<4, 6>                          45861 ns        45858 ns        15021
bench_dilithium::expand_mask<131072, 4>                  89970 ns        89967 ns         7781
bench_dilithium::expand_mask<524288, 5>                 121747 ns       121746 ns         5749
bench_dilithium::expand_mask<524288, 7>                 164768 ns       164767 ns         4248
bench_dilithium::sample_in_ball<39>                       2481 ns         2481 ns       280490
bench_dilithium::sample_in_ball<49>                       2567 ns         2567 ns       266763
bench_dilithium::sample_in_ball<60>                       2825 ns         2825 ns       251548
bench_dilithium::encode<3>                                2358 ns         2358 ns       296356
bench_dilithium::decode<3>                                2388 ns         2388 ns       292721
bench_dilithium::encode<4>                                2595 ns         2595 ns       269760
bench_dilithium::decode<4>                                2309 ns         2309 ns       305267
bench_dilithium::encode<6>                                4043 ns         4043 ns       173236
bench_dilithium::decode<6>                                4139 ns         4139 ns       166375
bench_dilithium::encode<10>                               6770 ns         6770 ns       102862
bench_dilithium::decode<10>                               7152 ns         7152 ns       100509
bench_dilithium::encode<13>                              10136 ns        10136 ns        69059
bench_dilithium::decode<13>                               9550 ns         9549 ns        71593
bench_dilithium::encode<18>                              12144 ns        12144 ns        57653
bench_dilithium::decode<18>                              13067 ns        13067 ns        53534
bench_dilithium::encode<20>                              13475 ns        13475 ns        51942
bench_dilithium::decode<20>                              14567 ns        14566 ns        48117
bench_dilithium::encode_hint_bits<4, 80>                  4718 ns         4718 ns       153042
bench_dilithium::decode_hint_bits<4, 80>                   438 ns          438 ns      1572890
bench_dilithium::encode_hint_bits<6, 55>                  6917 ns         6917 ns       101346
bench_dilithium::decode_hint_bits<6, 55>                   432 ns          432 ns      1608493
bench_dilithium::encode_hint_bits<8, 75>                  9357 ns         9357 ns        75038
bench_dilithium::decode_hint_bits<8, 75>                   590 ns          590 ns      1186236
keygen<4, 4, 13, 2>/32                                  549907 ns       549889 ns         1273 items_per_second=1.81855k/s
sign<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32        5390768 ns      5390743 ns          103 items_per_second=185.503/s
verify<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32       653186 ns       653174 ns         1072 items_per_second=1.53099k/s
keygen<6, 5, 13, 4>/32                                  902974 ns       902956 ns          775 items_per_second=1.10747k/s
sign<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32      2802594 ns      2802533 ns          185 items_per_second=356.82/s
verify<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32    1011657 ns      1011640 ns          692 items_per_second=988.494/s
keygen<8, 7, 13, 2>/32                                 1406636 ns      1406615 ns          498 items_per_second=710.926/s
sign<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32      9460843 ns      9460692 ns          262 items_per_second=105.701/s
verify<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32    1575236 ns      1575215 ns          444 items_per_second=634.834/s
```

### On ARM Cortex-A72 **[ compiled with Clang ]**

```bash
2022-11-14T08:56:47+00:00
Running ./bench/a.out
Run on (16 X 166.66 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x16)
  L1 Instruction 48 KiB (x16)
  L2 Unified 2048 KiB (x4)
Load Average: 0.37, 0.21, 0.09
----------------------------------------------------------------------------------------------
Benchmark                                                    Time             CPU   Iterations
----------------------------------------------------------------------------------------------
bench_dilithium::ff_add                                   8.27 ns         8.27 ns     84651330
bench_dilithium::ff_sub                                   7.26 ns         7.26 ns     96368494
bench_dilithium::ff_neg                                   2.18 ns         2.18 ns    321755468
bench_dilithium::ff_mul                                   19.6 ns         19.6 ns     35751523
bench_dilithium::ff_inv                                   61.8 ns         61.8 ns      9037575
bench_dilithium::ff_div                                   68.7 ns         68.7 ns      9689119
bench_dilithium::ff_exp                                   1407 ns         1407 ns       475966
bench_dilithium::ntt                                     16431 ns        16430 ns        42592
bench_dilithium::intt                                    20450 ns        20450 ns        34227
bench_dilithium::power2round                              7.40 ns         7.40 ns     94635073
bench_dilithium::decompose<190464>                        26.5 ns         26.5 ns     26373473
bench_dilithium::make_hint<190464>                        40.9 ns         40.9 ns     17115293
bench_dilithium::use_hint<190464>                         23.9 ns         23.9 ns     29251003
bench_dilithium::decompose<523776>                        23.1 ns         23.1 ns     30355256
bench_dilithium::make_hint<523776>                        37.5 ns         37.5 ns     18658920
bench_dilithium::use_hint<523776>                         22.6 ns         22.6 ns     30938978
bench_dilithium::expand_a<4, 4>                         161154 ns       161152 ns         4342
bench_dilithium::expand_a<6, 5>                         300756 ns       300751 ns         2327
bench_dilithium::expand_a<8, 7>                         566361 ns       566358 ns         1237
bench_dilithium::expand_s<2, 4>                          18140 ns        18140 ns        35704
bench_dilithium::expand_s<2, 8>                          36545 ns        36545 ns        18433
bench_dilithium::expand_s<4, 6>                          36705 ns        36705 ns        19078
bench_dilithium::expand_mask<131072, 4>                  82177 ns        82177 ns         8518
bench_dilithium::expand_mask<524288, 5>                 109515 ns       109513 ns         6392
bench_dilithium::expand_mask<524288, 7>                 153305 ns       153301 ns         4566
bench_dilithium::sample_in_ball<39>                       2019 ns         2019 ns       344407
bench_dilithium::sample_in_ball<49>                       2186 ns         2186 ns       318197
bench_dilithium::sample_in_ball<60>                       2409 ns         2409 ns       294048
bench_dilithium::encode<3>                                2370 ns         2370 ns       295413
bench_dilithium::decode<3>                                2387 ns         2387 ns       292834
bench_dilithium::encode<4>                                2609 ns         2609 ns       268005
bench_dilithium::decode<4>                                2518 ns         2518 ns       276899
bench_dilithium::encode<6>                                4049 ns         4049 ns       172046
bench_dilithium::decode<6>                                4063 ns         4063 ns       172151
bench_dilithium::encode<10>                               6912 ns         6912 ns       101134
bench_dilithium::decode<10>                               6738 ns         6738 ns       103669
bench_dilithium::encode<13>                               8941 ns         8941 ns        77985
bench_dilithium::decode<13>                               8757 ns         8757 ns        79756
bench_dilithium::encode<18>                              12277 ns        12277 ns        56962
bench_dilithium::decode<18>                              12104 ns        12104 ns        57652
bench_dilithium::encode<20>                              13683 ns        13683 ns        51595
bench_dilithium::decode<20>                              13438 ns        13437 ns        52080
bench_dilithium::encode_hint_bits<4, 80>                  4929 ns         4929 ns       134769
bench_dilithium::decode_hint_bits<4, 80>                   362 ns          362 ns      1929207
bench_dilithium::encode_hint_bits<6, 55>                  8643 ns         8643 ns        82580
bench_dilithium::decode_hint_bits<6, 55>                   397 ns          397 ns      1734531
bench_dilithium::encode_hint_bits<8, 75>                 11471 ns        11471 ns        60664
bench_dilithium::decode_hint_bits<8, 75>                   545 ns          545 ns      1255260
keygen<4, 4, 13, 2>/32                                  523754 ns       523713 ns         1336 items_per_second=1.90944k/s
sign<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32        9475761 ns      9475476 ns          100 items_per_second=105.536/s
verify<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32       608768 ns       608740 ns         1151 items_per_second=1.64274k/s
keygen<6, 5, 13, 4>/32                                  853442 ns       853438 ns          820 items_per_second=1.17173k/s
sign<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32      7535700 ns      7535662 ns          100 items_per_second=132.702/s
verify<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32     936799 ns       936782 ns          747 items_per_second=1067.48/s
keygen<8, 7, 13, 2>/32                                 1321677 ns      1321651 ns          529 items_per_second=756.629/s
sign<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32     17228718 ns     17228407 ns          100 items_per_second=58.0437/s
verify<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32    1458385 ns      1458358 ns          480 items_per_second=685.703/s
```

### On ARM Neoverse-V1 **[ compiled with GCC ]**

```bash
2022-11-14T08:59:22+00:00
Running ./bench/a.out
Run on (64 X 2100 MHz CPU s)
CPU Caches:
  L1 Data 64 KiB (x64)
  L1 Instruction 64 KiB (x64)
  L2 Unified 1024 KiB (x64)
  L3 Unified 32768 KiB (x1)
Load Average: 0.15, 0.03, 0.01
----------------------------------------------------------------------------------------------
Benchmark                                                    Time             CPU   Iterations
----------------------------------------------------------------------------------------------
bench_dilithium::ff_add                                   4.63 ns         4.63 ns    151084839
bench_dilithium::ff_sub                                   4.63 ns         4.63 ns    151278508
bench_dilithium::ff_neg                                  0.963 ns        0.963 ns    726868392
bench_dilithium::ff_mul                                   12.7 ns         12.7 ns     54931191
bench_dilithium::ff_inv                                   55.9 ns         55.9 ns     13560621
bench_dilithium::ff_div                                   51.2 ns         51.2 ns     10000000
bench_dilithium::ff_exp                                    889 ns          889 ns       765635
bench_dilithium::ntt                                      5749 ns         5749 ns       121756
bench_dilithium::intt                                     6412 ns         6412 ns       109047
bench_dilithium::power2round                              4.03 ns         4.03 ns    174748010
bench_dilithium::decompose<190464>                        14.2 ns         14.2 ns     49483572
bench_dilithium::make_hint<190464>                        18.7 ns         18.7 ns     37481935
bench_dilithium::use_hint<190464>                         14.4 ns         14.4 ns     48479027
bench_dilithium::decompose<523776>                        15.3 ns         15.3 ns     45839506
bench_dilithium::make_hint<523776>                        20.7 ns         20.7 ns     33779905
bench_dilithium::use_hint<523776>                         15.3 ns         15.3 ns     45825847
bench_dilithium::expand_a<4, 4>                          99025 ns        99022 ns         7069
bench_dilithium::expand_a<6, 5>                         182254 ns       182248 ns         3843
bench_dilithium::expand_a<8, 7>                         339640 ns       339631 ns         2061
bench_dilithium::expand_s<2, 4>                          12517 ns        12517 ns        56374
bench_dilithium::expand_s<2, 8>                          23367 ns        23367 ns        29016
bench_dilithium::expand_s<4, 6>                          22542 ns        22542 ns        30835
bench_dilithium::expand_mask<131072, 4>                  53577 ns        53576 ns        13067
bench_dilithium::expand_mask<524288, 5>                  71916 ns        71915 ns         9720
bench_dilithium::expand_mask<524288, 7>                  93392 ns        93389 ns         7499
bench_dilithium::sample_in_ball<39>                       1220 ns         1220 ns       570655
bench_dilithium::sample_in_ball<49>                       1314 ns         1314 ns       543792
bench_dilithium::sample_in_ball<60>                       1441 ns         1441 ns       498167
bench_dilithium::encode<3>                                1419 ns         1419 ns       493710
bench_dilithium::decode<3>                                1435 ns         1435 ns       488028
bench_dilithium::encode<4>                                1308 ns         1308 ns       535084
bench_dilithium::decode<4>                                1369 ns         1369 ns       513776
bench_dilithium::encode<6>                                2904 ns         2904 ns       241110
bench_dilithium::decode<6>                                2852 ns         2852 ns       244050
bench_dilithium::encode<10>                               4787 ns         4787 ns       146196
bench_dilithium::decode<10>                               4801 ns         4801 ns       146243
bench_dilithium::encode<13>                               6100 ns         6100 ns       114701
bench_dilithium::decode<13>                               7034 ns         7034 ns        99510
bench_dilithium::encode<18>                               7652 ns         7652 ns        91336
bench_dilithium::decode<18>                               8914 ns         8914 ns        78511
bench_dilithium::encode<20>                               8616 ns         8616 ns        82052
bench_dilithium::decode<20>                               9835 ns         9835 ns        71033
bench_dilithium::encode_hint_bits<4, 80>                  2823 ns         2823 ns       248648
bench_dilithium::decode_hint_bits<4, 80>                   104 ns          104 ns      6741069
bench_dilithium::encode_hint_bits<6, 55>                  4697 ns         4697 ns       149044
bench_dilithium::decode_hint_bits<6, 55>                  94.2 ns         94.2 ns      7482171
bench_dilithium::encode_hint_bits<8, 75>                  6203 ns         6203 ns       112658
bench_dilithium::decode_hint_bits<8, 75>                   126 ns          126 ns      5571538
keygen<4, 4, 13, 2>/32                                  250683 ns       250679 ns         2802 items_per_second=3.98916k/s
sign<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32        1773287 ns      1773240 ns          925 items_per_second=563.94/s
verify<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32       288168 ns       288159 ns         2432 items_per_second=3.47031k/s
keygen<6, 5, 13, 4>/32                                  415250 ns       415243 ns         1683 items_per_second=2.40823k/s
sign<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32      2157645 ns      2157593 ns          388 items_per_second=463.479/s
verify<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32     448611 ns       448603 ns         1555 items_per_second=2.22914k/s
keygen<8, 7, 13, 2>/32                                  652465 ns       652454 ns         1072 items_per_second=1.53268k/s
sign<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32      1100444 ns      1100425 ns          636 items_per_second=908.74/s
verify<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32     709418 ns       709398 ns          991 items_per_second=1.40965k/s
```

### On ARM Neoverse-V1 **[ compiled with Clang ]**

```bash
2022-11-14T09:02:14+00:00
Running ./bench/a.out
Run on (64 X 2100 MHz CPU s)
CPU Caches:
  L1 Data 64 KiB (x64)
  L1 Instruction 64 KiB (x64)
  L2 Unified 1024 KiB (x64)
  L3 Unified 32768 KiB (x1)
Load Average: 0.18, 0.15, 0.06
----------------------------------------------------------------------------------------------
Benchmark                                                    Time             CPU   Iterations
----------------------------------------------------------------------------------------------
bench_dilithium::ff_add                                   5.78 ns         5.78 ns    121065390
bench_dilithium::ff_sub                                   5.78 ns         5.78 ns    121132792
bench_dilithium::ff_neg                                   2.31 ns         2.31 ns    302851651
bench_dilithium::ff_mul                                   10.8 ns         10.8 ns     64896884
bench_dilithium::ff_inv                                   51.4 ns         51.4 ns     10880534
bench_dilithium::ff_div                                   50.1 ns         50.1 ns     12887107
bench_dilithium::ff_exp                                    647 ns          647 ns      1063890
bench_dilithium::ntt                                      9874 ns         9874 ns        70867
bench_dilithium::intt                                    11645 ns        11645 ns        60060
bench_dilithium::power2round                              6.55 ns         6.55 ns    106880922
bench_dilithium::decompose<190464>                        17.7 ns         17.7 ns     39502750
bench_dilithium::make_hint<190464>                        32.7 ns         32.7 ns     21378005
bench_dilithium::use_hint<190464>                         17.7 ns         17.7 ns     39501972
bench_dilithium::decompose<523776>                        13.7 ns         13.7 ns     51185628
bench_dilithium::make_hint<523776>                        23.7 ns         23.7 ns     29605477
bench_dilithium::use_hint<523776>                         13.7 ns         13.7 ns     51183664
bench_dilithium::expand_a<4, 4>                          76211 ns        76209 ns         9187
bench_dilithium::expand_a<6, 5>                         141705 ns       141702 ns         4941
bench_dilithium::expand_a<8, 7>                         264561 ns       264554 ns         2646
bench_dilithium::expand_s<2, 4>                           8171 ns         8171 ns        81146
bench_dilithium::expand_s<2, 8>                          18983 ns        18983 ns        37963
bench_dilithium::expand_s<4, 6>                          18563 ns        18563 ns        36896
bench_dilithium::expand_mask<131072, 4>                  43838 ns        43837 ns        15968
bench_dilithium::expand_mask<524288, 5>                  59338 ns        59337 ns        11794
bench_dilithium::expand_mask<524288, 7>                  83438 ns        83436 ns         8388
bench_dilithium::sample_in_ball<39>                        750 ns          750 ns       956256
bench_dilithium::sample_in_ball<49>                        794 ns          794 ns       881712
bench_dilithium::sample_in_ball<60>                        894 ns          894 ns       804892
bench_dilithium::encode<3>                                1302 ns         1302 ns       537269
bench_dilithium::decode<3>                                1396 ns         1396 ns       500894
bench_dilithium::encode<4>                                1450 ns         1450 ns       482791
bench_dilithium::decode<4>                                1460 ns         1460 ns       479165
bench_dilithium::encode<6>                                2184 ns         2184 ns       320436
bench_dilithium::decode<6>                                2507 ns         2507 ns       279363
bench_dilithium::encode<10>                               3640 ns         3639 ns       192636
bench_dilithium::decode<10>                               4210 ns         4210 ns       166267
bench_dilithium::encode<13>                               4727 ns         4727 ns       148229
bench_dilithium::decode<13>                               5491 ns         5491 ns       127336
bench_dilithium::encode<18>                               7271 ns         7270 ns        96216
bench_dilithium::decode<18>                               7897 ns         7897 ns        88660
bench_dilithium::encode<20>                               8087 ns         8086 ns        86566
bench_dilithium::decode<20>                               8787 ns         8787 ns        79562
bench_dilithium::encode_hint_bits<4, 80>                  2909 ns         2909 ns       234302
bench_dilithium::decode_hint_bits<4, 80>                   110 ns          110 ns      6824180
bench_dilithium::encode_hint_bits<6, 55>                  5039 ns         5039 ns       100000
bench_dilithium::decode_hint_bits<6, 55>                   100 ns          100 ns      7070957
bench_dilithium::encode_hint_bits<8, 75>                  6637 ns         6637 ns       104897
bench_dilithium::decode_hint_bits<8, 75>                   136 ns          136 ns      5200018
keygen<4, 4, 13, 2>/32                                  258237 ns       258228 ns         2715 items_per_second=3.87255k/s
sign<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32        1839065 ns      1839002 ns          663 items_per_second=543.773/s
verify<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32       322800 ns       322794 ns         2169 items_per_second=3.09795k/s
keygen<6, 5, 13, 4>/32                                  417447 ns       417430 ns         1679 items_per_second=2.39561k/s
sign<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32      1476029 ns      1476004 ns          739 items_per_second=677.505/s
verify<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32     483674 ns       483665 ns         1447 items_per_second=2.06754k/s
keygen<8, 7, 13, 2>/32                                  636997 ns       636987 ns         1101 items_per_second=1.56989k/s
sign<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32      1369543 ns      1369499 ns          511 items_per_second=730.194/s
verify<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32     739592 ns       739580 ns          947 items_per_second=1.35212k/s
```

### On Intel(R) Xeon(R) CPU E5-2686 v4 @ 2.30GHz **[ compiled with GCC ]**

```bash
2022-11-14T09:05:13+00:00
Running ./bench/a.out
Run on (4 X 2300.25 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x2)
  L1 Instruction 32 KiB (x2)
  L2 Unified 256 KiB (x2)
  L3 Unified 46080 KiB (x1)
Load Average: 0.15, 0.04, 0.01
----------------------------------------------------------------------------------------------
Benchmark                                                    Time             CPU   Iterations
----------------------------------------------------------------------------------------------
bench_dilithium::ff_add                                   5.20 ns         5.20 ns    134428934
bench_dilithium::ff_sub                                   5.28 ns         5.28 ns    132591546
bench_dilithium::ff_neg                                  0.743 ns        0.743 ns    942366202
bench_dilithium::ff_mul                                   12.0 ns         12.0 ns     58383647
bench_dilithium::ff_inv                                    123 ns          123 ns      5875484
bench_dilithium::ff_div                                    112 ns          112 ns      5080621
bench_dilithium::ff_exp                                    870 ns          870 ns       799280
bench_dilithium::ntt                                      8436 ns         8435 ns        83003
bench_dilithium::intt                                     9135 ns         9135 ns        76449
bench_dilithium::power2round                              4.56 ns         4.56 ns    153399674
bench_dilithium::decompose<190464>                        16.9 ns         16.9 ns     41339133
bench_dilithium::make_hint<190464>                        29.1 ns         29.1 ns     24058459
bench_dilithium::use_hint<190464>                         15.8 ns         15.8 ns     44456165
bench_dilithium::decompose<523776>                        17.4 ns         17.4 ns     40252788
bench_dilithium::make_hint<523776>                        30.3 ns         30.3 ns     23060835
bench_dilithium::use_hint<523776>                         16.6 ns         16.6 ns     42180711
bench_dilithium::expand_a<4, 4>                         153407 ns       153401 ns         4563
bench_dilithium::expand_a<6, 5>                         283704 ns       283672 ns         2469
bench_dilithium::expand_a<8, 7>                         527185 ns       527173 ns         1328
bench_dilithium::expand_s<2, 4>                          13248 ns        13248 ns        48098
bench_dilithium::expand_s<2, 8>                          29876 ns        29876 ns        24452
bench_dilithium::expand_s<4, 6>                          28672 ns        28672 ns        23806
bench_dilithium::expand_mask<131072, 4>                  59666 ns        59660 ns        11729
bench_dilithium::expand_mask<524288, 5>                  84309 ns        84303 ns         8305
bench_dilithium::expand_mask<524288, 7>                 130549 ns       130544 ns         5358
bench_dilithium::sample_in_ball<39>                       1255 ns         1255 ns       546720
bench_dilithium::sample_in_ball<49>                       1356 ns         1356 ns       516670
bench_dilithium::sample_in_ball<60>                       1446 ns         1446 ns       482372
bench_dilithium::encode<3>                                1729 ns         1729 ns       404600
bench_dilithium::decode<3>                                1586 ns         1586 ns       440670
bench_dilithium::encode<4>                                1734 ns         1734 ns       404954
bench_dilithium::decode<4>                                1731 ns         1731 ns       404155
bench_dilithium::encode<6>                                3165 ns         3165 ns       221427
bench_dilithium::decode<6>                                3451 ns         3451 ns       203412
bench_dilithium::encode<10>                               5260 ns         5259 ns       132850
bench_dilithium::decode<10>                               5740 ns         5740 ns       122257
bench_dilithium::encode<13>                               6856 ns         6856 ns       102417
bench_dilithium::decode<13>                               7450 ns         7450 ns        93808
bench_dilithium::encode<18>                               9430 ns         9429 ns        74266
bench_dilithium::decode<18>                               9568 ns         9568 ns        73207
bench_dilithium::encode<20>                              10479 ns        10478 ns        66819
bench_dilithium::decode<20>                              11440 ns        11439 ns        61178
bench_dilithium::encode_hint_bits<4, 80>                  6844 ns         6843 ns       102284
bench_dilithium::decode_hint_bits<4, 80>                   231 ns          231 ns      3160114
bench_dilithium::encode_hint_bits<6, 55>                 10257 ns        10257 ns        68249
bench_dilithium::decode_hint_bits<6, 55>                   235 ns          235 ns      2942776
bench_dilithium::encode_hint_bits<8, 75>                 13707 ns        13706 ns        51093
bench_dilithium::decode_hint_bits<8, 75>                   318 ns          317 ns      2216655
keygen<4, 4, 13, 2>/32                                  359601 ns       359569 ns         1938 items_per_second=2.78111k/s
sign<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32        2501909 ns      2501625 ns          391 items_per_second=399.74/s
verify<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32       409996 ns       409972 ns         1707 items_per_second=2.43919k/s
keygen<6, 5, 13, 4>/32                                  596192 ns       596154 ns         1174 items_per_second=1.67742k/s
sign<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32      6075053 ns      6074409 ns          100 items_per_second=164.625/s
verify<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32     650293 ns       650276 ns         1077 items_per_second=1.53781k/s
keygen<8, 7, 13, 2>/32                                  947223 ns       947170 ns          733 items_per_second=1055.78/s
sign<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32      3734376 ns      3734274 ns          187 items_per_second=267.79/s
verify<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32    1044624 ns      1044517 ns          670 items_per_second=957.381/s
```

### On Intel(R) Xeon(R) CPU E5-2686 v4 @ 2.30GHz **[ compiled with Clang ]**

```bash
2022-11-14T09:09:31+00:00
Running ./bench/a.out
Run on (4 X 2300.25 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x2)
  L1 Instruction 32 KiB (x2)
  L2 Unified 256 KiB (x2)
  L3 Unified 46080 KiB (x1)
Load Average: 0.36, 0.29, 0.13
----------------------------------------------------------------------------------------------
Benchmark                                                    Time             CPU   Iterations
----------------------------------------------------------------------------------------------
bench_dilithium::ff_add                                   6.31 ns         6.31 ns    110624151
bench_dilithium::ff_sub                                   6.31 ns         6.31 ns    110932527
bench_dilithium::ff_neg                                  0.990 ns        0.990 ns    707180491
bench_dilithium::ff_mul                                   12.5 ns         12.5 ns     55845941
bench_dilithium::ff_inv                                    167 ns          167 ns      4687848
bench_dilithium::ff_div                                    152 ns          152 ns      4878713
bench_dilithium::ff_exp                                    771 ns          771 ns       926612
bench_dilithium::ntt                                      6875 ns         6875 ns       101485
bench_dilithium::intt                                     7506 ns         7506 ns        93304
bench_dilithium::power2round                              3.90 ns         3.90 ns    179845859
bench_dilithium::decompose<190464>                        13.2 ns         13.2 ns     52951200
bench_dilithium::make_hint<190464>                        22.7 ns         22.7 ns     30837604
bench_dilithium::use_hint<190464>                         11.4 ns         11.4 ns     61461667
bench_dilithium::decompose<523776>                        13.5 ns         13.5 ns     51709300
bench_dilithium::make_hint<523776>                        24.0 ns         24.0 ns     29189437
bench_dilithium::use_hint<523776>                         12.2 ns         12.2 ns     57535171
bench_dilithium::expand_a<4, 4>                          70846 ns        70838 ns         9836
bench_dilithium::expand_a<6, 5>                         132788 ns       132786 ns         5263
bench_dilithium::expand_a<8, 7>                         243098 ns       243087 ns         2881
bench_dilithium::expand_s<2, 4>                          10955 ns        10954 ns        60935
bench_dilithium::expand_s<2, 8>                          23431 ns        23428 ns        30576
bench_dilithium::expand_s<4, 6>                          24164 ns        24162 ns        29664
bench_dilithium::expand_mask<131072, 4>                  49189 ns        49187 ns        14226
bench_dilithium::expand_mask<524288, 5>                  66956 ns        66952 ns        10451
bench_dilithium::expand_mask<524288, 7>                  94037 ns        94027 ns         7456
bench_dilithium::sample_in_ball<39>                        848 ns          848 ns       783080
bench_dilithium::sample_in_ball<49>                        941 ns          941 ns       720022
bench_dilithium::sample_in_ball<60>                       1131 ns         1131 ns       618390
bench_dilithium::encode<3>                                1479 ns         1479 ns       472903
bench_dilithium::decode<3>                                1600 ns         1599 ns       435922
bench_dilithium::encode<4>                                1247 ns         1247 ns       561347
bench_dilithium::decode<4>                                1351 ns         1351 ns       517176
bench_dilithium::encode<6>                                2645 ns         2645 ns       265215
bench_dilithium::decode<6>                                2906 ns         2906 ns       241722
bench_dilithium::encode<10>                               4409 ns         4408 ns       159292
bench_dilithium::decode<10>                               4897 ns         4897 ns       143262
bench_dilithium::encode<13>                               6630 ns         6629 ns       105872
bench_dilithium::decode<13>                               7059 ns         7058 ns        98858
bench_dilithium::encode<18>                               7902 ns         7901 ns        88588
bench_dilithium::decode<18>                               9100 ns         9100 ns        76963
bench_dilithium::encode<20>                               8777 ns         8776 ns        79734
bench_dilithium::decode<20>                              10168 ns        10167 ns        68865
bench_dilithium::encode_hint_bits<4, 80>                  2726 ns         2726 ns       260335
bench_dilithium::decode_hint_bits<4, 80>                   152 ns          152 ns      4503166
bench_dilithium::encode_hint_bits<6, 55>                  4571 ns         4571 ns       154582
bench_dilithium::decode_hint_bits<6, 55>                   154 ns          154 ns      4584453
bench_dilithium::encode_hint_bits<8, 75>                  6067 ns         6067 ns       114564
bench_dilithium::decode_hint_bits<8, 75>                   205 ns          205 ns      3490660
keygen<4, 4, 13, 2>/32                                  235203 ns       235184 ns         2978 items_per_second=4.25199k/s
sign<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32        1111823 ns      1111717 ns         1305 items_per_second=899.509/s
verify<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32       280869 ns       280836 ns         2492 items_per_second=3.5608k/s
keygen<6, 5, 13, 4>/32                                  384327 ns       384324 ns         1825 items_per_second=2.60197k/s
sign<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32      3597811 ns      3597619 ns          195 items_per_second=277.962/s
verify<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32     428562 ns       428536 ns         1634 items_per_second=2.33353k/s
keygen<8, 7, 13, 2>/32                                  580192 ns       580164 ns         1205 items_per_second=1.72365k/s
sign<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32      2247248 ns      2247236 ns          312 items_per_second=444.991/s
verify<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32     651674 ns       651660 ns         1073 items_per_second=1.53454k/s
```

## Usage

`dilithium` is a zero-dependency, header-only C++ library such that it's fairly easy-to-use

- Clone Dilithium repository
- Fetch only dependency `sha3`, by enabling git submodule

```bash
pushd dilithium
git submodule update --init
popd
```

- Write program which makes use of Dilithium {keygen, signing, verification} API ( all of these routines live under `dilithium::` namespace ), while importing `include/dilithium.hpp`.
- Finally compile your program, while letting your compiler know where it can find Dilithium and Sha3 headers.

These are the namespaces of interest

Namespace | What does it hold ?
:-- | --:
`dilithium::*` | Dilithium key generation, signing and verification algorithm implementation, which can be parameterized to act like either of Dilithium{2, 3, 5}
`dilithium_utils::*` | Several utility routines, among those worth mentioning are routines which can be used for compile-time computing how much memory to allocate for storing Dilithium public key, secret key & signature, for specific NIST security level parameters.

> **Note**

> Find Dilithium parameters in table 2 of [specification](https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf).

**Dilithium DSA API Usage Flow**

- (i) Generate key pair using `dilithium::keygen<k, l, d, η>(...)`
- (ii) Sign N (>0) -bytes message using `dilithium::sign<k, l, d, η, γ1, γ2, τ, β, ω>(...)`
- (iii) Verify signature using `dilithium::verify<k, l, d, γ1, γ2, τ, β, ω>(...)`

What does it do ? | Header | Namespace | API | Inputs | Outputs
--- | --- | --- | --- | --- | ---
Key Generation | [keygen.hpp](./include/keygen.hpp) | `dilithium::` | `keygen<k, l, d, η>(seed, pubkey, seckey)` | 32 -bytes seed | Public key and secret key bytes
Signing | [signing.hpp](./include/signing.hpp) | `dilithium::` | `sign<k, l, d, η, γ1, γ2, τ, β, ω>(seckey, msg, mlen, sig)` | Secret key, N -bytes message s.t. N > 0 | Signature bytes
Verification | [verification.hpp](./include/verification.hpp) | `dilithium::` | `verify<k, l, d, γ1, γ2, τ, β, ω>(pubkey, msg, mlen, sig)` | Public key, N -bytes message s.t. N > 0 and Signature bytes | Boolean value denoting success during verification

I suggest you look at following three example programs, which demonstrates how to use Dilithium DSA API

- For Dilithium2 ( i.e. parameters providing NIST security level 2 ) see [dilithium2.cpp](./example/dilithium2.cpp)
- For Dilithium3 ( i.e. parameters providing NIST security level 3 ) see [dilithium3.cpp](./example/dilithium3.cpp)
- For Dilithium5 ( i.e. parameters providing NIST security level 5 ) see [dilithium5.cpp](./example/dilithium5.cpp)

```bash
$ g++ -std=c++20 -O3 -I ./include -I ./sha3/include example/dilithium2.cpp && ./a.out
Dilithium @ NIST security level 2
seed      : 66431d9b73cabf480808dc7a367f9cbc2772e91002027216c5b6f64905cdb008
pubkey    : 5e45fc0f989da464c78c3902c73f5d24ab4d5fc7dc12c4e100499e1a7cad96df7a44cba5c3d9f7dd40cdeeb88d549e9075653b7a27c86df19004d20acf9001cdc4d78036758aa38b64316d43432b7a9ed99350f5bc10b382d402625d666c0b50243f479da1d1f5d08d75ce226a3477e06a130b0cf5bd2026ebfe55e1e7bc83108f4e94e3ff90f6f25d7fe62fb0547b86c3dfd76c04da17323358b8fc61efe8e3caaacde0ebd862d7af3f580c18c43526c48d7a38903c271d4693a48408a242b01cda34e08f20750e12713e2db25855a1d7d725f702f853eb20175dfec28aa5e010204bcb0d4d544e5a7b5bf4f0ad1da8630a6f664dfdf3e0c64ac5da8ea242214186707ecc775959b9c384dab883df415ad10466c68de85856609f551dec408599c8f84d0d39fbf73667a22d5e82efce9374e09c9f32615f81fe4c9eb7c9bc5a6725a86006cfef121e4a9e5f152d0927bbe436856500fa215e313645661f307d7de4f0a0d712792ecba39bd11c444251115dc9e15bba129e92289b27c57561a34524efbd23568b99cf1a08d9ea0d48fd0796bfc7bcb87c909cd19c726633da879559187e9d5f71ca3e141872bd94a197f2301da444088b6caaa20134254d48e406de92e88a9ea35c1d7be8fff318bed1068ce3e4120f8041cdc3c0881b771b453d6c80f257840c476d1d367245c37649f7c7e38eb6896c77c1316e9fe1c0c876c5bac7be7873ea80d885f2b9b8eb5ed7426726974a3e7d91ae8f4acba4ebe723db638dee7a47d08cf446373fcb934cdfa10b77d7c2d01b526c58b6dbccfffad683e9207b56684d1fafd2e6352a56c94abd298daa13c7c2d08e1717b70663a85eefc12dfae934a1098e471def6507d671bd706c8ea86a599ec8b9aeb2aac2785853a5b934665113d1d4553cc1971a2b71f206fba6ba68fc63669a5b82dbb7ba64885ebb81134d7834056825da407f81fc5cde5c08c41ca5c5f7a2094a34949d8d545f115412fd7d702479cdd06eca97122515be7ffd98cae36ec482d1e16e1bd4e926e4f5b5f68b695aac5749694f401bd388e5e82112d0c07654190d510915cfc0394e43fb676a24ca0b4d4a902392c82537e72963b25b68d70e31922839374b7aabba5eff13f391b16a26481bcb2813766c14f96fec2eb0130cae84a0858dac9debe27bc2ebdfd39f942730d0b6674f2e2460f2881a4233c790ba084a409c63c437b06d306c3366b18b2aa397aa7f6ce0d9f7188f2fa23a52032469a9b8a6e6a1a05c3b570126f999c97787a42126bf324715e749a5021536552c5604fbaa74f862f5f77a0b9582548e994965c8d6c12bd4629039d6443ad166e7b27aa4d4ab2a089b8a997ab813e7a0fe2bccf1b0bccca9dc043941f5917fcd6689e7ec59644d1013392a788549d458956b497afaa24fe936bb478037e6d9b573edbae80660b922d331cb06ba8570df32ae222726a6a9ef14dc0a4c7f5707d99cfbed10cdf604c3ab5d695ee7b0b675e8c9453aff75563490e3e92acc51dd692d29da6945661d398825f07761b35cbd2d90c7f430e1d899f274de73df5569b7abd54009e16c2b062aca866dd78cc2b44a2c387e4210e6259f187445889a98c1167db49018c606106db824eaad0a328096c7d0c018dcb569a8908c7b88600d49c74db878c054f61f1e9cfb532fb8fe0714c96056e03bb4fb7f2f4de98b0c9e4d6f33a2ec8b60d308e010a52cfa5c6af1f05322ac0ec259d6cd322ad3aad553265491e313f867ed0bf5ab2f879c6e64208c404320622a2e562454e4ee26ba27a47ecd3ccba38355ba9094722b03a9222a454334c124bdd35186f26ad6415f7be795603f9f4e07
seckey    : 5e45fc0f989da464c78c3902c73f5d24ab4d5fc7dc12c4e100499e1a7cad96dfa29f9939512851d70089a4526d82a18acdf1873783221406f36e3a4cfa08812d09f3f0d50c5e2b5472b3549d2de5e5ddb3d092cfe8185ff8bc4a5e710616def60ab48148b80d1ac248d2468ca11005e4320613102512c5885126911b234c01826141128d49180e00a16c632492800262944428c1b6311a958c13926cc2228641866c5132250443860225451a2809410809a114121b02821c200841c80001c30044b82d59c610903672c0904d9188200144041a93045bc66444a4314bb669603080238100991802014564c8162263060d61204842224cc408904ab270da120844b685d442045208311283898b46684ac22ddb9221a324694a188601110c5c468181842940802501805008352e93a40d0396712227441336421b15459cc009934692242700c1866c9c9411930242c84272231420a2c82d0b463189283099468113c76542464ca1228ad318260c286dd4324084868982b42181a82c11322a4900425040055ab2604c2650423866e3226113312c423266598284db306a121951c2800508c591c3844d193380a3a8601b0226c3282e04c008db2084024088083489101341c0384062200d62b820d0b285c910611a360c5ac031481849441431e4a28409940851a28453a421e1200281c0605a1270523641a480809b148ca3265052b20d4422462419481a386e5b426c1b152602c090c10201843429e02205cba4288aa2298ba48121b068092690e0960d53906d5a84255402061b910c0b01699b1480013265d4364e44206564265061004c0b444d00384501a66c0b829004a51124480222490121b04913092151265021914411c96c0a36720b2120d24631d2400a1041800c99291894088a300e19912561448e2181091bb3305486400ab5411c38869cb44c92148e03486c43b260cc260080c25111a80191266842b04124b78904a304223065e3008c823864cba44d104045a1c86c94224262c07011b068cc207020c02961484d8b008a9180840b972013310a9a10910bb4685c3842182369c81028cba665c994111a072e1497451997450222654184481326490a152d88184c49c009d0929110312ca49069044310228550844642c124109cc40d11a685e0220600322620088e49324ca3b264c9b641c348689332044896848b4432a4a8440b4949d18ad62b5e1592b3f232ee43fc96d88e5606be9e9a7fe42aa360d319521114b3d17b480393bf68ced1135ec6c01b05a154a554dbdad8f4a6379e8bd92beca8bc7fb9053e0c49aeaee6810d10fb5ac990301275ccd33114a54594f8993bd6076f2a66e68a78bd662a90c9eaf3cfd286c7486a89937cddb4f823c949e8ed66975cadf6001393a62dec3622f6369ec2365585b744cb53058cb819b9362647529af98c84cea2f86326b07a2444f722f518848cc3672a44c5c1c6f4ee89a411a54ffd5a71b3dde5abf3e5eb47cb5d14b921a9d538f97722fbc8183de2c8b9f230eabd06d5c51469d234854bce7dfee9fb5ef61d41ab689e29b103ec102e6f1358e0dc17b0b129cebd1e298c21b6412fa52c5843d517edcf156224a21174865723b6e34a84167ff0a43b8e063b54cb3a0c3eac7e524f41545b98c7f36ce8e3567fc9bc5f9319168ebb133021ff07e091bcdb525343597725df4eaaf7a4386618e74028b07bd1574d0bd5f31df2a00dc3d656ade0663b06032d0cf66ce213f84110804e92b6c17c049edb975cdb28052e7348a916caa4a90045b3be872b2403299b2f1b5710235ff7742cebc54d5af85c10ddb8b3bf06075b6dd1f94961db13f13034a7dec39d1e8e875b198be596c2df7f6fd611a46aae8e622dec963055700a396fa25e3a87de9f67dfa532dc81c53bde853d25defb9bc437e3a8d576f1f1b883a8c5f2de6286aef308f8a159f724e4f72a9afeafd8ea638dae797c72d75e890d48546c9c4a98d157c1a2df5729d0e83b5ba2a363bc9c17ba7a4838bafb8a316c34788c10f21193d5c04595d5202ebc94fe477ac1aad036748ae25a0e21ac2d0b733be60f59094823cb9ace4023e0925e5834e8d06a366bbced957e60059a56a2bff4cf2704e5fa0cbd89c7ab4b2b30f82e59162acf2527fc9fbf264e01fe2bcd59e323760984e044ccda664f3f54cdbf40c204b693e1bf2476104abb91e0e0faf860e422f55ccb62e3db6533e3cf5ca1434f1e7d53ba394bcf22206549925fcebc1951295eb00a4370a8f2b3edcacacf586a189d7e42554168db6124e1fa59206169d52700cddf049e72d2a0c3c2cd634ec10083147a88bc29259717d652463108369d800cc4e39d3d0a4e63aa957d785370453fdba05d8efa41f015a0cd785ff60c62e7c9c84a76fade9217bc85c59a6de8eef69bf22ce16e5e9501b1e65d37aed2c18758d44c5f8c58bd9a997cf891d9500d82fbe53adfc91a37391b67bdcdcb1b0152005362117bc444245c6cdd2dc091e7334a3f366220ac44f900d919ec9a1509104dc6c8ac2a84ee05f41d244943432564bdc3ea6a1ab49c4a7109425aee30230a7b9cb5654de792e31604b8fedc535747db6732574102ce448dabca684691a40c0028b17b973e82060e390a13f2fc6c24243f8371eb8170151c06e8e2f03476b2b73f2efa87d34cb6da09362c8bf9ea116f2ab1cf54f9a41a43e3bbd511df8f5c8148f3b538ed90c70e9c919572025a7022380b6b7825614f6ab050422d3a376e6d56470d755af32477eb18dbe38860743d5622f3ba988ad54fb55db70073a429c84db3c060b5f7143b489eec927814dd0ed8f8c379da918239f1d4fdcbac6e65486084484320f60f1c51e67476e875f8549bba3a137ff6dc9a02fb59742323dbde614819a47e05d5a83a45854f9affb393f6d4bcab978ee6ffb8df1a9a3a0d8e531c9dd94df299121647b1d14da7c0a523d638224f41d7bd1c12445f6699a6c28768f9f6e31284987908bb1030817856158ea4f9c9c259e867187a02c0c375d3479a7bfaf4e97f59d53eb567a402c09adf8e80934e9a7c917b1e25b50222d269ba4d2b4606266ebd16b29864e1785b8866394759f0a9feabf51be43d98fa457ddca77fa3046f1420e00b8a26787c15948792004b113f64e3bf22b386be2b87338922f6f277eb392d68318598555823315ddde630b0f33c066730075a8e4dfe91de57d2eeee754e390fa24812ff59a7f51a79ddb79cf947e3f23504903447c59aa31ae858afc2d2c28e9152f319af006e249cfa1912bb90f14b74779a71b249b07c2fc4bfc3377dcc566b5508ab21d28577384b14891c34f99b13022b8ad9455d2e7f02bcb7d218ece2b228f076818c710b757f0b30226d47a05e3a1bec0535449449ce06c5172c04060f89a4ebec6748190fc47935842aada7de3293a52a0ef4ae136d79e8f22871a050c3ab6d5233ad6f914becbf58334e13ec68ae78d02959106a0b7c02b6d52b5889ec07fdd188babbb5b3d7253bb8721322e3fab09dc1ee8e5c8675180caf202d4a8b6571acc71952934ffd3676fac8b32043a1346b
message   : 7f244373d8af9b9c12ac5ed139804872d5b86d85eae11dd23a66ecbf2d396725
signature : 97a3eaff2cce526223227d857dfc7e9d47213cad12d21cb326c1c55c250a5cd10c9c5a9c391ebb491142ab93e362f9b33667e744d3c1f58e42e212bc5673df632a52d60459310a7d8602998a6e34cdd80ad8ed2c67c5c339460adbe0aeaf70830f62b629cfb0dda60355f51851f4d2e588e00f6c37f08eeba5bc028dc63643b742c6af24e9c981d7fad0b856f77b1f93d45d944da3fe7544015c868c735bc30ca39145b8eb7b370c807e9d4a55c5f203db6d51be6c395f0c3611023c8a8a8a8b4d6b72b360ff087c7b89da4e41b9447a72ef6c967e4a80f70ff830714359f24e79d8a5121069ad56b0823bab2afcb745373c91a7372ac5ffbca6f3cdb5b181c5e0b5fbbe6abe1052aa7980f5f7009d8f7a0291b741cd08cd4b65f980bd99ed5ef4a35be28ebe1f833e239845265636542335da3f8d1ffacdf7b18119495f0e06d82188f523ca2b9c1c20b424cea293c4b2b63f142a7973972682229f86713ef99ae305084d6e72479e99ba7251e7f6d5795f2a1f7ba87c56f5f6661260e17b8769ada07d6cfaca1d36733f96d91849c4e5fa7732a43aba7eecdd34e6a429dfd2cffc9a31408fbba6d9baee7e10462b01343d0824ccaabe5f32ba128af2c754b487006adac4121371573eb039f21b5c1fd7010024cb2ab6422dfaec86ef641cfe24e14226074ea454215182bc240ae970ae3aa0eb0e7ae34451755e4e0e2daa436c616ab3619edcdb324967806706368565176a01c8683901edd5a2f4f6cb3e36d21a928e82092badbbdeaf1f4ffc1174c1f164c049febd179f0dad75cd6288dfbadb2cec2e5dd151a59d8c6c9ba12522c1e969b821ae45b46dd4f1f6e90d77d0bdabee2707d5485d25c5acd6fa17ee996001dfe7c69b723078fd4d1a543ab4e586d9d98c11c1d6b4f2ba983b9506da6f60ed9e146514fe64f4b7759016d925f340321949632ba47a87066e42f89d1bcf26da930f42eb367f0fcadafb64da503c5502a07f11d17504ba9d92432f8b145a6f942f95f957f50701e94405295af59930167ff75270b9b0e5a255d22972e6fcc0f597c79795e14ec997b32a27e6edcca6adafbc755f72da9f5c5c019a2828eee6ea2735cb6c85a729cd61c123d4925d6ba51fe897aaff696f2b8e574e42947900246e4fd13de44d6e4b384780f2eed0626c30803d9e678969baf9fce2ea946c54017e6b9ec241d03ba5eacab0c45909ddd7cda542f7fca7362568fca258b465dea34644c464ac97a680e53ea5d3f375cf19227a6c19950d0d745c6c923be7374c37391ece778ecdc9631abfd9e589076ca4fef74074ef3984c8777b152117817a570f7ab1d17e28980f3d66434e7cff3f7369055875d87a04f4955648834d8bd71d01c2ce202f903f2d6e7ffedbad3faf7602ad80630d6fc9089615ef076405f2f457b6696635a51d5936ea823776da5cfca1bf82e565e8647e315adec048fb79916f348d8d9ec67e9f0273ca10f9bc4af915ea6db45ed3cd62874fc4753bdd3f87e2617f8d15373720a679b6aaabd64e5e2ba2606a0c4b95998fc63e91f25f5cd48628dbe1e94161e4eb65866f297fca3eee52c78ff1c259a46174e9268930f53da3755a0729bba229efe875db9e19818870178126fe2df88f674c356459877df501a71a115b53bf4ab74eafae0b7310c82934e00b4503b0ee6693fa078fa4ed0859b72b8fecb33095195ef7919ba634af92c26cde202080c314c351f6e6bcf2759a69b3488ec9af93012fc544e8ec46a9dd0de4de71c2f1e0d4dc5e6d0f67e188f27bc91b3c480b3c25cc7b9ef532da201a3294e34062e5b351b482df1fb1531e1b781a45a96e7af12c590e90663fff82b220523d2baf6480a2ddeed85de47cbe6468d6a97a3a17425c6392d7509be05797f0c85c3a7d854ae235a2e069ab6aa8e35b9d9322828ba448aa0527e675451eb3659cb57f32d08397c07967e7e486c9b852d69753e7372b230ae464c26f0337bc001351e469d10076f1a10fee8cdfd71c49767f22b9abf64c7c0e15931a4a7676e6559aa8c4337c06bb4c859ebadd033fa9cbbb9d80908b1f2c1fc4fc745a5ee6bc3506550e65bb122b0eac976f85a24d369c5b739d03e1eb5a45987e5364407fccd42680b3d58da1c8286f0380e3e5f9a81a569fcf8a68c43d8755e378b85721c85d07f2ed3e55375faeeaeaf457198b386f5d7ccb9e27fee3cc59ff7b9ce2c480f27e7ffb8b1f80b779d741251b55dd9e335b9c5a077d3dab73519f2aa6983073070cba224e54d55fb9662c414a9f4c4fa5b3d4a6428b171bba0f1f701da31aefcafa8ef34a9142ea381ced089769332080da3bab7e34da39f5c29087ef6ce6cad5847aa05ab563a8f0938dbace2cd4da1456f841c754107d3887d7d3b253306e08ddbeaea85c4f25e45ffa0fa06e132f2269746c90dd41199da1de78d9b9f6bbb2a48b9775623ab6d256fb8dcfde036633eef0bd7984b094fe318bef2ef364cfe73c2014e353dd66a9552a56d707a5609fbd62274216ba50a22978972f852ea6977f2029f7dfed01d1eab95e58e60348e40618e9ef363df4f02d53128cb1b8cc2d5f85356b0a833b888a8e5668f2b68b948e8cd09e3677a1bdcb521976c7268bd951a99d280fd0ebfc9b8630ce581d789b092230d5788433a418181caa6236cc02b107fca64d8181340a395c01c072f439bd63f8baa395514952396e0f092c66a7e8c73a88b082cf309f9631240ad33c0a6884edd407b5003054479c228d29869224e17efe0cee0428f6e268ab6378571b983c916f8891a3e2e9f6ea7ff63585bd43f0a88e99906362c10b97f19cc0f9dc87f67389b130ce3d0abc1d521fa5a32c21cf0e6533a2fc374811c846a9f4a2230454642c0abb3241b7bcd7c5ee659be53d0817cb94d5f21b51b58b094e726a93ab256cb430689dd4966d2f9ee2d25ce16445401c539567e2a5a808c84d0850330f1b4011a54413b7f8b44c1f013d1f2a85ecefc767d872402c4e67943afc5207905f5c7ef6bd891944d7c20d9c2afc79b75b289e90dd0cc3ce8077bca2f4497e5f58ccb7fbc02af08e6f3bdc78a258953c08bc223c667e3c51b5a8e0d7c8f4668b968a91531b5369ee973a89e8eb54ddd7b731bbf6bc18330148c156d8abd9ce2cbf83122a403dc187244a82bdd2bca902755baa3f64a470170d41e06b4e7fa5d3185ae13db117a52998445bb3ae8939eef408bc36f2190f46b7ce06fcc1f50d7211691e38fb199f889d310bb82e0ff5dd47ef550d5ac91f96bf53b0d4b95f4534658b350e8260810fc3e51c333550535c63a4afc6d00f191a28374860a1adb4c5d0f8fd3242466482909b9eb9d6ff04050c16203d3e44465159676a718b98a4a8b6ced7d8d9fd00000000000000000000000000000000000000000b19243c
verified   : true
```
