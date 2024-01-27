> [!CAUTION]
> This Dilithium implementation is conformant with Dilithium specification @ https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf. I also try to make it timing leakage free, using `dudect` (see https://github.com/oreparaz/dudect) -based tests, but be informed that this implementation is not yet audited. *If you consider using it in production, be careful !*

# dilithium
CRYSTALS-Dilithium: Post-Quantum Digital Signature Algorithm

## Motivation

Dilithium is one of those post-quantum digital signature algorithms ( DSA ), which is selected by NIST for standardization. Dilithium's security is based on hardness of finding short vectors in lattice i.e. it's a lattice based Post Quantum Cryptographic (PQC) construction.

Dilithium DSA offers following three algorithms.

Algorithm | What does it do ?
--- | --:
KeyGen | It takes a 32 -bytes seed, which is used for deterministically computing both public key and secret key i.e. keypair.
Sign | It takes a secret key and a N (>0) -bytes message as input, which is used for deterministically ( default )/ randomly ( in this case, you must supply 64 uniform random sampled bytes as seed ) signing message, producing signature bytes.
Verify | It takes a public key, N (>0) -bytes message and signature, returning boolean value, denoting status of successful signature verification operation.

Here I'm maintaining Dilithium as a header-only, easy-to-use C++ library, offering key generation, signing & verification API for three NIST security level ( i.e. 2, 3, 5 ) parameters, as defined in table 2 of Dilithium specification. For more details see [below](#usage).

> [!NOTE]
> Find Dilithium specification @ https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf, which you should refer to when understanding intricate details of this implementation.

> [!NOTE] 
> Follow progress of NIST PQC standardization effort [here](https://csrc.nist.gov/projects/post-quantum-cryptography).

## Prerequisites

- A C++ compiler with C++20 standard library such as `g++`/ `clang++`.

```bash
$ clang++ --version
Ubuntu clang version 17.0.2 (1~exp1ubuntu2.1)
Target: x86_64-pc-linux-gnu
Thread model: posix
InstalledDir: /usr/bin

$ g++ --version
gcc (Ubuntu 13.2.0-4ubuntu3) 13.2.0
```

- System development utilities such as `make`, `cmake`.

```bash
$ make --version
GNU Make 4.3

$ cmake --version
cmake version 3.25.1
```

- For testing correctness and compatibility of this Dilithium implementation, you need to globally install `google-test` library and headers. Follow guide @ https://github.com/google/googletest/tree/main/googletest#standalone-cmake-project, if you don't have it installed.
- For benchmarking Dilithium algorithms, you must have `google-benchmark` header and library globally installed. I found guide @ https://github.com/google/benchmark#installation helpful.

> [!NOTE]
> If you are on a machine running GNU/Linux kernel and you want to obtain CPU cycle count for KEM routines, you should consider building `google-benchmark` library with `libPFM` support, following https://gist.github.com/itzmeanjan/05dc3e946f635d00c5e0b21aae6203a7, a step-by-step guide. Find more about `libPFM` @ https://perfmon2.sourceforge.net.

> [!TIP]
> Git submodule based dependencies will generally be imported automatically, but in case that doesn't work, you can manually initialize and update them by issuing `$ git submodule update --init` from inside the root of this repository.

## Testing

For ensuring functional correctness ( & conformance with the Dilithium specification, assuming the reference implemenation @ https://github.com/pq-crystals/dilithium.git is correct ) of this Dilithium library implemenation, issue

> [!NOTE] 
> Dilithium Known Answer Tests are generated following the procedure, described in https://gist.github.com/itzmeanjan/d14afc3866b82119221682f0f3c9822d.

```bash
make -j            # Run tests without any sort of sanitizers
make asan_test -j  # Run tests with AddressSanitizer enabled
make ubsan_test -j # Run tests with UndefinedBehaviourSanitizer enabled
```

```bash
[13/13] Dilithium.ArithmeticOverZq (365 ms)
PASSED TESTS (13/13):
       1 ms: build/test.out Dilithium.HintBitPolynomialEncodingDecoding
       1 ms: build/test.out Dilithium.PolynomialEncodingDecoding
       1 ms: build/test.out Dilithium.HashingToABall
       1 ms: build/test.out Dilithium.NumberTheoreticTransform
       6 ms: build/test.out Dilithium.Power2Round
      16 ms: build/test.out Dilithium.MakingAndUsingOfHintBits
      54 ms: build/test.out Dilithium.Dilithium3KeygenSignVerifyFlow
      54 ms: build/test.out Dilithium.Dilithium2KeygenSignVerifyFlow
      92 ms: build/test.out Dilithium.Dilithium3KnownAnswerTests
      93 ms: build/test.out Dilithium.Dilithium5KeygenSignVerifyFlow
     112 ms: build/test.out Dilithium.Dilithium2KnownAnswerTests
     149 ms: build/test.out Dilithium.Dilithium5KnownAnswerTests
     365 ms: build/test.out Dilithium.ArithmeticOverZq
```

You can run timing leakage tests, using `dudect`, execute following

> [!NOTE]
> `dudect` is integrated into this library implementation of Dilithium DSA to find any sort of timing leakages. It checks for constant-timeness of most of the vital internal functions. Though it doesn't check constant-timeness of functions which use uniform rejection sampling, such as expansion of public matrix `A` or sampling of the vectors `s1`, `s2` or hashing to a ball etc.

```bash
# Can only be built and run on x86_64 machine.
make dudect_test_build -j

# Before running the constant-time tests, it's a good idea to put all CPU cores on "performance" mode.
# You may find the guide @ https://github.com/google/benchmark/blob/main/docs/reducing_variance.md helpful.

timeout 10m taskset -c 0 ./build/dudect/test_dilithium2.out
timeout 10m taskset -c 0 ./build/dudect/test_dilithium3.out
timeout 10m taskset -c 0 ./build/dudect/test_dilithium5.out
```

> [!TIP]
> `dudect` documentation says if `t` statistic is `< 10`, we're *probably* good, yes **probably**. You may want to read `dudect` documentation @ https://github.com/oreparaz/dudect. Also you might find the original paper @ https://ia.cr/2016/1123 interesting.

```bash
...
meas:   48.38 M, max t:   +2.77, max tau: 3.99e-04, (5/tau)^2: 1.57e+08. For the moment, maybe constant time.
meas:   48.48 M, max t:   +2.73, max tau: 3.93e-04, (5/tau)^2: 1.62e+08. For the moment, maybe constant time.
meas:   48.57 M, max t:   +2.76, max tau: 3.96e-04, (5/tau)^2: 1.59e+08. For the moment, maybe constant time.
meas:   48.67 M, max t:   +2.78, max tau: 3.99e-04, (5/tau)^2: 1.57e+08. For the moment, maybe constant time.
meas:   48.76 M, max t:   +2.79, max tau: 3.99e-04, (5/tau)^2: 1.57e+08. For the moment, maybe constant time.
meas:   48.85 M, max t:   +2.78, max tau: 3.97e-04, (5/tau)^2: 1.58e+08. For the moment, maybe constant time.
meas:   48.95 M, max t:   +2.79, max tau: 3.98e-04, (5/tau)^2: 1.58e+08. For the moment, maybe constant time.
meas:   49.05 M, max t:   +2.77, max tau: 3.95e-04, (5/tau)^2: 1.60e+08. For the moment, maybe constant time.
meas:   49.14 M, max t:   +2.69, max tau: 3.84e-04, (5/tau)^2: 1.70e+08. For the moment, maybe constant time.
meas:   49.24 M, max t:   +2.75, max tau: 3.92e-04, (5/tau)^2: 1.62e+08. For the moment, maybe constant time.
meas:   49.33 M, max t:   +2.73, max tau: 3.89e-04, (5/tau)^2: 1.65e+08. For the moment, maybe constant time.
meas:   49.43 M, max t:   +2.76, max tau: 3.93e-04, (5/tau)^2: 1.62e+08. For the moment, maybe constant time.
meas:   49.52 M, max t:   +2.76, max tau: 3.92e-04, (5/tau)^2: 1.63e+08. For the moment, maybe constant time.
meas:   49.62 M, max t:   +2.79, max tau: 3.97e-04, (5/tau)^2: 1.59e+08. For the moment, maybe constant time.
meas:   49.71 M, max t:   +2.78, max tau: 3.94e-04, (5/tau)^2: 1.61e+08. For the moment, maybe constant time.
```

## Benchmarking

Benchmarking key generation, signing and verification algorithms for various instantiations of Dilithium digital signature scheme can be done, by issuing

```bash
make benchmark -j  # If you haven't built google-benchmark library with libPFM support.
make perf -j       # If you have built google-benchmark library with libPFM support.
```

> [!NOTE] 
> Benchmarking expects presence of `google-benchmark` headers and library in global namespace ( so that it can be found by the compiler ) i.e. header and library path must live on **$PATH**.

> [!CAUTION] 
> Ensure you've put all your CPU cores on performance mode before running benchmarks, follow guide @ https://github.com/google/benchmark/blob/main/docs/reducing_variance.md.

> [!NOTE]
> Only deterministic signing procedure is benchmarked here, while signing random message of length 32 -bytes. One can benchmark non-deterministic signing procedure by explicitly passing truth value to template parameter of `sign(...)` routine and they must supply a 64 -bytes uniform random sampled seed, when invoking `sign` routine.

> [!WARNING]
> Relying only on average timing measurement for understanding performance characteristics of Dilithium signing algorithm may not be a good idea, given that it's a post-quantum digital signature scheme of **"Fiat-Shamir with Aborts"** paradigm - broadly speaking, during signing procedure it may need to abort and restart again, multiple times, based on what message is being signed or what sort of random seed is being used for randomized signing. So it's a better idea to also compute other statistics such as minimum, maximum and *median* ( pretty useful ) when timing execution of signing procedure. In following benchmark results, you'll see such statistics demonstrating broader performance characteristics of Dilithium signing procedure for various parameter sets.

> [!NOTE]
> `make perf` - was issued when collecting following benchmarks. Notice, *cycles* column, denoting cost of executing Dilithium signature scheme routines in terms of CPU cycles. Follow [this](https://github.com/google/benchmark/blob/main/docs/perf_counters.md) for more details.

### On 12th Gen Intel(R) Core(TM) i7-1260P

Compiled with **gcc version 13.2.0 (Ubuntu 13.2.0-4ubuntu3)**.

```bash
$ uname -srm
Linux 6.5.0-14-generic x86_64
```

```bash
2023-12-24T13:05:17+04:00
Running ./build/perf.out
Run on (16 X 4129.81 MHz CPU s)
CPU Caches:
  L1 Data 48 KiB (x8)
  L1 Instruction 32 KiB (x8)
  L2 Unified 1280 KiB (x8)
  L3 Unified 18432 KiB (x1)
Load Average: 0.33, 0.29, 0.27
--------------------------------------------------------------------------------------------------
Benchmark                            Time             CPU   Iterations     CYCLES items_per_second
--------------------------------------------------------------------------------------------------
dilithium5_sign/32_mean            573 us          573 us           32   2.62244M       2.15221k/s
dilithium5_sign/32_median          570 us          570 us           32   2.56475M       1.77223k/s
dilithium5_sign/32_stddev          289 us          289 us           32   1.33162M        983.885/s
dilithium5_sign/32_cv            50.50 %         50.50 %            32     50.78%           45.72%
dilithium5_sign/32_min             266 us          266 us           32   1.22116M        584.246/s
dilithium5_sign/32_max            1712 us         1712 us           32   7.85984M       3.75566k/s
dilithium2_sign/32_mean            413 us          413 us           32   1.92469M       4.31431k/s
dilithium2_sign/32_median          239 us          239 us           32   1.10512M       4.18457k/s
dilithium2_sign/32_stddev          393 us          393 us           32   1.83463M       2.77288k/s
dilithium2_sign/32_cv            95.04 %         95.04 %            32     95.32%           64.27%
dilithium2_sign/32_min             114 us          114 us           32     531.5k        548.719/s
dilithium2_sign/32_max            1822 us         1822 us           32   8.53117M       8.78612k/s
dilithium3_keygen_mean             100 us          100 us           32    450.02k       10.0107k/s
dilithium3_keygen_median          98.4 us         98.4 us           32   450.223k        10.158k/s
dilithium3_keygen_stddev          3.72 us         3.72 us           32   1.44564k        370.119/s
dilithium3_keygen_cv              3.72 %          3.72 %            32      0.32%            3.70%
dilithium3_keygen_min             95.9 us         95.9 us           32   443.754k       9.43455k/s
dilithium3_keygen_max              106 us          106 us           32   452.294k       10.4222k/s
dilithium3_sign/32_mean            511 us          511 us           32   2.38786M        2.9922k/s
dilithium3_sign/32_median          346 us          346 us           32   1.60917M       2.89361k/s
dilithium3_sign/32_stddev          370 us          370 us           32   1.73198M       1.82606k/s
dilithium3_sign/32_cv            72.38 %         72.38 %            32     72.53%           61.03%
dilithium3_sign/32_min             171 us          171 us           32   798.767k        611.443/s
dilithium3_sign/32_max            1636 us         1635 us           32   7.65597M       5.86292k/s
dilithium3_verify/32_mean          104 us          104 us           32   472.594k       9.60074k/s
dilithium3_verify/32_median        102 us          102 us           32   472.428k       9.82254k/s
dilithium3_verify/32_stddev       3.54 us         3.55 us           32    870.197        322.901/s
dilithium3_verify/32_cv           3.40 %          3.40 %            32      0.18%            3.36%
dilithium3_verify/32_min           101 us          101 us           32   470.808k        9.0583k/s
dilithium3_verify/32_max           110 us          110 us           32   474.024k       9.92375k/s
dilithium5_keygen_mean             168 us          168 us           32   749.512k       5.96366k/s
dilithium5_keygen_median           167 us          167 us           32   748.591k       6.00453k/s
dilithium5_keygen_stddev          4.98 us         4.97 us           32   3.57104k        175.466/s
dilithium5_keygen_cv              2.96 %          2.96 %            32      0.48%            2.94%
dilithium5_keygen_min              162 us          162 us           32   743.167k       5.63682k/s
dilithium5_keygen_max              177 us          177 us           32   757.093k       6.18287k/s
dilithium2_verify/32_mean         64.0 us         64.0 us           32   292.642k        15.631k/s
dilithium2_verify/32_median       62.9 us         62.9 us           32   292.392k       15.9072k/s
dilithium2_verify/32_stddev       2.00 us         2.00 us           32    891.422        476.406/s
dilithium2_verify/32_cv           3.12 %          3.12 %            32      0.30%            3.05%
dilithium2_verify/32_min          62.4 us         62.4 us           32   291.515k       14.6913k/s
dilithium2_verify/32_max          68.1 us         68.1 us           32   294.848k       16.0265k/s
dilithium2_keygen_mean            59.0 us         59.0 us           32   269.516k       16.9619k/s
dilithium2_keygen_median          58.4 us         58.4 us           32   269.005k       17.1373k/s
dilithium2_keygen_stddev          1.96 us         1.96 us           32   2.73875k        546.274/s
dilithium2_keygen_cv              3.31 %          3.31 %            32      1.02%            3.22%
dilithium2_keygen_min             56.7 us         56.7 us           32   265.559k       15.6044k/s
dilithium2_keygen_max             64.1 us         64.1 us           32   275.942k       17.6457k/s
dilithium5_verify/32_mean          171 us          171 us           32    780.57k        5.8392k/s
dilithium5_verify/32_median        168 us          168 us           32   780.972k       5.95881k/s
dilithium5_verify/32_stddev       5.63 us         5.63 us           32   1.43262k        187.536/s
dilithium5_verify/32_cv           3.28 %          3.28 %            32      0.18%            3.21%
dilithium5_verify/32_min           167 us          167 us           32   778.381k       5.49484k/s
dilithium5_verify/32_max           182 us          182 us           32   782.668k       6.00273k/s
```

### On ARM Cortex-A72 i.e. Raspberry Pi 4B

Compiled with **gcc version 13.2.0 (Ubuntu 13.2.0-4ubuntu3)**.

```bash
$ uname -srm
Linux 6.5.0-1008-raspi aarch64
```

```bash
2023-12-24T15:13:38+05:30
Running ./build/perf.out
Run on (4 X 1800 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 48 KiB (x4)
  L2 Unified 1024 KiB (x1)
Load Average: 1.18, 1.24, 0.78
--------------------------------------------------------------------------------------------------
Benchmark                            Time             CPU   Iterations     CYCLES items_per_second
--------------------------------------------------------------------------------------------------
dilithium5_verify/32_mean          982 us          982 us           32   1.76352M       1.01874k/s
dilithium5_verify/32_median        982 us          982 us           32   1.76347M       1.01877k/s
dilithium5_verify/32_stddev      0.618 us        0.417 us           32      447.3       0.432759/s
dilithium5_verify/32_cv           0.06 %          0.04 %            32      0.03%            0.04%
dilithium5_verify/32_min           981 us          981 us           32   1.76263M       1.01761k/s
dilithium5_verify/32_max           984 us          983 us           32   1.76433M        1.0195k/s
dilithium3_verify/32_mean          627 us          627 us           32   1.12594M       1.59563k/s
dilithium3_verify/32_median        627 us          627 us           32   1.12584M       1.59581k/s
dilithium3_verify/32_stddev      0.369 us        0.267 us           32      385.4       0.678979/s
dilithium3_verify/32_cv           0.06 %          0.04 %            32      0.03%            0.04%
dilithium3_verify/32_min           626 us          626 us           32   1.12543M       1.59386k/s
dilithium3_verify/32_max           628 us          627 us           32   1.12704M       1.59664k/s
dilithium2_verify/32_mean          397 us          397 us           32   712.872k       2.51939k/s
dilithium2_verify/32_median        397 us          397 us           32   712.921k       2.51999k/s
dilithium2_verify/32_stddev       1.01 us        0.901 us           32    514.135        5.66412/s
dilithium2_verify/32_cv           0.25 %          0.23 %            32      0.07%            0.22%
dilithium2_verify/32_min           396 us          396 us           32   711.708k       2.49017k/s
dilithium2_verify/32_max           402 us          402 us           32   713.909k       2.52477k/s
dilithium2_keygen_mean             332 us          332 us           32   596.912k       3.00994k/s
dilithium2_keygen_median           333 us          333 us           32   597.624k       3.00643k/s
dilithium2_keygen_stddev          1.30 us         1.31 us           32   2.40391k        11.8347/s
dilithium2_keygen_cv              0.39 %          0.39 %            32      0.40%            0.39%
dilithium2_keygen_min              329 us          329 us           32   591.684k       2.98561k/s
dilithium2_keygen_max              335 us          335 us           32   601.775k       3.03661k/s
dilithium3_sign/32_mean           4059 us         4058 us           32   7.29006M        365.449/s
dilithium3_sign/32_median         3304 us         3303 us           32   5.93442M        302.715/s
dilithium3_sign/32_stddev         2724 us         2723 us           32   4.89362M        233.459/s
dilithium3_sign/32_cv            67.10 %         67.11 %            32     67.13%           63.88%
dilithium3_sign/32_min            1171 us         1171 us           32   2.10391M        71.6582/s
dilithium3_sign/32_max           13957 us        13955 us           32   25.0762M        854.011/s
dilithium5_sign/32_mean           6044 us         6043 us           32   10.8539M        270.538/s
dilithium5_sign/32_median         4732 us         4732 us           32   8.49736M        211.344/s
dilithium5_sign/32_stddev         4933 us         4932 us           32   8.86216M        184.414/s
dilithium5_sign/32_cv            81.63 %         81.63 %            32     81.65%           68.17%
dilithium5_sign/32_min            1722 us         1722 us           32   3.09382M        38.8392/s
dilithium5_sign/32_max           25754 us        25747 us           32   46.2578M        580.602/s
dilithium5_keygen_mean             869 us          869 us           32   1.56125M       1.15065k/s
dilithium5_keygen_median           869 us          869 us           32    1.5609M       1.15112k/s
dilithium5_keygen_stddev          2.55 us         2.30 us           32   3.61955k        3.03513/s
dilithium5_keygen_cv              0.29 %          0.26 %            32      0.23%            0.26%
dilithium5_keygen_min              865 us          865 us           32   1.55393M       1.14095k/s
dilithium5_keygen_max              879 us          876 us           32   1.57077M       1.15633k/s
dilithium3_keygen_mean             548 us          547 us           32   983.306k       1.82668k/s
dilithium3_keygen_median           547 us          547 us           32   983.369k       1.82715k/s
dilithium3_keygen_stddev         0.969 us        0.740 us           32    698.653        2.45958/s
dilithium3_keygen_cv              0.18 %          0.14 %            32      0.07%            0.13%
dilithium3_keygen_min              547 us          547 us           32   981.931k       1.81604k/s
dilithium3_keygen_max              552 us          551 us           32   984.944k       1.82955k/s
dilithium2_sign/32_mean           2253 us         2253 us           32   4.04616M         666.05/s
dilithium2_sign/32_median         1763 us         1762 us           32   3.16635M        567.381/s
dilithium2_sign/32_stddev         1738 us         1738 us           32   3.12125M        385.348/s
dilithium2_sign/32_cv            77.14 %         77.13 %            32     77.14%           57.86%
dilithium2_sign/32_min             782 us          782 us           32   1.40548M        109.753/s
dilithium2_sign/32_max            9114 us         9111 us           32   16.3662M       1.27835k/s
```

### On Apple M1 Max

Compiled with **Apple clang version 15.0.0 (clang-1500.1.0.2.5)**.

```bash
$ uname -srm
Darwin 23.2.0 arm64
```

```bash
2024-01-22T19:57:30+04:00
Running ./build/bench.out
Run on (10 X 24 MHz CPU s)
CPU Caches:
  L1 Data 64 KiB
  L1 Instruction 128 KiB
  L2 Unified 4096 KiB (x10)
Load Average: 1.61, 1.96, 2.34
---------------------------------------------------------------------------------------
Benchmark                            Time             CPU   Iterations items_per_second
---------------------------------------------------------------------------------------
dilithium5_verify/32_mean          182 us          182 us           32       5.48988k/s
dilithium5_verify/32_median        182 us          182 us           32       5.49471k/s
dilithium5_verify/32_stddev      0.790 us        0.792 us           32         23.356/s
dilithium5_verify/32_cv           0.43 %          0.43 %            32            0.43%
dilithium5_verify/32_min           182 us          182 us           32       5.36387k/s
dilithium5_verify/32_max           187 us          186 us           32        5.5007k/s
dilithium3_keygen_mean             103 us          102 us           32       9.75627k/s
dilithium3_keygen_median           103 us          103 us           32       9.75592k/s
dilithium3_keygen_stddev         0.113 us        0.113 us           32        10.7959/s
dilithium3_keygen_cv              0.11 %          0.11 %            32            0.11%
dilithium3_keygen_min              102 us          102 us           32       9.72649k/s
dilithium3_keygen_max              103 us          103 us           32       9.77192k/s
dilithium2_sign/32_mean            389 us          388 us           32       3.60304k/s
dilithium2_sign/32_median          308 us          308 us           32       3.28064k/s
dilithium2_sign/32_stddev          224 us          224 us           32       2.13469k/s
dilithium2_sign/32_cv            57.71 %         57.71 %            32           59.25%
dilithium2_sign/32_min             125 us          125 us           32       1.10009k/s
dilithium2_sign/32_max             910 us          909 us           32       7.99269k/s
dilithium3_verify/32_mean          113 us          112 us           32       8.89718k/s
dilithium3_verify/32_median        113 us          112 us           32        8.8974k/s
dilithium3_verify/32_stddev      0.062 us        0.044 us           32        3.48072/s
dilithium3_verify/32_cv           0.06 %          0.04 %            32            0.04%
dilithium3_verify/32_min           112 us          112 us           32       8.89127k/s
dilithium3_verify/32_max           113 us          112 us           32       8.90303k/s
dilithium2_verify/32_mean         69.5 us         69.4 us           32       14.4038k/s
dilithium2_verify/32_median       69.5 us         69.4 us           32       14.4049k/s
dilithium2_verify/32_stddev      0.055 us        0.026 us           32        5.49004/s
dilithium2_verify/32_cv           0.08 %          0.04 %            32            0.04%
dilithium2_verify/32_min          69.4 us         69.4 us           32       14.3902k/s
dilithium2_verify/32_max          69.7 us         69.5 us           32       14.4125k/s
dilithium2_keygen_mean            60.7 us         60.6 us           32       16.4974k/s
dilithium2_keygen_median          60.7 us         60.6 us           32       16.5101k/s
dilithium2_keygen_stddev         0.489 us        0.482 us           32        130.751/s
dilithium2_keygen_cv              0.81 %          0.80 %            32            0.79%
dilithium2_keygen_min             59.8 us         59.8 us           32       16.1638k/s
dilithium2_keygen_max             61.9 us         61.9 us           32       16.7339k/s
dilithium5_sign/32_mean            786 us          785 us           32       1.79488k/s
dilithium5_sign/32_median          574 us          573 us           32       1.74461k/s
dilithium5_sign/32_stddev          560 us          559 us           32        943.588/s
dilithium5_sign/32_cv            71.24 %         71.24 %            32           52.57%
dilithium5_sign/32_min             287 us          287 us           32        368.016/s
dilithium5_sign/32_max            2721 us         2717 us           32       3.48385k/s
dilithium3_sign/32_mean            498 us          497 us           32       3.06659k/s
dilithium3_sign/32_median          393 us          392 us           32        2.5494k/s
dilithium3_sign/32_stddev          366 us          365 us           32       1.76289k/s
dilithium3_sign/32_cv            73.44 %         73.44 %            32           57.49%
dilithium3_sign/32_min             190 us          189 us           32         621.66/s
dilithium3_sign/32_max            1611 us         1609 us           32       5.27815k/s
dilithium5_keygen_mean             168 us          167 us           32       5.97094k/s
dilithium5_keygen_median           168 us          168 us           32       5.96961k/s
dilithium5_keygen_stddev         0.495 us        0.469 us           32        16.7342/s
dilithium5_keygen_cv              0.30 %          0.28 %            32            0.28%
dilithium5_keygen_min              167 us          166 us           32       5.94183k/s
dilithium5_keygen_max              169 us          168 us           32       6.01249k/s
```

## Usage

Dilithium is a header-only C++ library which is fairly easy-to-use. Let's see how to get started using it.

- Clone Dilithium repository.

```bash
cd

# Multi-step cloning and importing of submodules
git clone https://github.com/itzmeanjan/dilithium.git && pushd dilithium && git submodule update --init && popd
# Or do single step cloning and importing of submodules
git clone https://github.com/itzmeanjan/dilithium.git --recurse-submodules
```

- Write a program which makes use of Dilithium{2,3,5} {keygen, signing, verification} API ( all of these routines and constants, representing how many bytes of memory to allocate for holding public/ secret key and signature, live under `dilithium{2,3,5}::` namespace ), while importing proper header file.
- Finally compile your program, while letting your compiler know where it can find Dilithium ( `./include` ) and Sha3 ( `./sha3/include` ) headers.

```bash
# Assuming `dilithium` was cloned just under $HOME

DILITHIUM_HEADERS=~/dilithium/include
SHA3_HEADERS=~/dilithium/sha3/include

g++ -std=c++20 -Wall -Wextra -pedantic -O3 -march=native -I $DILITHIUM_HEADERS -I $SHA3_HEADERS main.cpp
```

### Dilithium API Usage Flow

Let's walk through an example, where I show you how to use Dilithium key generation, signing ( both deterministic and randomized ) and verification API for Dilithium2 instantiation. It should be pretty similar using other Dilithium instances.

1) Let's begin by generating a Dilithium2 keypair, given 32 -bytes seed.

```cpp
// main.cpp

// In case interested in using Dilithium3 or Dilithium5 API, import "dilithium3.hpp" or "dilithium5.hpp" and use keygen/ sign/ verify functions living either under `dilithium3::` or `dilithium5::` namespace.
#include "dilithium2.hpp"
#include <array>
#include "prng.hpp"

int main() {
    std::array<uint8_t, 32> seed{};
    std::array<uint8_t, dilithium2::PubKeyLen> pubkey{};
    std::array<uint8_t, dilithium2::SecKeyLen> seckey{};

    // Sample seed bytes from PRNG
    //
    // Be careful, read API documentation in include/prng.hpp
    // before you consider using it in production.
    prng::prng_t prng;
    prng.read(seed);

    dilithium2::keygen(seed, pubkey, seckey);

    // ...

    return 0;
}
```

2) Given a Dilithium2 secret key and non-empty message M, sign it, computing signature.

```cpp
int main() {
  // Key Generation
  // ...

  std::array<uint8_t, 32> msg{};
  std::array<uint8_t, dilithium2::SigLen> sig{};

  // Sample a psuedo-random message, to be signed
  prng.read(msg);

  // Default behaviour is deterministic signing and you can safely pass
  // an empty std::span (i.e. `{}`) for last parameter i.e. random seed.
  dilithium2::sign(seckey, msg, sig, {});

  // ...

  return 0;
}
```

3) In case you're interested in randomized signing, you may explicitly opt in ( at compile-time ) by passing **TRUTH** value for the only template parameter present in `sign` function definition, while also supplying a 64 -bytes uniform sampled random seed, when invoking `sign` procedure.

```cpp
int main() {
  // Key Generation
  // ...

  // Deterministic Signing ( default )
  // ...

  // 64 -bytes random seed to be used for randomized
  // message signing. We'll sample random seed bytes using PRNG.
  std::array<uint8_t, 64> rnd_seed{};
  prng.read(rnd_seed);

  // You must pass a 64 -bytes random seed when you explicitly opt for randomized signing.
  dilithium2::sign<true>(seckey, msg, sig, rnd_seed);

  return 0;
}
```

4) Verify signature, given public key, message M and the signature itself. It returns boolean truth value in case of successful signature verification otherwise it returns false.

