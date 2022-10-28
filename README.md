# dilithium
CRYSTALS-Dilithium: Post-Quantum Digital Signature Algorithm

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

### On Benchmarked on Intel(R) Core(TM) i5-8279U CPU @ 2.40GHz **[ compiled with Clang ]**

```bash
2022-10-28T15:04:18+04:00
Running ./bench/a.out
Run on (8 X 2400 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB
  L1 Instruction 32 KiB
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB
Load Average: 1.73, 2.19, 2.15
------------------------------------------------------------------------------------------------
Benchmark                                                      Time             CPU   Iterations
------------------------------------------------------------------------------------------------
bench_dilithium::ff_add                                     3.79 ns         3.79 ns    182527907
bench_dilithium::ff_sub                                     3.73 ns         3.72 ns    185763115
bench_dilithium::ff_neg                                     1.24 ns         1.24 ns    558636926
bench_dilithium::ff_mul                                     5.94 ns         5.93 ns    114309976
bench_dilithium::ff_inv                                      103 ns          103 ns      7490316
bench_dilithium::ff_div                                      121 ns          121 ns      7342221
bench_dilithium::ff_exp                                      331 ns          331 ns      2116069
bench_dilithium::ntt                                        4688 ns         4684 ns       148122
bench_dilithium::intt                                       4369 ns         4366 ns       158209
bench_dilithium::power2round                                2.34 ns         2.34 ns    296699431
bench_dilithium::decompose<190464>                          8.31 ns         8.30 ns     83003296
bench_dilithium::make_hint<190464>                          13.6 ns         13.6 ns     51100112
bench_dilithium::use_hint<190464>                           7.94 ns         7.93 ns     85610156
bench_dilithium::decompose<523776>                          8.73 ns         8.73 ns     76990761
bench_dilithium::make_hint<523776>                          14.6 ns         14.6 ns     47704071
bench_dilithium::use_hint<523776>                           8.40 ns         8.39 ns     82591972
bench_dilithium::expand_a<4, 4>                            52880 ns        52842 ns        13021
bench_dilithium::expand_a<6, 5>                            97096 ns        97068 ns         7056
bench_dilithium::expand_a<8, 7>                           184330 ns       184241 ns         3749
bench_dilithium::expand_s<2, 4>                             7001 ns         6996 ns        92368
bench_dilithium::expand_s<2, 8>                            14842 ns        14833 ns        48996
bench_dilithium::expand_s<4, 6>                            16602 ns        16589 ns        42446
bench_dilithium::expand_mask<1u << 17, 4>                  30408 ns        30387 ns        22987
bench_dilithium::expand_mask<1u << 19, 5>                  41710 ns        41662 ns        16932
bench_dilithium::expand_mask<1u << 19, 7>                  58067 ns        58014 ns        11630
bench_dilithium::sample_in_ball<39>                          622 ns          621 ns      1104153
bench_dilithium::sample_in_ball<49>                          658 ns          657 ns       998901
bench_dilithium::sample_in_ball<60>                          813 ns          813 ns       879264
bench_dilithium::encode<3>                                   961 ns          961 ns       716817
bench_dilithium::decode<3>                                  1018 ns         1017 ns       678006
bench_dilithium::encode<4>                                   833 ns          832 ns       824781
bench_dilithium::decode<4>                                  1025 ns         1024 ns       671927
bench_dilithium::encode<6>                                  1698 ns         1698 ns       407500
bench_dilithium::decode<6>                                  1828 ns         1828 ns       377470
bench_dilithium::encode<10>                                 2889 ns         2888 ns       240638
bench_dilithium::decode<10>                                 3117 ns         3116 ns       223223
bench_dilithium::encode<13>                                 4322 ns         4318 ns       162480
bench_dilithium::decode<13>                                 4657 ns         4655 ns       150425
bench_dilithium::encode<18>                                 5008 ns         5006 ns       135946
bench_dilithium::decode<18>                                 5767 ns         5764 ns       118674
bench_dilithium::encode<20>                                 5540 ns         5534 ns       125029
bench_dilithium::decode<20>                                 6433 ns         6430 ns       105815
bench_dilithium::encode_hint_bits<4, 80>                    1608 ns         1606 ns       451566
bench_dilithium::decode_hint_bits<4, 80>                     106 ns          106 ns      6642312
bench_dilithium::encode_hint_bits<6, 55>                    2604 ns         2602 ns       269188
bench_dilithium::decode_hint_bits<6, 55>                     107 ns          107 ns      6377261
bench_dilithium::encode_hint_bits<8, 75>                    3460 ns         3458 ns       201914
bench_dilithium::decode_hint_bits<8, 75>                     138 ns          138 ns      4886255
keygen<4, 4, 13, 2>/32                                    154891 ns       154764 ns         4419 items_per_second=6.46143k/s
sign<4, 4, 13, 2, 1u << 17, 95232, 39, 78, 80>/32        1566040 ns      1565175 ns         2046 items_per_second=638.906/s
verify<4, 4, 13, 2, 1u << 17, 95232, 39, 78, 80>/32       185756 ns       185661 ns         3759 items_per_second=5.38615k/s
keygen<6, 5, 13, 4>/32                                    251367 ns       251253 ns         2774 items_per_second=3.98005k/s
sign<6, 5, 13, 4, 1u << 19, 261888, 49, 196, 55>/32       732057 ns       731683 ns          957 items_per_second=1.36671k/s
verify<6, 5, 13, 4, 1u << 19, 261888, 49, 196, 55>/32     279313 ns       279047 ns         2501 items_per_second=3.58363k/s
keygen<8, 7, 13, 2>/32                                    393138 ns       392421 ns         1791 items_per_second=2.54828k/s
sign<8, 7, 13, 2, 1u << 19, 261888, 60, 120, 75>/32      1064477 ns      1063465 ns          957 items_per_second=940.322/s
verify<8, 7, 13, 2, 1u << 19, 261888, 60, 120, 75>/32     434996 ns       434849 ns         1610 items_per_second=2.29965k/s
```