```cpp
int main() {
  // Key Generation
  // ...

  // Deterministic Signing ( default )
  // ...

  // Randomized Signing ( explicit )
  // ...

  const bool flg = dilithium2::verify(pubkey, msg, sig);
  assert(flg);

  return 0;
}
```

I suggest you look at example [program](./examples/dilithium2.cpp), which demonstrates how to use Dilithium2 API, similarly you can use Dilithium{3,5} API.

```bash
$ g++ -std=c++20 -Wall -Wextra -pedantic -O3 -march=native -I ./include -I ./sha3/include examples/dilithium2.cpp && ./a.out
Dilithium @ NIST security level 2
seed      : e9ae2e74d3bc6e60b4966eb0da2f6d03c0d864ef7b001947d294d64cb90fd8c7
pubkey    : 1587ba74cc89529d73cc656feed4787e7c3b81399f211eaa262fe7d2e6b4c1e36ab472cf5f3f86a34f24504168dd3d5fa5464b7baeef99cfcc0cd5327a58c7a14dd26b231c70587011e23536d6710bdc265d58dbc036f12c4d20c867b912929b2b4113319927d262b19faaa5a700464bc31fe5082c57d533075115f80adf74d6dbed2387391006c4a30d3fdb56bd536e6f28c0f0030f55cc5fa2fa8ad4ade48bc07b23e8be49bdd4b1af6965ce6621d9a4a0f6dc634161702b3436d18c52f45ef3df50bd56d8572e4c10db101ae8b035f25f4f94169306de474f5b63ef50765c55b8b1088ed524c95441a31a7e886166dc1bbe61ca5c43ec81e60087b749c1a1143439c7df852e49087cc9bf442ffa01e02e9c21ba734195561e764a0f8700313a6c7002728220e27a7cffbc123c77e41b8a34ebcc9b2c7137da55aedcc7e37f9ebb0d146c60ada1089a57ee3b8e9a2456daf8900b65f3c74fe0150331f0c6b13ec18a12e3033b74ca934dadcc8fc03d22277fec5baa6565340de50cd04656cf8482c13f696316ef5bdd049d81a8b9197ebb817f4da289efb839ddef0101e13b3c5a76411fa48a1899c6a3b3c9a01143f47dc1b215073298fae398edbda469bf9fa314886ed23984335647ff6dbde83844b26b997fbf65069b5e6b304b1934863ce9395f247501c7071d0519949adb6708b4bcfef36842073f435a22e83355a87662531e5fdf5ce985bc96b23f993046fb32819a14b3f9c8b68cf71b5d1841cdfc0197ed5c2751846504afc198652f41f611ab1e617606a8557f672e11bad50475fac511c83e2b8970b86e2e4e06d055f46adf218cfd8e7c711ba5a92acbd015cfb21ce41f751111a58115f78742c7f48ded07149e6d4e9f65e11a616443475a0d1f3a100ba3532d79b13393e3671dc97fc6e4befd4497b6d0c8c7be4a7f9d4a5eff849c6240c985c4fd6bd8b548ed9c1c382a4f60b046beec03a9f7f4e26056ccc90357e726b768bc7b6ce75f9ab447a07b60b97bc8a591736dcd023fb8473e8de0898f34393e5be897802f017d7be7fb4766f866520fe2a5032bdc630c453c7f00a48307465185b25e1061452399513f569bee79db2ad1dab0d6e513a5a9542a5b438cbc140e56ea7ce9f951d39b0a5de98df072197632683892b2741bac4d0059fd5dfad37a3e3e2a4e3726933c81a5b1dd9c7f1bb6104784457dda8888c30e95d3b6786038c3c9b970c6a24d4df85b966315855e4b00ef7a5a2e8e0ba64cfd76de2302077425a1f6deaebed670c3e7d144c74bc2068fbaa3cdb44a1ae98f912272c1d980dba976ca93206cce50a4128066411e7322c1a3e4ecdba2011a0f64284bb5551c840f9305233700cbd1cb4d387fdb6066c0cff6b8aece0af44a78f6c5054228664c29092081fb341761696a25ac8aceb360e7e6764230cf83bae14746f7f8b6569d88e01b604b3294286e922e2314dd180a82eae1d31af6c7eec71917acc18cb86c6f590db24d6653d39dc995b576088784a6f95bdb48dbd4edd6e1d8e936b27f7e9d8d5c59c0d54dc690c8c8e0cb2c06561b2a8b89fad0550a7c07a5c6165bb7dc3b72e7cc559bdf9259a9290d03dda1f57779b89bd46f801d24ebd6cb6d9924225d817b625afb42b405163494d005e5a5e49a87ba55482adb81dc0e07717d7d3278880233720aabadd9cef8638474f333a92dca085b22935add88ebc3f33a5db1f17db92f4b5c76cad1b51477dbd729bdbb86d40f762668969fdc92692451e816113bc44b6398d6535ffa81e4685457a9c8eb0058445e8dbc80e36fe62c7b4267d6a47ca466f50c1921bd3343e1f5d6e0dd86fb67e32a0bdee8
seckey    : 1587ba74cc89529d73cc656feed4787e7c3b81399f211eaa262fe7d2e6b4c1e36b78b48676280afa460ce03472dc2c242fe85773d0c9a4d37c0e7dbeeb74d71949e8713ee92b14d4e205c90d1038c51eb7211e29fc6d426ce6d61e8ccaa83a7999226043286d193681149384e0424019120864104548a420dc088ad93861d1006502b231c4244818377024381208b22800818c10801080388d6444009ba48d0bb4641012685a320c5bb441003164db144209430989022822a42123398613324ccc10695000012298691817054cb88054302913340401033150002850083092004613c96cdba22c54143049a8451c1885ccc20cd930250b046c001121c908640b82018ca45060365213c75111b64c09b389501068022712e31830833848c216651b164d50b05044b401a2484114370ecb96890b950c03240e22c240d9022a84a23141248a64a24c5392502103919824420194691c350dd3340494c60140104091b084a31285000845d896700c1086d0468ada226dd2a448ca006449349040420c1c462903b140c8b08d63920ca21885cbc00d24b370e0805150004e12096401a16d82a88c0b80680ab36124992c1b276a081329d016109a329243462a042649e18841d30242d21042d1924419456914903021a565401862a0a881093551e2168e04b5045b04691a3312639030a1241011230c5416268a304102c8809bb6100bc564d3022188406c0448880a966c1932411419415c180e1ac5311194900012456018406322200aa16d1a8945942822c0444e13c8610b39258904605a183080a28921a5055a80050bc38051026811b24014460404310ea286218a8271622828cbc8680b32864bb251a0a64d10b491e400505b48528c943161a888838631132122d2100604c7259a408d8496085a4672da8824cc8261a286888b8228219088d9802c130766920089caa45092a601031080c1263159c680029488d4020808992840a68541800989082c9496288a22405a348258a44c1b474e990010c9260621194c94b0485098901226084a400ac0c87041004112445183228484024ac828084a366e64262688888d184320d990709aa4855100098c30121bb500000972e1461193444813821053128c5048492219690132108204255aa08821a4008ca84980305188321163b4492214129a9211a2184e93203280880081a4818a365114a14d94b00de81112a4b485babdc56b38e4c69a0abd9947d519682dfd173e67f3ab0a6d16dd0ac247dbc615e7bee38a02e30319f9e86d9e2d76db66c272c9ef24b9518e99a3644f47bd1bcf87affb98c9ad37c036bbab9b13cb9b56de08aa012826e59cb5db47b308df15c6b403138cf2bbfdc6a7e5e70e6b3b760107c1028b5618ffca98b08801b3fb30012ffd3fbd08fc965e1c861c41877ca19c1b132153c0aab53751468051ae961b1837cdecf9385af8e327d792552ecd5f935a39327e6203fa63029b7e3764001ce0fdd7930001f9ef0e9440754f7a6c2232666cb2fb85ed24db226e04286658e3403c9c414c8eb31663c75b047161a38dde38f6ede6326ddbf75172baba23a5dcb6f669445283eec5633dd37db584ec83a5cd2c7c12804a33325fa7e97d48de11300812a5e0a6d4eb6eb39e0bc69ad7228c09d9c56ead0848f66415c4242f85b4583a6290fc7b5763d6dc037066f76c7cb791a2b2a777ac0f5664be4b6abe397d15758dccd74cd1805344fd1a045b2dadf639157f0e06c5d9a0f62b4e64273d0529f2a9c97724e7d5e00fe26d76f9ab25d93a0dd1647bbd6841263729f90f4500efb15f55a02ea7f94affd0d365626ad7c53e03534d73023f712c8f718627bcfd2856082e17ba9efd7257d7655044385084e6eaded5276157393fbf9d3cba865afa0317afd68b72e888bdb04118f478b59a9f8479948faa94356ee4d260a3a807ae6db3b2c684041174184e033c061cfbe487fdf16f340d38e8de54a835f9a9c633e08d59298e227a64015e89d835c852f50edffbc0de2bcec72777399382c33bb32d4ffafc36e6dc43d847850c82000af4bd1955f7671424ec5b1cd40719f248d80625ccff03400f78570bbb774961443acfe78eaef57601e360f5403fcfc3daa9d4d4b5f85f2b9031033ef0b63adb5e2b7bca52b25ca20ae19123fb2b18d41aecc7dbd74ee496b38399f37e29d79e2db9412538bb4ef8c60e8ab16ffe7a0b5fe6ab9cd1abe56dea2f0fd2eb12d9d1c55c83fb3039e6ad4bac5cd110efb012ff49bed117036ac2b984fc211e29fccdbc1617b502f81d7f607448bd559e85b32f4fa4003e78cedac8dd8dc405ccf1999d216af8b1778f0425578ffec54455c020f6a1ae8e761025f14a8da7bd84eedc0e77a09466efa36d9dcab57a08c99a1a98d2a69c39fea365c4bf53709d54d08079b4d59f204976e498ec5800483e66e5e7529124e5bffe00250c1e766809bbc4769369c52ef35c618cbe739e3a12a55c30610c07410ed4b83b4a1982aa8f7ccb73acc15dfc1f17329dd4d9fa35b4ff439ec68dd4f5800e514159c3a83a22d0dc39b33b250c29a7c3ced2cc84c088a2a09489d117ec0926ebb98cf467b8eb483fca4f4766d8e03cb20f30bae6b0a548813b8d54d41b969234d07250098daeae931c83669451ff12b7dd5815f122381f85ac7de89b6e66134255e5b82876795965cd4d9c0409de6242ed38b13e12eda6ae7fda7e521beb528eba34e694f982ac0eb2e06888046566797c58bfe89e11bbff3c393bb9fb3a8c6f5b926b67dd06c254c576ca146de55ed87b628539c5a34ab2f1b5541b13682b809b8f3d831ac50c80556db0ae58f7993e9dc46aa20c42217eb9266bc3c13c4475705ecb7c12a237525a3b8a7d22c2087bd80687d225a01702eeda76d7bc51f7603bde890487cac304a59d701501fd30b741da3bb7bb520198db9558c55dfdeafd51e34be726786819bf30f2a4884cc221bf8ca43662a2ae9e080cb969b6aac2c585cce01e92cc81e5696b2e42d4fa675f863e114561b5d771d557ddca929dfd7e179b39fe66d71232667dd2f16b6a9cfd7cdd7ed0b9bfea72084b9b5ec769dd0f280eb1a2bd5d05bce845923fc383a21eb96a19ab0821a531abd21de82846206825b728beba454e0008bbfa5c03e3b753f9e81a297ac1ddd35e061d395ea10a05aea273f408c0ed6818b80b8f3e3a7f88ce6f27444749179075deefa00117ec8625dfb86e78063c5d1b0afbbd7fbdf2bb12bc42017342739a80abcbdb710ecba0e6d9250b11b55a3d173e7a0f8d1f90c50ad084929c4c0e8b8e2f3a7ada5e5e8cfe93aa19f6bddd840b7df529414ce885a4547dbba56f1e3f43c5e8b24c6f6a52cbed8b3d78d45a235a7540ae29c0d197036c612d113059a28735052007f734e4833552362924223097465c6ae1b212eaa7f3565186aa36944ba55384f3ef15f714c30b4dc946d987ca541f033049683b795620c7ba8c379652d404f0c8dd1b4b71ac224134c8782ea351d74529bdc039ad6cc3461a1826c6bcf60c066611158dd208aa72c1266a32f6de4d77cea857c2da77cd47
message   : 3d507521c3fb52591006a02be88a103e71c8dbc480e569012151413f5e00a1d8
signature : 7d6ccc43cc460718299032ad1051c6af240db4ab4292ac061149b1cac88bdf633d424d6ec6a2a50fa5a15652eb7b726146b2c7783721cb38f6c7f02959fdbc3361c03df15a0b0f7db17a8a24c59ee926509c66ebd560a1a3564e6420f4cb3f5ac92e15b8097fde1cd146fded46c809f90e9495717cbda9173a7b51de8d88a84f1f8d756720bb18bfc604c20cb7940870e7ee1561c495e9524d38de5ef51a0756c9fb2b1e12814ce8aa707309d5baa7e055ef73f7cd90059472305d9dcc5de047e951f973da2daedaebb1dd51ee5c62a20df03a4ed1fa29e2ce000cbe6dff4e4a843a2f70f8fe1429b2e50f18efaf8227d0cd0ca5c4a8c70cf443bb9fe3bd7139219214bf8fc7b1ab8899aeb2d9b4658856e73244a34398e2f461c2ee4e600fe11ebe581144035faf1d0a0914e6af013f122aa84b38a42fddea693ff1f5dd5010ac07d6107cc2b97b563a0b967e159a78ee742e46eb2c5d2d3f4366ffcc015e6236c19a6fa3a0f355724e5382af1f4f227d8e33ab9852229b58d5a6a524c2e095f65be8dd6c020604b42c052acb28fe054dc1d00e6807e77324dd19ae9635be83216ef982e89bbf7dfaf68f6a34bd0664bc185375c131dcdc38852cdf1effbe04fb2a778536efed88d84702e1db0dc192a19e188b2495f267b4d9a9d32d8739dc0a064b45be9979691db9d20ee74550bd1ed05ece852e1ff17a582602f20b561329ec4687c998603a2551bc63f9e72b3a73ea084bd3ba0b4af383636ed2e0dd6a0c6b084f4bfe5e25d89c15b207f92ec58fed737930be7ed13e5106dbb8f4d4e9550b5ab85cb227a8c6db974974403aa6c0ddba9109770e772ae1a9497918273bbc0916ff248fb68a38d296668b6adc0d738cb60f1dabbad544911ffeda34693279fd2ee8a43a34ddde37034abc4613d877aa7ebe57c651b53c05ab175c74acdfecb0472ca037c3e85934f2adcf8db25ee5accb4e1e8446746b263b9f6d1193dd7fb396bc08a40a031b8a3d78c4b39765c7758fdedfc5c02354ab5ac3892c38a1764329103c881f7d6a9b762491150a3a4ead940ec5cb5959779cbb2ca3326d6030086f8fce9ea80fe755d886eb74b54236faf8b6e3ff82978ee78e255eda2aa8e1d86a7941594ee654fde554292c3ac775f5437cdb52ea28faa0894d7d7edd83007a56e76e8addc8c613900f200aece90a9627b43eba7c53f806cfd89bd149ff67c395c5411ac5581604c204104f96f1c92a3425294c8b0b9918b511d3d152e7238a641aa3d7afafd43ff38213f44088355240f45bb9b35701034f5a0f295154d0218bc0cac3b17494620e7e53a6b1df05de58b55d5c009f9cde570d0c32b77227ae0c73d2b98e4ccc9a32fd0925e771759636fc3789e2942169a78ecc084ac4b71c684d95b229c262b33adbca454b93f4c1ac4d5bae33755cf084a85fd0dbb7d617b10d6591ff7448f6821361f6fe9f555584b0c02dc3d6649f2b4b7099b4b5d09e48d075437bdd92438a7093550063321b5d9c035e4ec2f5ef562fddd38cdb5d3b35b7666f20fe08d34db622aeb8aada1213f82a55de97129ee5d4895abf3863a14d278e6addac5ca76a99bf5c704de36ee083f460bafacc4f4b1ad5fd9b60e6158a1a773a76525070304e8ab3c10db566b0b6b72f8e882efd586ea536492f603300060f8abfe783e0d1790114293e01fe3c594d4a62aff5f22d5dcf586c4231afd4ef1566f2069962ff0d27fdf2f0494edcd13c928708aafa70aa24acd8addb1176b4bdc0461552b5d46fa3ddcc5de91cd6f55f424310665243ee5f65d2e78053bd70154de284d7b2f4724594109be6e091653b0131b161eab73cdfb7bbb8418f819bb3a49a8067c1b8fd30b5d61aa5309187328b2dc62d61f12062361d5f72234dbee11da5dbf57f32b068ef13c8da10b2f38714fc773fb753a6ac17e6fed27257079d903185f490b1236d5c834e2360ba6fd28df61e76525599739d19dd0c89229348c45ec5e5ca503313b776cabd0e19b6298828cdfd230c9dd233f0072cf07ce2ce45444f243bf610cac8871460570b8f9343ef5a33b537fae0756d32fac4e1478b4bcdd0f5a393981b89aa9fed8b84068fe836f1953244718b83a1964677062034ab4c2b9d79ce4a287eeb49d866ee335fa57952f08f20d34f4c10c0346e1d215851fe9d6083ca0a1528dc5efbe3ed6bc0ddac07f874239106bce60a1909cb367a1cb20408f7305af88d2f2dda5045682a6ded06287721ec958a1338f6181d763ee6e889e02200b6a15d1da1e476121bde926e6b5432422df1baee39a9230bb6588a9d6648566faf1f893b5447a36ce394a417d251feb4c2b89f4237001733fa3bac81ff8ad06c0bd43a01e0b9765af485f96e2bda4fa827d81b2761336ab93a804a159c07def22ca9d23cf19937786e9796047c6c9a54aef867ab3165aaccbe1ca75755465856a80185dc8a00eb0f3da916e9120cc1f086a84ef454f9003bf9a1480b03c4f4a231a2b5fdd7686272a24811e6baf7b3b2c8288aa3b16e03a92ae537bb3f5b06ae82ad578af6d9e09a4380212bda1609d5c7426211b9570253a91a49a3aff2a99039e948842ec59b2bb15169e7694793a05f1895f3a1b0fe328529a4e12574f48ed021fc23bea860bff95f96092b39d189817143c758126c71ebc042cfa38b04cffc84d38ffb420d95b03a1402ce0617cdd2e24fcbc0413bd239016443817ff8967f66dc3277f9b20c32bb5101160e6c5900b6a8f3804895168fc542ea38f31aa703aec0ec6993fa5a6c721b3f778a73e397742ee334d4839fe46b8efe05701b5ff6767a8af5d5716ad0f344a7536021b15d3eee9de5590789f2207a30879027effdd84f939cf14c9f6d2a0639aaf9bce25a85ca44e60292a44713742d10eaddc46f16d95435fa42d1d223dfbba7626e43bb58de83945d068e8a4d69fc3898ad6dd853a18d63796486f2c7fafc2b3a2968c746e5fe91ccc2e2e6ebee91695d3611bdb2224b272b49d03344c930b3398cb7100f1d41ffd7a277417a248bd20b55274822da1dc311d0e12ce6f849e4fafa85889fed2325b93651e1f0ece50a65da798e654fdd82a297b50e0bb24143bd73c68caa0612b64a4ae98e98410a0ed9e3e23c786db48aa3a65cb7053e3ec80cc1b3b05003e588596bc53e12d9a1434b686cd5b36dee4e5785cce41a52784805384b7f8714cc911c48748bac790008a4b2c13865886e1b9b537f317bd1c638e8901260295516b051b327e10799817cd390c1dd5f55c2d030bc06f37a55ae2ff860b14552d43d9a05090d2c46484d5e6b7173838b8fdaf2f4fe02030f171a222f415a737b848590a1a6abd7dddfeaf3042c2d666c757d9497b2bbc1c2c6d1e31215162d38444e70b4bbcad9eaf9fd00000000000000000012283847
verified   : true
```



> [!CAUTION]
> Before you consider using Psuedo Random Number Generator which comes with this library implementation, I strongly advice you to go through [include/prng.hpp](./include/prng.hpp).

> [!NOTE]
> Looking at the API documentation, in header files, can give you a good idea of how to use Dilithium API. Note, this library doesn't expose any raw pointer based interface, rather everything is wrapped under statically defined `std::span` - which one can easily create from `std::{array, vector}`. I opt for using statically defined `std::span` based function interfaces because we always know, at compile-time, how many bytes the seeds/ keys/ signatures are, for various different Dilithium instantiations. *This gives much better type safety and compile-time error reporting.*
