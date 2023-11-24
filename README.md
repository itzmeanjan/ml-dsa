> [!CAUTION]
> This Dilithium implementation is conformant with Dilithium [specification](https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf) and I also try to make it constant-time but be informed that it is not yet audited. **If you consider using it in production, be careful !**

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

Here I'm maintaining Dilithium as a zero-dependency, header-only & easy-to-use C++ library, offering key generation, signing & verification API for three NIST security level ( i.e. 2, 3, 5 ) parameters, as defined in table 2 of Dilithium specification. For more details see [below](#usage).

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
make -j
```

```bash
Note: Randomizing tests' orders with a seed of 61489 .
[==========] Running 13 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 13 tests from Dilithium
[ RUN      ] Dilithium.ArithmeticOverZq
[       OK ] Dilithium.ArithmeticOverZq (327 ms)
[ RUN      ] Dilithium.HintBitPolynomialEncodingDecoding
[       OK ] Dilithium.HintBitPolynomialEncodingDecoding (0 ms)
[ RUN      ] Dilithium.Power2Round
[       OK ] Dilithium.Power2Round (1 ms)
[ RUN      ] Dilithium.Dilithium5KeygenSignVerifyFlow
[       OK ] Dilithium.Dilithium5KeygenSignVerifyFlow (49 ms)
[ RUN      ] Dilithium.HashingToABall
[       OK ] Dilithium.HashingToABall (0 ms)
[ RUN      ] Dilithium.MakingAndUsingOfHintBits
[       OK ] Dilithium.MakingAndUsingOfHintBits (7 ms)
[ RUN      ] Dilithium.Dilithium2KnownAnswerTests
[       OK ] Dilithium.Dilithium2KnownAnswerTests (49 ms)
[ RUN      ] Dilithium.Dilithium3KnownAnswerTests
[       OK ] Dilithium.Dilithium3KnownAnswerTests (72 ms)
[ RUN      ] Dilithium.Dilithium5KnownAnswerTests
[       OK ] Dilithium.Dilithium5KnownAnswerTests (111 ms)
[ RUN      ] Dilithium.Dilithium2KeygenSignVerifyFlow
[       OK ] Dilithium.Dilithium2KeygenSignVerifyFlow (20 ms)
[ RUN      ] Dilithium.PolynomialEncodingDecoding
[       OK ] Dilithium.PolynomialEncodingDecoding (0 ms)
[ RUN      ] Dilithium.NumberTheoreticTransform
[       OK ] Dilithium.NumberTheoreticTransform (0 ms)
[ RUN      ] Dilithium.Dilithium3KeygenSignVerifyFlow
[       OK ] Dilithium.Dilithium3KeygenSignVerifyFlow (34 ms)
[----------] 13 tests from Dilithium (674 ms total)

[----------] Global test environment tear-down
[==========] 13 tests from 1 test suite ran. (674 ms total)
[  PASSED  ] 13 tests.
```

## Benchmarking

Benchmarking key generation, signing and verification algorithms for various instantiations of Dilithium digital signature scheme can be done, by issuing

```bash
make benchmark  # If you haven't built google-benchmark library with libPFM support.
make perf       # If you have built google-benchmark library with libPFM support.
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

### On 12th Gen Intel(R) Core(TM) i7-1260P **[ Compiled with GCC-13.2.0 ]**

```bash
2023-11-24T14:00:30+05:30
Running ./build/perf.out
Run on (16 X 839.989 MHz CPU s)
CPU Caches:
  L1 Data 48 KiB (x8)
  L1 Instruction 32 KiB (x8)
  L2 Unified 1280 KiB (x8)
  L3 Unified 18432 KiB (x1)
Load Average: 0.28, 0.52, 0.46
--------------------------------------------------------------------------------------------------
Benchmark                            Time             CPU   Iterations     CYCLES items_per_second
--------------------------------------------------------------------------------------------------
dilithium3_verify/32_mean          105 us          105 us           32   492.479k       9.50972k/s
dilithium3_verify/32_median        105 us          105 us           32   492.457k       9.51049k/s
dilithium3_verify/32_stddev      0.154 us        0.153 us           32    390.133        13.8426/s
dilithium3_verify/32_cv           0.15 %          0.15 %            32      0.08%            0.15%
dilithium3_verify/32_min           105 us          105 us           32   491.685k       9.47499k/s
dilithium3_verify/32_max           106 us          106 us           32   493.318k        9.5289k/s
dilithium3_sign/32_mean            550 us          550 us           32   2.57555M        2.8136k/s
dilithium3_sign/32_median          383 us          383 us           32   1.78843M        2.6104k/s
dilithium3_sign/32_stddev          379 us          379 us           32   1.77113M       1.72678k/s
dilithium3_sign/32_cv            68.80 %         68.80 %            32     68.77%           61.37%
dilithium3_sign/32_min             185 us          185 us           32   869.013k        734.125/s
dilithium3_sign/32_max            1362 us         1362 us           32   6.37317M       5.39513k/s
dilithium5_sign/32_mean            833 us          833 us           32   3.90133M       1.80499k/s
dilithium5_sign/32_median          689 us          689 us           32   3.22353M       1.45107k/s
dilithium5_sign/32_stddev          561 us          561 us           32   2.62878M       1.11638k/s
dilithium5_sign/32_cv            67.38 %         67.38 %            32     67.38%           61.85%
dilithium5_sign/32_min             279 us          279 us           32   1.30622M        385.125/s
dilithium5_sign/32_max            2597 us         2597 us           32   12.1587M       3.58552k/s
dilithium5_keygen_mean             158 us          158 us           32   731.709k       6.31797k/s
dilithium5_keygen_median           157 us          157 us           32   731.726k       6.38765k/s
dilithium5_keygen_stddev          3.26 us         3.24 us           32   2.37345k        126.241/s
dilithium5_keygen_cv              2.06 %          2.05 %            32      0.32%            2.00%
dilithium5_keygen_min              156 us          156 us           32   725.972k       6.03342k/s
dilithium5_keygen_max              166 us          166 us           32   736.149k       6.41971k/s
dilithium3_keygen_mean            97.5 us         97.5 us           32    453.17k        10.262k/s
dilithium3_keygen_median          96.9 us         96.9 us           32   453.301k       10.3187k/s
dilithium3_keygen_stddev          1.33 us         1.33 us           32    673.339        134.571/s
dilithium3_keygen_cv              1.36 %          1.36 %            32      0.15%            1.31%
dilithium3_keygen_min             96.6 us         96.6 us           32   451.315k       9.69239k/s
dilithium3_keygen_max              103 us          103 us           32   454.457k       10.3549k/s
dilithium5_verify/32_mean          169 us          169 us           32   788.103k       5.92115k/s
dilithium5_verify/32_median        169 us          169 us           32   788.195k       5.93295k/s
dilithium5_verify/32_stddev       1.55 us         1.55 us           32   1.12534k        52.2231/s
dilithium5_verify/32_cv           0.92 %          0.92 %            32      0.14%            0.88%
dilithium5_verify/32_min           168 us          168 us           32   784.869k       5.65495k/s
dilithium5_verify/32_max           177 us          177 us           32    790.32k       5.95345k/s
dilithium2_keygen_mean            59.0 us         59.0 us           32   273.133k       16.9577k/s
dilithium2_keygen_median          58.8 us         58.8 us           32   273.182k       17.0014k/s
dilithium2_keygen_stddev         0.971 us        0.964 us           32   1.89083k        272.668/s
dilithium2_keygen_cv              1.65 %          1.63 %            32      0.69%            1.61%
dilithium2_keygen_min             57.6 us         57.6 us           32   269.561k        16.176k/s
dilithium2_keygen_max             61.8 us         61.8 us           32   276.459k       17.3708k/s
dilithium2_sign/32_mean            355 us          355 us           32   1.66041M       3.97351k/s
dilithium2_sign/32_median          263 us          263 us           32   1.23333M       3.79739k/s
dilithium2_sign/32_stddev          236 us          236 us           32   1.10475M       2.10547k/s
dilithium2_sign/32_cv            66.51 %         66.51 %            32     66.53%           52.99%
dilithium2_sign/32_min             124 us          124 us           32   581.286k       1.12438k/s
dilithium2_sign/32_max             889 us          889 us           32   4.16355M       8.06122k/s
dilithium2_verify/32_mean         66.7 us         66.7 us           32   310.794k       14.9895k/s
dilithium2_verify/32_median       66.5 us         66.5 us           32    310.74k       15.0431k/s
dilithium2_verify/32_stddev      0.843 us        0.833 us           32    746.344        181.516/s
dilithium2_verify/32_cv           1.26 %          1.25 %            32      0.24%            1.21%
dilithium2_verify/32_min          66.2 us         66.2 us           32   309.596k       14.4198k/s
dilithium2_verify/32_max          69.5 us         69.3 us           32    312.63k        15.103k/s
```

### On 12th Gen Intel(R) Core(TM) i7-1260P **[ Compiled with Clang-17.0.2 ]**

```bash
2023-11-24T14:04:13+05:30
Running ./build/perf.out
Run on (16 X 3936.15 MHz CPU s)
CPU Caches:
  L1 Data 48 KiB (x8)
  L1 Instruction 32 KiB (x8)
  L2 Unified 1280 KiB (x8)
  L3 Unified 18432 KiB (x1)
Load Average: 0.80, 0.61, 0.51
--------------------------------------------------------------------------------------------------
Benchmark                            Time             CPU   Iterations     CYCLES items_per_second
--------------------------------------------------------------------------------------------------
dilithium5_keygen_mean             133 us          133 us           32   596.752k       7.51006k/s
dilithium5_keygen_median           132 us          132 us           32   595.349k       7.59197k/s
dilithium5_keygen_stddev          3.96 us         3.96 us           32   5.00362k        217.639/s
dilithium5_keygen_cv              2.97 %          2.97 %            32      0.84%            2.90%
dilithium5_keygen_min              129 us          129 us           32   588.644k       7.09706k/s
dilithium5_keygen_max              141 us          141 us           32   609.603k       7.73063k/s
dilithium5_sign/32_mean            517 us          517 us           32   2.34129M       2.63652k/s
dilithium5_sign/32_median          421 us          421 us           32   1.92159M       2.37601k/s
dilithium5_sign/32_stddev          325 us          325 us           32   1.47486M       1.32433k/s
dilithium5_sign/32_cv            62.84 %         62.84 %            32     62.99%           50.23%
dilithium5_sign/32_min             219 us          219 us           32  0.999609M        700.328/s
dilithium5_sign/32_max            1428 us         1428 us           32   6.53057M       4.55983k/s
dilithium3_verify/32_mean         84.9 us         84.9 us           32   386.239k       11.7898k/s
dilithium3_verify/32_median       83.4 us         83.4 us           32   385.641k       11.9876k/s
dilithium3_verify/32_stddev       2.91 us         2.91 us           32   3.57067k        393.249/s
dilithium3_verify/32_cv           3.43 %          3.43 %            32      0.92%            3.34%
dilithium3_verify/32_min          81.9 us         81.9 us           32   381.238k       11.0994k/s
dilithium3_verify/32_max          90.1 us         90.1 us           32   393.569k        12.209k/s
dilithium3_keygen_mean            82.3 us         82.3 us           32   367.222k       12.1657k/s
dilithium3_keygen_median          80.2 us         80.2 us           32   367.026k       12.4677k/s
dilithium3_keygen_stddev          3.05 us         3.05 us           32   1.93705k        442.201/s
dilithium3_keygen_cv              3.70 %          3.70 %            32      0.53%            3.63%
dilithium3_keygen_min             79.5 us         79.5 us           32   364.153k       11.5223k/s
dilithium3_keygen_max             86.8 us         86.8 us           32   371.472k       12.5746k/s
dilithium2_sign/32_mean            301 us          301 us           32   1.38409M       5.15186k/s
dilithium2_sign/32_median          210 us          210 us           32   917.325k       4.75364k/s
dilithium2_sign/32_stddev          216 us          216 us           32   992.083k       3.24304k/s
dilithium2_sign/32_cv            71.76 %         71.76 %            32     71.68%           62.95%
dilithium2_sign/32_min            91.9 us         91.9 us           32   427.908k       1.09186k/s
dilithium2_sign/32_max             916 us          916 us           32   4.28524M       10.8864k/s
dilithium5_verify/32_mean          138 us          138 us           32   622.474k       7.23725k/s
dilithium5_verify/32_median        137 us          137 us           32   619.847k       7.29201k/s
dilithium5_verify/32_stddev       3.51 us         3.52 us           32   5.83644k        178.789/s
dilithium5_verify/32_cv           2.54 %          2.54 %            32      0.94%            2.47%
dilithium5_verify/32_min           135 us          135 us           32   615.854k       6.88218k/s
dilithium5_verify/32_max           145 us          145 us           32   634.293k       7.42256k/s
dilithium2_keygen_mean            47.7 us         47.7 us           32   215.429k       20.9872k/s
dilithium2_keygen_median          46.5 us         46.5 us           32   215.182k       21.5171k/s
dilithium2_keygen_stddev          2.01 us         2.01 us           32   1.60075k        865.492/s
dilithium2_keygen_cv              4.21 %          4.20 %            32      0.74%            4.12%
dilithium2_keygen_min             45.5 us         45.5 us           32   212.904k       19.5778k/s
dilithium2_keygen_max             51.1 us         51.1 us           32   218.501k       21.9639k/s
dilithium2_verify/32_mean         52.3 us         52.3 us           32   237.486k         19.14k/s
dilithium2_verify/32_median       51.0 us         51.0 us           32   237.476k       19.6144k/s
dilithium2_verify/32_stddev       2.10 us         2.09 us           32   1.09609k        742.983/s
dilithium2_verify/32_cv           4.01 %          3.99 %            32      0.46%            3.88%
dilithium2_verify/32_min          50.5 us         50.5 us           32   236.034k       18.0146k/s
dilithium2_verify/32_max          55.6 us         55.5 us           32   239.579k       19.8102k/s
dilithium3_sign/32_mean            431 us          431 us           32   1.94524M        3.3234k/s
dilithium3_sign/32_median          371 us          371 us           32   1.65281M       2.69524k/s
dilithium3_sign/32_stddev          292 us          292 us           32   1.33275M       1.98216k/s
dilithium3_sign/32_cv            67.67 %         67.67 %            32     68.51%           59.64%
dilithium3_sign/32_min             144 us          144 us           32   655.349k        655.243/s
dilithium3_sign/32_max            1526 us         1526 us           32   6.98492M       6.96257k/s
```

## Usage

Dilithium is a zero-dependency, header-only C++ library which is fairly easy-to-use. Let's see how to get started using it.

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

g++ -std=c++20 -Wall -pendantic -O3 -march=native -I $DILITHIUM_HEADERS -I $SHA3_HEADERS main.cpp
```

### Dilithium API Usage Flow

Let's walk through an example, where I show you how to use Dilithium key generation, signing ( both deterministic and randomized ) and verification API for Dilithium2 instantiation. It should be pretty similar using other Dilithium instances.

1) Let's begin by generating a Dilithium2 keypair, given 32 -bytes seed.

```cpp
// main.cpp

// In case interested in using Dilithium3 or Dilithium5 API, import "dilithium3.hpp" or "dilithium5.hpp" and use keygen/ sign/ verify functions living either under `dilithium3::` or `dilithium5::` namespace.
#include "dilithium2.hpp"
#include "prng.hpp"

int main() {
    uint8_t seed[32];
    uint8_t pubkey[dilithium2::PubKeyLen];
    uint8_t seckey[dilithium2::SecKeyLen];

    // Sample seed bytes from PRNG
    //
    // Be careful, read API documentation in include/prng.hpp
    // before you consider using it in production.
    prng::prng_t prng;
    prng.read(seed, sizeof(seed));

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

  uint8_t msg[32];
  uint8_t sig[dilithium2::SigLen];

  // Sample psuedo-random message, to be signed
  prng.read(msg, sizeof(msg));

  // Default behaviour is deterministic signing and
  // you can safely pass null pointer for last parameter
  // i.e. random seed. It won't be access, in case you adopt
  // default deterministic signing.
  dilithium2::sign(seckey, msg, mlen, sig, nullptr);

  // ...

  return 0;
}
```

3) In case you're interested in randomized signing, you may explicitly opt in ( at compile-time ) by passing truth value for the only template parameter present in `sign` function definition, while also supplying a 64 -bytes uniform sampled random seed, when invoking `sign` procedure.

```cpp
int main() {
  // Key Generation
  // ...

  // Deterministic Signing ( default )
  // ...

  // 64 -bytes random seed to be used for randomized
  // message signing. We'll sample random seed bytes using PRNG.
  uint8_t rnd_seed[64];
  prng.read(rnd_seed, sizeof(rnd_seed));

  // You must pass 64 -bytes random seed when you opt for
  // randomized signing. In case you don't do that, it'll
  // result in undefined behaviour.
  dilithium2::sign<true>(seckey, msg, mlen, sig, rng_seed);

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

  bool flg = dilithium2::verify(pubkey, msg, mlen, sig);
  assert(flg);

  return 0;
}
```

I suggest you look at following three example programs, which demonstrates how to use Dilithium API.

- For Dilithium2 ( i.e. parameters providing NIST security level 2 ) see [dilithium2.cpp](./example/dilithium2.cpp)
- For Dilithium3 ( i.e. parameters providing NIST security level 3 ) see [dilithium3.cpp](./example/dilithium3.cpp)
- For Dilithium5 ( i.e. parameters providing NIST security level 5 ) see [dilithium5.cpp](./example/dilithium5.cpp)

```bash
$ g++ -std=c++20 -O3 -I ./include -I ./sha3/include example/dilithium2.cpp && ./a.out
Dilithium @ NIST security level 2
seed      : e9ae2e74d3bc6e60b4966eb0da2f6d03c0d864ef7b001947d294d64cb90fd8c7
pubkey    : 1587ba74cc89529d73cc656feed4787e7c3b81399f211eaa262fe7d2e6b4c1e36ab472cf5f3f86a34f24504168dd3d5fa5464b7baeef99cfcc0cd5327a58c7a14dd26b231c70587011e23536d6710bdc265d58dbc036f12c4d20c867b912929b2b4113319927d262b19faaa5a700464bc31fe5082c57d533075115f80adf74d6dbed2387391006c4a30d3fdb56bd536e6f28c0f0030f55cc5fa2fa8ad4ade48bc07b23e8be49bdd4b1af6965ce6621d9a4a0f6dc634161702b3436d18c52f45ef3df50bd56d8572e4c10db101ae8b035f25f4f94169306de474f5b63ef50765c55b8b1088ed524c95441a31a7e886166dc1bbe61ca5c43ec81e60087b749c1a1143439c7df852e49087cc9bf442ffa01e02e9c21ba734195561e764a0f8700313a6c7002728220e27a7cffbc123c77e41b8a34ebcc9b2c7137da55aedcc7e37f9ebb0d146c60ada1089a57ee3b8e9a2456daf8900b65f3c74fe0150331f0c6b13ec18a12e3033b74ca934dadcc8fc03d22277fec5baa6565340de50cd04656cf8482c13f696316ef5bdd049d81a8b9197ebb817f4da289efb839ddef0101e13b3c5a76411fa48a1899c6a3b3c9a01143f47dc1b215073298fae398edbda469bf9fa314886ed23984335647ff6dbde83844b26b997fbf65069b5e6b304b1934863ce9395f247501c7071d0519949adb6708b4bcfef36842073f435a22e83355a87662531e5fdf5ce985bc96b23f993046fb32819a14b3f9c8b68cf71b5d1841cdfc0197ed5c2751846504afc198652f41f611ab1e617606a8557f672e11bad50475fac511c83e2b8970b86e2e4e06d055f46adf218cfd8e7c711ba5a92acbd015cfb21ce41f751111a58115f78742c7f48ded07149e6d4e9f65e11a616443475a0d1f3a100ba3532d79b13393e3671dc97fc6e4befd4497b6d0c8c7be4a7f9d4a5eff849c6240c985c4fd6bd8b548ed9c1c382a4f60b046beec03a9f7f4e26056ccc90357e726b768bc7b6ce75f9ab447a07b60b97bc8a591736dcd023fb8473e8de0898f34393e5be897802f017d7be7fb4766f866520fe2a5032bdc630c453c7f00a48307465185b25e1061452399513f569bee79db2ad1dab0d6e513a5a9542a5b438cbc140e56ea7ce9f951d39b0a5de98df072197632683892b2741bac4d0059fd5dfad37a3e3e2a4e3726933c81a5b1dd9c7f1bb6104784457dda8888c30e95d3b6786038c3c9b970c6a24d4df85b966315855e4b00ef7a5a2e8e0ba64cfd76de2302077425a1f6deaebed670c3e7d144c74bc2068fbaa3cdb44a1ae98f912272c1d980dba976ca93206cce50a4128066411e7322c1a3e4ecdba2011a0f64284bb5551c840f9305233700cbd1cb4d387fdb6066c0cff6b8aece0af44a78f6c5054228664c29092081fb341761696a25ac8aceb360e7e6764230cf83bae14746f7f8b6569d88e01b604b3294286e922e2314dd180a82eae1d31af6c7eec71917acc18cb86c6f590db24d6653d39dc995b576088784a6f95bdb48dbd4edd6e1d8e936b27f7e9d8d5c59c0d54dc690c8c8e0cb2c06561b2a8b89fad0550a7c07a5c6165bb7dc3b72e7cc559bdf9259a9290d03dda1f57779b89bd46f801d24ebd6cb6d9924225d817b625afb42b405163494d005e5a5e49a87ba55482adb81dc0e07717d7d3278880233720aabadd9cef8638474f333a92dca085b22935add88ebc3f33a5db1f17db92f4b5c76cad1b51477dbd729bdbb86d40f762668969fdc92692451e816113bc44b6398d6535ffa81e4685457a9c8eb0058445e8dbc80e36fe62c7b4267d6a47ca466f50c1921bd3343e1f5d6e0dd86fb67e32a0bdee8
seckey    : 1587ba74cc89529d73cc656feed4787e7c3b81399f211eaa262fe7d2e6b4c1e36b78b48676280afa460ce03472dc2c242fe85773d0c9a4d37c0e7dbeeb74d71949e8713ee92b14d4e205c90d1038c51eb7211e29fc6d426ce6d61e8ccaa83a7999226043286d193681149384e0424019120864104548a420dc088ad93861d1006502b231c4244818377024381208b22800818c10801080388d6444009ba48d0bb4641012685a320c5bb441003164db144209430989022822a42123398613324ccc10695000012298691817054cb88054302913340401033150002850083092004613c96cdba22c54143049a8451c1885ccc20cd930250b046c001121c908640b82018ca45060365213c75111b64c09b389501068022712e31830833848c216651b164d50b05044b401a2484114370ecb96890b950c03240e22c240d9022a84a23141248a64a24c5392502103919824420194691c350dd3340494c60140104091b084a31285000845d896700c1086d0468ada226dd2a448ca006449349040420c1c462903b140c8b08d63920ca21885cbc00d24b370e0805150004e12096401a16d82a88c0b80680ab36124992c1b276a081329d016109a329243462a042649e18841d30242d21042d1924419456914903021a565401862a0a881093551e2168e04b5045b04691a3312639030a1241011230c5416268a304102c8809bb6100bc564d3022188406c0448880a966c1932411419415c180e1ac5311194900012456018406322200aa16d1a8945942822c0444e13c8610b39258904605a183080a28921a5055a80050bc38051026811b24014460404310ea286218a8271622828cbc8680b32864bb251a0a64d10b491e400505b48528c943161a888838631132122d2100604c7259a408d8496085a4672da8824cc8261a286888b8228219088d9802c130766920089caa45092a601031080c1263159c680029488d4020808992840a68541800989082c9496288a22405a348258a44c1b474e990010c9260621194c94b0485098901226084a400ac0c87041004112445183228484024ac828084a366e64262688888d184320d990709aa4855100098c30121bb500000972e1461193444813821053128c5048492219690132108204255aa08821a4008ca84980305188321163b4492214129a9211a2184e93203280880081a4818a365114a14d94b00de81112a4b485babdc56b38e4c69a0abd9947d519682dfd173e67f3ab0a6d16dd0ac247dbc615e7bee38a02e30319f9e86d9e2d76db66c272c9ef24b9518e99a3644f47bd1bcf87affb98c9ad37c036bbab9b13cb9b56de08aa012826e59cb5db47b308df15c6b403138cf2bbfdc6a7e5e70e6b3b760107c1028b5618ffca98b08801b3fb30012ffd3fbd08fc965e1c861c41877ca19c1b132153c0aab53751468051ae961b1837cdecf9385af8e327d792552ecd5f935a39327e6203fa63029b7e3764001ce0fdd7930001f9ef0e9440754f7a6c2232666cb2fb85ed24db226e04286658e3403c9c414c8eb31663c75b047161a38dde38f6ede6326ddbf75172baba23a5dcb6f669445283eec5633dd37db584ec83a5cd2c7c12804a33325fa7e97d48de11300812a5e0a6d4eb6eb39e0bc69ad7228c09d9c56ead0848f66415c4242f85b4583a6290fc7b5763d6dc037066f76c7cb791a2b2a777ac0f5664be4b6abe397d15758dccd74cd1805344fd1a045b2dadf639157f0e06c5d9a0f62b4e64273d0529f2a9c97724e7d5e00fe26d76f9ab25d93a0dd1647bbd6841263729f90f4500efb15f55a02ea7f94affd0d365626ad7c53e03534d73023f712c8f718627bcfd2856082e17ba9efd7257d7655044385084e6eaded5276157393fbf9d3cba865afa0317afd68b72e888bdb04118f478b59a9f8479948faa94356ee4d260a3a807ae6db3b2c684041174184e033c061cfbe487fdf16f340d38e8de54a835f9a9c633e08d59298e227a64015e89d835c852f50edffbc0de2bcec72777399382c33bb32d4ffafc36e6dc43d847850c82000af4bd1955f7671424ec5b1cd40719f248d80625ccff03400f78570bbb774961443acfe78eaef57601e360f5403fcfc3daa9d4d4b5f85f2b9031033ef0b63adb5e2b7bca52b25ca20ae19123fb2b18d41aecc7dbd74ee496b38399f37e29d79e2db9412538bb4ef8c60e8ab16ffe7a0b5fe6ab9cd1abe56dea2f0fd2eb12d9d1c55c83fb3039e6ad4bac5cd110efb012ff49bed117036ac2b984fc211e29fccdbc1617b502f81d7f607448bd559e85b32f4fa4003e78cedac8dd8dc405ccf1999d216af8b1778f0425578ffec54455c020f6a1ae8e761025f14a8da7bd84eedc0e77a09466efa36d9dcab57a08c99a1a98d2a69c39fea365c4bf53709d54d08079b4d59f204976e498ec5800483e66e5e7529124e5bffe00250c1e766809bbc4769369c52ef35c618cbe739e3a12a55c30610c07410ed4b83b4a1982aa8f7ccb73acc15dfc1f17329dd4d9fa35b4ff439ec68dd4f5800e514159c3a83a22d0dc39b33b250c29a7c3ced2cc84c088a2a09489d117ec0926ebb98cf467b8eb483fca4f4766d8e03cb20f30bae6b0a548813b8d54d41b969234d07250098daeae931c83669451ff12b7dd5815f122381f85ac7de89b6e66134255e5b82876795965cd4d9c0409de6242ed38b13e12eda6ae7fda7e521beb528eba34e694f982ac0eb2e06888046566797c58bfe89e11bbff3c393bb9fb3a8c6f5b926b67dd06c254c576ca146de55ed87b628539c5a34ab2f1b5541b13682b809b8f3d831ac50c80556db0ae58f7993e9dc46aa20c42217eb9266bc3c13c4475705ecb7c12a237525a3b8a7d22c2087bd80687d225a01702eeda76d7bc51f7603bde890487cac304a59d701501fd30b741da3bb7bb520198db9558c55dfdeafd51e34be726786819bf30f2a4884cc221bf8ca43662a2ae9e080cb969b6aac2c585cce01e92cc81e5696b2e42d4fa675f863e114561b5d771d557ddca929dfd7e179b39fe66d71232667dd2f16b6a9cfd7cdd7ed0b9bfea72084b9b5ec769dd0f280eb1a2bd5d05bce845923fc383a21eb96a19ab0821a531abd21de82846206825b728beba454e0008bbfa5c03e3b753f9e81a297ac1ddd35e061d395ea10a05aea273f408c0ed6818b80b8f3e3a7f88ce6f27444749179075deefa00117ec8625dfb86e78063c5d1b0afbbd7fbdf2bb12bc42017342739a80abcbdb710ecba0e6d9250b11b55a3d173e7a0f8d1f90c50ad084929c4c0e8b8e2f3a7ada5e5e8cfe93aa19f6bddd840b7df529414ce885a4547dbba56f1e3f43c5e8b24c6f6a52cbed8b3d78d45a235a7540ae29c0d197036c612d113059a28735052007f734e4833552362924223097465c6ae1b212eaa7f3565186aa36944ba55384f3ef15f714c30b4dc946d987ca541f033049683b795620c7ba8c379652d404f0c8dd1b4b71ac224134c8782ea351d74529bdc039ad6cc3461a1826c6bcf60c066611158dd208aa72c1266a32f6de4d77cea857c2da77cd47
message   : 3d507521c3fb52591006a02be88a103e71c8dbc480e569012151413f5e00a1d8
signature : 7d6ccc43cc460718299032ad1051c6af240db4ab4292ac061149b1cac88bdf633d424d6ec6a2a50fa5a15652eb7b726146b2c7783721cb38f6c7f02959fdbc3361c03df15a0b0f7db17a8a24c59ee926509c66ebd560a1a3564e6420f4cb3f5ac92e15b8097fde1cd146fded46c809f90e9495717cbda9173a7b51de8d88a84f1f8d756720bb18bfc604c20cb7940870e7ee1561c495e9524d38de5ef51a0756c9fb2b1e12814ce8aa707309d5baa7e055ef73f7cd90059472305d9dcc5de047e951f973da2daedaebb1dd51ee5c62a20df03a4ed1fa29e2ce000cbe6dff4e4a843a2f70f8fe1429b2e50f18efaf8227d0cd0ca5c4a8c70cf443bb9fe3bd7139219214bf8fc7b1ab8899aeb2d9b4658856e73244a34398e2f461c2ee4e600fe11ebe581144035faf1d0a0914e6af013f122aa84b38a42fddea693ff1f5dd5010ac07d6107cc2b97b563a0b967e159a78ee742e46eb2c5d2d3f4366ffcc015e6236c19a6fa3a0f355724e5382af1f4f227d8e33ab9852229b58d5a6a524c2e095f65be8dd6c020604b42c052acb28fe054dc1d00e6807e77324dd19ae9635be83216ef982e89bbf7dfaf68f6a34bd0664bc185375c131dcdc38852cdf1effbe04fb2a778536efed88d84702e1db0dc192a19e188b2495f267b4d9a9d32d8739dc0a064b45be9979691db9d20ee74550bd1ed05ece852e1ff17a582602f20b561329ec4687c998603a2551bc63f9e72b3a73ea084bd3ba0b4af383636ed2e0dd6a0c6b084f4bfe5e25d89c15b207f92ec58fed737930be7ed13e5106dbb8f4d4e9550b5ab85cb227a8c6db974974403aa6c0ddba9109770e772ae1a9497918273bbc0916ff248fb68a38d296668b6adc0d738cb60f1dabbad544911ffeda34693279fd2ee8a43a34ddde37034abc4613d877aa7ebe57c651b53c05ab175c74acdfecb0472ca037c3e85934f2adcf8db25ee5accb4e1e8446746b263b9f6d1193dd7fb396bc08a40a031b8a3d78c4b39765c7758fdedfc5c02354ab5ac3892c38a1764329103c881f7d6a9b762491150a3a4ead940ec5cb5959779cbb2ca3326d6030086f8fce9ea80fe755d886eb74b54236faf8b6e3ff82978ee78e255eda2aa8e1d86a7941594ee654fde554292c3ac775f5437cdb52ea28faa0894d7d7edd83007a56e76e8addc8c613900f200aece90a9627b43eba7c53f806cfd89bd149ff67c395c5411ac5581604c204104f96f1c92a3425294c8b0b9918b511d3d152e7238a641aa3d7afafd43ff38213f44088355240f45bb9b35701034f5a0f295154d0218bc0cac3b17494620e7e53a6b1df05de58b55d5c009f9cde570d0c32b77227ae0c73d2b98e4ccc9a32fd0925e771759636fc3789e2942169a78ecc084ac4b71c684d95b229c262b33adbca454b93f4c1ac4d5bae33755cf084a85fd0dbb7d617b10d6591ff7448f6821361f6fe9f555584b0c02dc3d6649f2b4b7099b4b5d09e48d075437bdd92438a7093550063321b5d9c035e4ec2f5ef562fddd38cdb5d3b35b7666f20fe08d34db622aeb8aada1213f82a55de97129ee5d4895abf3863a14d278e6addac5ca76a99bf5c704de36ee083f460bafacc4f4b1ad5fd9b60e6158a1a773a76525070304e8ab3c10db566b0b6b72f8e882efd586ea536492f603300060f8abfe783e0d1790114293e01fe3c594d4a62aff5f22d5dcf586c4231afd4ef1566f2069962ff0d27fdf2f0494edcd13c928708aafa70aa24acd8addb1176b4bdc0461552b5d46fa3ddcc5de91cd6f55f424310665243ee5f65d2e78053bd70154de284d7b2f4724594109be6e091653b0131b161eab73cdfb7bbb8418f819bb3a49a8067c1b8fd30b5d61aa5309187328b2dc62d61f12062361d5f72234dbee11da5dbf57f32b068ef13c8da10b2f38714fc773fb753a6ac17e6fed27257079d903185f490b1236d5c834e2360ba6fd28df61e76525599739d19dd0c89229348c45ec5e5ca503313b776cabd0e19b6298828cdfd230c9dd233f0072cf07ce2ce45444f243bf610cac8871460570b8f9343ef5a33b537fae0756d32fac4e1478b4bcdd0f5a393981b89aa9fed8b84068fe836f1953244718b83a1964677062034ab4c2b9d79ce4a287eeb49d866ee335fa57952f08f20d34f4c10c0346e1d215851fe9d6083ca0a1528dc5efbe3ed6bc0ddac07f874239106bce60a1909cb367a1cb20408f7305af88d2f2dda5045682a6ded06287721ec958a1338f6181d763ee6e889e02200b6a15d1da1e476121bde926e6b5432422df1baee39a9230bb6588a9d6648566faf1f893b5447a36ce394a417d251feb4c2b89f4237001733fa3bac81ff8ad06c0bd43a01e0b9765af485f96e2bda4fa827d81b2761336ab93a804a159c07def22ca9d23cf19937786e9796047c6c9a54aef867ab3165aaccbe1ca75755465856a80185dc8a00eb0f3da916e9120cc1f086a84ef454f9003bf9a1480b03c4f4a231a2b5fdd7686272a24811e6baf7b3b2c8288aa3b16e03a92ae537bb3f5b06ae82ad578af6d9e09a4380212bda1609d5c7426211b9570253a91a49a3aff2a99039e948842ec59b2bb15169e7694793a05f1895f3a1b0fe328529a4e12574f48ed021fc23bea860bff95f96092b39d189817143c758126c71ebc042cfa38b04cffc84d38ffb420d95b03a1402ce0617cdd2e24fcbc0413bd239016443817ff8967f66dc3277f9b20c32bb5101160e6c5900b6a8f3804895168fc542ea38f31aa703aec0ec6993fa5a6c721b3f778a73e397742ee334d4839fe46b8efe05701b5ff6767a8af5d5716ad0f344a7536021b15d3eee9de5590789f2207a30879027effdd84f939cf14c9f6d2a0639aaf9bce25a85ca44e60292a44713742d10eaddc46f16d95435fa42d1d223dfbba7626e43bb58de83945d068e8a4d69fc3898ad6dd853a18d63796486f2c7fafc2b3a2968c746e5fe91ccc2e2e6ebee91695d3611bdb2224b272b49d03344c930b3398cb7100f1d41ffd7a277417a248bd20b55274822da1dc311d0e12ce6f849e4fafa85889fed2325b93651e1f0ece50a65da798e654fdd82a297b50e0bb24143bd73c68caa0612b64a4ae98e98410a0ed9e3e23c786db48aa3a65cb7053e3ec80cc1b3b05003e588596bc53e12d9a1434b686cd5b36dee4e5785cce41a52784805384b7f8714cc911c48748bac790008a4b2c13865886e1b9b537f317bd1c638e8901260295516b051b327e10799817cd390c1dd5f55c2d030bc06f37a55ae2ff860b14552d43d9a05090d2c46484d5e6b7173838b8fdaf2f4fe02030f171a222f415a737b848590a1a6abd7dddfeaf3042c2d666c757d9497b2bbc1c2c6d1e31215162d38444e70b4bbcad9eaf9fd00000000000000000012283847
verified   : true

$ g++ -std=c++20 -O3 -I ./include -I ./sha3/include example/dilithium3.cpp && ./a.out
Dilithium @ NIST security level 3
seed      : 5bbb19bf23f34785a5ac4bc576d0b24d486cccba3c3d843483fd602a8b16a55b
pubkey    : 5ba36d15cec6b3efbb9a10ceae710309138cb52659942dfd4025696b2f8bea7387a55c46394e17565ea16f05b6719d50573a3663e05418c0dcb9c6988632b0f23b6681fdd857dae971ab88494f834fe7fe52460bf0766dc50e869b65712c5f37460fe544b63abe6b1b7d03b1a2a5d4a83ac569fc3fd4892027b8e9f034d03fa8a78348ff456145fe6d66092977d3e7de738b5324ee57e246046a03f8047a4fcc98e585e93f81cb27f1751e52608fc0c5e48c3ab461c106d5299b53889411383340d76f4fed7528468eea92cd5280000109d2928d09c6d7bbe1224010b3dcf8f27da5a422c847a548d8fbbe287d2187dc92024236d45e2889f4f956a401d5418e566df6271f07faf0a09f6c2e22af044a1c164a94b9d51d3bd32b185453298bcd19894eb6133d44e733faf6a762261b05bbef06bb13f10011c68ddf74a835f5017d8cc2807ff3e151190a901076919e87f722240c0c6cdd83e0d3796cec5922913f1828fa0a0c7cce2960edfdd1dd5120b1ca2598375251ccb347ac652da86c8774a2f5e9fee598e49653c753c497d00dd8e17f762bffe2eed3f0a126af9aa3c52f045315882e276e68c3a5a19d945344e59871e171765974d499606fb2f9eded071daf3a4af6300988ab93f0d0fc596f53a45447b656e9b3ccebcfcd31d40fe43af2ede45842e781cb045c61ee8fd399c858557c39c4abdba0f9fbfdc58ea55d8edb3e1abf6ca4c6063ff9d081ba35a984dee38866b0f4c54892848a9995bac34b4a940b7bc2ce458f5c726e18bcdbd609eae2ba21240b36da852438cc30e64c8234e5cb7e1d7d89622cd4cce5a7062197500059fe70f9302fbed5ebff73331c882e19dea2e9e434ff848bb861d4a1c0ee165ba3df761ef026102fb2ceeca70c558426fc7736ca09a3401d56e7e6036a1cf86eb342ebbe406d10891671a595a56484c456422096ae1df9fd1578bc8d31b3d05f60d72cf8375d538c88ec8990b2552d65a541f42d49769de45ae700bfc4a1c2baad6b0c2fe591e607f11e2519fff18db104b080debc0126a1da196fec44ee4fcce40087b6e75e9275d0ca222eac7c8eedbaa40fc3e0bfec948aa6a4c209d626ce28e9883498f3df8ea6026c4c21e9c71de7d751d43c7c06363b01cd7bba119b1a849c0cca13e2648d2af445289935cc6fcabff9acb372c91c770f637f5606410179be984d6b98ac4b071b3fb26f921db86226f1fb1ec3d41f4c14bd782745e232b34c4b3f8570dff377648fd2c84ff0fe9cf2a9b429fdec5a339441745e5cc77ff6b8469ef34b00cdca07e52d353383ff2abb42ce1a843b070d4226d15a1fd8078901121a732083fb03e677670729317b3baef123d7f979a67dad3cabb3758141a95fab3470a391f7ce00d9799f853f120853dd280ec7ac83c0c99c2ce0649fa27f55cc95f1973729677c2c17ce4cee6580c92e726080e8d16dcc5981d6536a6a638646c4ac9388c47f6417cd7eebb695170f879f387f4acf44a8729b4b828d746580c2193e32a31496d69189b525215d463db056a92f73f2533134042e2383b05bbff211ca771531a5881d1208a7118844a6e8c12c9461b992d9277d026c9c8dbe2ba9007df47b44fac64f00bb32328eff081813dab9d41c1008fe16ffcc270fb3a361f24371d7ba86a5858f3255491e917720eb9822f50783a18770437ed30392d067e9b0a022d98c237528810f76bf5c6dab31584d7a275a0d71dcaad8e989c5910d0f3d5e3279b30c93c592cdf953c99c3d6470bfe597fb393894b04eb76553851887db011bc2d7fc82993cc3a7ec2cb4643175a840ee5779b3f2e6351c665f3aade4fd8d3a2e492c67b7ea33d7d78b337e417d316cf01990775eb49b00f5e14dea623f259a3fbc7fb1ddce1e19d1fe5cd0b969a08e16215b780079828ea9899c43cf441d7e0061cc6ad54cff4d9cdb35a852a450d39288aff75df0a7619f329df4b2e24b580bc7e2cd9308578f7eacf428578b60279024f30bf2410d8bf46284ddbf86ac2542812d121b8bb6f4c93c228937dd8114bd6a95765f17a2bd686421073164235a5af1209b87e185ef1051a517a1d8b85d1e1622c40d61f4bc7f9ede466e59dfd10e5bc7caae627eb70c730e42e23e1bcda764cbe90a222c9f16d291931731d0e0184e8a8adb3074dd9afe2d99aea76d13097c06f5823ca77789c804edc25358d1cb1a3fa8c0680fdc48e4578a1de81de7cac69be8faf5ba2867cb47a3f1f33c821ebd7e36e33045ab38dbf84c36283ae7fa257d9d65e894c53f3aba547ae35fcb5ce532c09e505e15894cee3da5c7fc7f1dcb82e713a7d002d0b096b120da149b61afbe3cefb01e37560ecd763496dcdea2c8074bd9bf8f8c903eca9383bf562ff99439a9facb01283b19ac46f3a0150c2154db8d0e2c98383732871601f8de27462a81d36dcf1d73868cbce426c3bd97abe85a1cc2cd3bc9acb8571d37224958196d2df572978027f487f564be84be61f0809953ff718ef4a2143e759817cee353f086e94462267ef74602346a5d878d0109759875be06487dd875edf907a80995e9319b2c781d27b90a9cf0e109400d7aac8d160bb9120583c18d44eec5a58c762e907436ef2f3f778281b05744d12c8d84e35e93cbb9a5b1ffd953fd883079be7f8d4126590996b826e481fe21580d40737b74aa3b2bbb2f657da728c8b99fd2d03aa6338e21038a8f901c566f0c47dd78950f5b1cbbc97e2d89f5d70a76c43caac168ce7b
seckey    : 5ba36d15cec6b3efbb9a10ceae710309138cb52659942dfd4025696b2f8bea732dd8449cfed4ea53c14462389b6f1627fb6526602bc7d522c7b65beb79f31e435f2f4492b7b3b440e3daaf62fc7278569a0d318522b608882027a64fa1bb28d903111725341444527670475808883880335725542278648627743825851867574136263741150706405653837505568814010642855157715063745884523375432210762867471117264855638847060447685410237383648776234822728515347768720144452278424140256558420484883767787315704008133350137158622815567723526135667477526063773848820507042807322080042246545216213670423705631271711434302680842162370828462826335424183783533868088845130272223420254174417803246457708221355733188045851404401278358300386140687610834687631402184207014654261660186410512503874624734086214800162681030463815510677483486476880525762513361707060472655674258632861288710525648710888265126840876532780538885622468521667657372833304471440180026235268417027170657656400401265587363168731713738133155867561018837210168822535384100811620245517042623704262120564023438351027480100046742873654421833663475063844868275610747161520511080288487011032222224455344050688834588367413071080051743423068241437863062860185376816228207284364862251124577640151475010353513350417151231676173280437425008557541741668680081467461023365525853037447125514841884085361443256256772387046144608812220541658078663428577851550687531060787238687014381266567578340411683682305631578847146285712273855511010623787326448421153275362206844254022665560060173130016572631301761433551500815548822770712467385650888151314346135458036503003623713571203075534540801645257473287314782627086232168160326551178260368828051666574440356846830504130213270656570431680826846402273575437617503224087702458118541070870134722703111451172202676030231076700455163123354863330182458001878623462107130611678260575200032534387783003544182253380456068078868381376884246143312802340200643872850052107177888484350112802502226788055116876160114330437308324065183116480123888424316201473363804462182248511151676257835113240567463106553405460082671230033321312818112311250017402386858030168515208730072646731667170032213688031502435855261235068554726176575584411680532773606214581850136487721270744814481026347053171228728736824115133442715356706130230385034884328260746206874071815230367712700452715300387117457841317406500584064603810445746742622472431528233142457871625745177313462487551808068076437343863060372507623687737105201633733324873613865380180853684284015603825507341821040828116307386823622747724564468280143866582068245121414105150305808663144653453360521756620865834356411412888722178682884280136754408477171310453688201614620212335107630212617401767751055533254218355381808520652833124585422510882380784845331080677846372345133133405885447185874721513425368800150463785242428367784741528642460606100766310655834562331264083244275617880875177582743312712880037833717611202444042365133838368714757838862518270208784844456818868437211170453402665302478745227351308267588814375862811481513551886354345303505510356612765072d2ccd200bf301d675582cf9c8a285503a21c23e4f5da65440d66cf9a1977883680d3135b3e9fc8ce8c1b40eb96bafc2e5dbf6261322089a2c4543c64ea76d7aeec04446b07ffe052a1cbd87b14f8e13288396fb484a51a193ddf4de89dade46b290186c576e46b46aafd09a08ab5d24e1d01d1891432cbe4a231afedfad191ec0e5ec90e66e73888980a4fca897e9b5a5957859136b57ba8f1ef2a176738df89173dfadfec41c612768005b02af342c0b4b61f327ad1c6ca533e61774f890e1542ef5c5a028a82d95f99c5a620472773bcc8cb6f5761d68e73024af2ca88a7056bbb35daa8e4b15dd0aec9d8aa31e5d4c2b84fcc162e5d7584e0b99ca1cd451f7b3666939c63970dd24f00385557c513d760933011a365c4195c1c7485ff11efacd115ba11d4ba350df14f43822c3e2b1d6852c2d995148c273e43726775b9862ed886e1fcd8d32f92f14aa6409c0a8cee56c1c6593c487dca47596b2e3dde27fcc5c3c4bb3490c38b52833a64c33c0f5b449bcf0580f2abc2f78a2c5bbf76b03ee449f02ad6437dc6bb875955f4489ea5f7954cdf6f38894939e48dd1dc3f3bd32fbbd079e221bfc56bbd4fe516e26dfd39dd9901dfc43a12ef5edf078b2b6cb1a09de9a72e5e1a59f2c1a3b781b47abed96d9a54618be6780d86d4cd02680c2b514d3ae142358d77513100045a9b7d20f68f7356533a5f7c0c25b5e0be23312a44ef4cc635ce92781ee0d4e58538fc97c11ebe2fbf13a10173844392b5353f00680ea4289c9cadb38fc22f501fd5bbd1c1319e6044b20e5b8bc74078737fc307e8d68fa0aa5d3e4e5423fae6f06703d34e5c2d3246c73a675466977afa6d240ceea349a4d2bc2c9756aa08c6307c5097cb05ca49648dd4e06d78a375b4eb37c29d8543e32f78454166705837cdf518469e53d532149f8d03c70c3827ea9e48a1fcee648e95a1fae05c09858a91278406149379813a54fdee2c54e7b3a6cc8320786271d9245f16a844a93f1a6cd5eb1c550e78f26b5097764c8ecad78aa8fc2da17f5deab96de034f7e0c1f66357ac48a0104c766f78b0f8ad5fb57871592990ab512cc472e9df7a460cd37a8e078aa6a5071d787ba02db378d833f3dc81b2f55bd2bf761ef268525ab6a620250b0b95b49e4b6a78a252b5690b1ca3d76a723028306a1fa07bdbc44b75e4ff77c80af044d524a4e20d9e8c6769b8f6afbf59285b1806e883e5688640e4c8b6eadf2971f5efa49aa83c31463605b1638f616b08acf53ecb58e1af64c683d8da2d2f1c24d65e65c61f270aff0f3e48576fbd003babdf2b12d1713836506c08f053721a10d8237ae3fc6927e4b4390718459021d62967150ebcaa3ad5a8a43461a80c6b00b2acc476b3d3f437850005e114fabd3498cd4384cbd389d43ffd8397e39fd06bf2e4e2ae017d395021555c4c59addae7fed4546ced095b0a9f5e17aff3ba4a914e16b107b529f20bf0e9caa7059f0490c19bb07206d0102f90067b6b55b945a91cab2931536fa2c06d3ca8429342325e0536e6352a823fde5e9480b1490856d166ec967af10f97e61263cba05e66f832d85192e4875c513f4b1476befbe88b278137ccdaed6249f8e1fe9d1f45ab1171098c37430870c0a27eb70b6186ab6ec9570edf155f3ca4cb2b703576d287b235ded93183042407b427875f8b2b06ad271e2ddbd75acd5c3a894a34fc27b4e19ef49abd1c73a07585d200cd5d200a7b9f758ab1b544d7e904d8c82d459aa3de401e6d88de9b24c03448dfe4e01fccb05e316a997fd77756a73c31576fcf31039e365b6cda6ce38596cb12c34d9cf8483f81399ff6a1dc93fc6bfc7422dee577fbdfbda84ca685ed2f86f2f8f0f633fcaf5adada6b8582b8d75a3faf776422c97a86ca004e72fb5c3cc3ffeb434258e24351a19246ce3857b91687b79d775b0991a7de18a6036471c2fa799dcae9ee9d486de2ad2f9b5655b962dc2a2fd2c87a4939f9957df37bfd004ecb792b5c02bf9a86588b1ef603eca95d56d8e89c599e9a4432df827ed4c2aeca5751737efe3d82f0dfd00449e125964ffa5a90c9448af84cdf86719838c826c3f16dfe25321041cd86813a52d969c1f6c182c282d7898d6c83f63d6c361565bf558ea6324d341c7f6825fa6a8e2f847ddba4e02163a64001ef5787240a8a1e4a886b291299c7b3ebeddde3d8c7f7a2cc131814009c1c836aa3b7b62e8c4c7f110cda41d75161169d0ab97fb61dfd64a216bb65a83a0d09a69fc7aacdaf78c09b9fd6ee3ebd807313825e67f2e0372400fc10c9bc492b2e434090287397ca986c859f1daafb0da40fea27b2c40a0c3031cc7237263fc897a2200aca46c7223cfb17430232e82f232e064c1b5e3ad0f42e9edbded5ea30bccfac549d37bbe6ffa6a3a394992a63483464a8ac9819a1e9a843716aead623ed8e1538ff1164b8e5d75145c41f6db13b5d56544745229df0b9d758c9b6390d7f6a1316e0bc8f594e332299bfdbb0eac58abf4e7c68a96090717c00ee0b4029c1d0477d9e91a5078812ec976838f1b17be12090845bf673d520f06368941e7a3ab4eb5961bb560cc7f440aeb820cc0743839c40364202ef44a5bb7914a20d7b92df3b6f4c642cf6e17ebc1cb50d80262613cbb9e951ad6b8ae109ecc4f7ef13dc9620e2c00e9dea72d5b59608c4cbf5bd8e00e68574e7167b275f3a7849169b1bab1f24fcdeb921a347cd523c50ad958651b85b495ac726fafedeebc8225e7ca35e57a56b9baa80da6a4b2a1a529d6325b6db905d64d2e1bd2af2ef0bd086150317cec12d7d9439cb7ad2e5247c9af3741a18e993f8286decbdf7c5174ca9a01ff1301c847e540c5340924f9254b61a970ac62ec071c9571d63bb7b0ac8623370e894834a63b0a3cfa01e234352172b322a60762d402e66138ee2b90510626e9eb05025685d9dde0b5b535ea30711ee0d71f96038f29608a528e3b525c5ed2a81a7e1a689b4a9cb3737984d3b0048035fcea3cdf574d1ead5e5690b0ca123ba80a02e1122690535fcfc38aa4cce487cc526d2d1f331bf091f02dffeb6a45aa5df31e9d0ccd28a77dbfc23a093309042cb89e68174b6f4a0af953aaf15645d9a5a7bfdffbce1d1e351b02f319e3709398832a2ab8e99e6e72c018fd6147cb73a5d02fdabd84877790aac3408f96636e1cbf143c9ebc901dd4c62cff58727235ae6016d7e71b58da12a251813c0d1a8942d76c48271811f36ca64ba5e1799bbea2023d2cbf09d05da4b1176eb771917e134ab6094c27484f4c99a513beb831103358ba671ba0deca7389a1b0f702c9fc3c95a748e222879918e09bfbc757f65ab38032e511dd54b2711f460ffb3b97c79d1fe934f92f0e2d82a784bd3b936dde7ec4907ee18d36088e2bb452b57f442d11f817bd824696855a96ca6027e5c1025efe2654c7c58203522fed75f021b2d5bc957d16a4b57a072fd5674e8bacac723fbbc6f113fa3f306d33ba20f249123d3d7d82886285b7dc68875e74b36f4ed94
message   : 927aca99690703df9c8dfc9d01ff699b9181af0dc0828fc5435b44a9c19e09f3
signature : aaafb2a6e7801f66a0d42f609bf557f0b8f66a48bd47822fb5b971c8e0aa9ac8325f4af29a29dfd30ac3503c0becea8cc942e01cb8908b8e374c811ee3bda14eed43e70bbae8badec871fcb9d1692a0ce07faaf2787789cb10fb00a07440d9648fae02dcfbc18bbd25f929d2bc50e0886be8b4b783960701f91cae8f69c731edd546dfc127658756ffac13b4522bf95027a807984161b8b9210b0efbde94d47f178e7c6e762e3ceb3a300816ae1389d4ee94693a8b1e17d01ce96a55ce8e65fb75ff409c994154df71e7ba0180ca63c1c2a706bf05300af670d59c8ae309705aa8182b5b7e1d413aa81739775ff9787a1f2e78500b379c76f7f1a926000dac0b6140b0d36308922743c7b02d308b3164045f55c6f71e36637a0917bac4422eca16524ba2b96825b402a8ffcdedf50024d914d1ee1164aec9de62c5b6d8b212c6eb961baf42cdc4a360b94ce8b448b79484b93b3f16ac87b3b27e62faad99f944b14e5c81e6374fbd13488a7c76f25a6450bc4bd86beab3aff7f3046468599f4c09e014f73a4bdcb1eddc24b430192a95073cdf297b818db211ab64b10c33fbb8a1c0c903a8877a30754b346ca0febb093b7f5363a2d09b3822e87d82a6cfb0d259a2f3a40623e7ab9b2345008427f41f8a1277b93f698ffd3db643bc33c732d0637159de19645d26768a7d3c9d42314124076592351ce10752516138ff83893bab95cbb8fa83f1bb236b9adaf78bd3f5a195ba9cb3531ac20b42e083035bbce13aeb7ddb0fa127edf4c60e1e73206cf88f1381df7359b0c5b108afaeef1753d6e4360ba81eb3ba16d0f82e88e78aff97c4a4e65ecdb698a9acd0ac20707406aeb9180c0b73a8acb79c24d52772a4ecf00e224114a63062e1e7f479bc224de5b254d1b26b51bb5cc99ee9556d7d5019ca1725e53294fbc7accd7a6fad28863c725ee13f6ccc2c83166a58bbd7c72638f671079ecb7d6f292afcc04879e588be38cd1f42e42e4988be62b80f85f53a7f213bf11689d587b825be64fa4aea1f0f0b7ee19c0f0cff5b3a23cb5cc6739ce11c0a91753f38b3df5f3189774e50accf6e32e57cfbacd4615b74a726fdd38255d8be3f1d8f528961c6a8bc7cb535aa2ce760fb3091e2cee15565846b89f1ffead187ae6250b3322cae81ad121d2be7393ffdc7f5d445dedc313a55fc33272415e8f6314bbf3e544bef03a06faa20b23b3c0357f6cf62390fb7d4fdb84b3c505b875d353417416931868a8838a2b4d4da7c0ca8ba69c632f60f9479ba157ee47f999bab895de2ee6c22d6846707ed6873ae3059294c3bc13b9ff8742162edcf58f8785d40fabadd7dafd7f9065ab260afe65fdc49997a7d1327b29b197ffb9ffd632ad0badec8d440a0e196e36ca40d3b93d644645f647af9466526bca0e65283bf6b46d2bb36d7c7314e055efea4c8264e7b5c349086a7984649a081fcad9a908fcdcc11fa0a001b66f10737f824a4a9a73be5b2649d633ea045c5cf1a1ec458091f0c078b1a762b264afc0e820cd4d9283c8f324fb9d460994b073429a7e787e54118de4a8982525ffe8982153f66674e97a5da4bdc06dd4ec193ff2ae2fa0c6c316eee3c9b5a94013ff02560870da97778bf6e38e8ccba4c670b791c1074c11de379d55c86d6c173beeb975ea1ba1cfacacc17f18aaf232b3bbaff6034ec24b141102ab942840c5cd81e344b135bd3dd327dc15b50fc4956fa06a323f0fcdc297d856f27df77e75d83191568f969a55c0d2d4ac989897d1357e6291cb3bb59c354255e9be91a4a43021cf2db81c0b43a52e1d755907cd5dd66ebfb2ef31e353be0601a9f6b7b4c0fd425b5bed51ae3fbc8a91de3be93ede254f2ec358dacb21c89b27d0fcb7f689e5d9024d5fce5328a9a75588cb075613a11bcad5b53961cd10eb3968a460aaca2ecaef89c9983793d72a8ad76c4945560aaead86f6ac53d67fe9b3ceea2eb528b14c6bc0bb0dd68d7c7b5711c18d2b77f7fb73db3913c7b4514caead8a08c8c21aea482bd1207a22e8381d3322b71d58ecd53d909e3927d96f05fdeb6b55816b656f5ef1c2e60cc9536804571ad52121ce4fab9ebf05f4e4fee3449e91493f15d1e342a43b7ae0748ccbf86601b5ac569c655c4d662fd9b975b6d96ea7c9d65eed0c265168e201e889b1f8529044a78a54f7007bfb9211e20b7cdd78f1e93e0ad8655dc0b1dbbfcfc6a619c00729d471413c3b3338117f07117a1aaef0fb946a341cb80ef2a322b2a223ffc67feb53972236291fbe4650d6b3e1e1fc71eb48c4e4924a2cd8e1ef301e951ea2b0a327cefa0b262fd814bcb10a08af2c187522219ebd15c2e375e2d8574c7ae8bd5e10eb298ae4f7010554556342db1441903344a3dd7df5753df258a5facbf6e3eec838cbeb43bd2032001aa75fb53fb3d0119a477c65debcb4607053c82625d7533745df99f0608c75efca47728ac576cde03335113c9d91a89268582ee62dc1b074b6bbee456eba2131d6458a6d301afee7a4b82780f4ccfff06aa42c3c0ca453dae2d7f5dd5c7e89438457e73bd9e733aff5c4892e903bbbea854de6eff382c3a908772839174907c10a6349356eda8b50a2bc292a30ddfb80448dbe6931b8df4cbc01d6c1c0f497ab09ca1f046a9594e086a94d0bdf5d3ad8c9df035afbb2db9db0fcbd68b30ad06c1b43c762ee98ba68be638afe1b80c0481041e85636ab91982b13bd08d63f6b53ded7ca3916a3d3238504cc73fedd1f64cf993cda58ec4a3cff0d0aa6bf97230c918227094245bfd2b83c6a5bf22b20cdd27ec78dac7c9a891cd442e77fddec158b1f7eac6ea6ef741ac57eda1a0eea4791bc9fffaf193b5e3d606e7a6427ae611acdf6fd3829a1ea556ccdda872615a0571e7f9ed7398d468eaf5d8b771f6eab27323f2e5d1365b75fe748793604682a9245f68b9ca5c710eb8c08d41f8216e79c58d290d3297da2684a2fa6bcf2ce28160430beb89e63b3e8d1e81997c6b0d94b7eb2a69de52d2a56aaa27ad0bcd8e6fe618e64213d1f49582e7dae0ae4866bcc36a605acc51c57d3aefc86d6a9171cde62dae378f1bbf2a15aa48995318c74b6e665a7ef9081ac8990360016c683fd7bfca32c81b013d9e92e5641ff4c59e7b7f1c4fd7e2ac77206d5ff3ea8114c6550dc6b0f74335f0391ac55232180f9bfa5b6a6a234219b5022809b96cdfa5b1eb49074375067e8d734c214bca32753ae7a0d2397d27332c264e25041c14e48024531cf0bc24af50a7fc1c0f07c5c2ae4c30efcbbfc4efc19e9d06fdfaa636acfb642a0158e668020610d9de2e3fdd8b0da35ce82e2294e451f675a2b40a399bddb298c298403401af9762d6a92d573bf8a2481d896ce5f486c869d3433ef60e8a433f4177ceffb3bdb3c8f6f4f5fd552c63d8b4c84d37102f3580a99001ee2954c8f1ce9fafed122a8e2d4817ca4505aa7cc0e024365d524494c049f579d7d27ce09a1b4c41247ace7082b7832ce3d0df10d2089de0eab4f300d02127390e98103cfafc66db6be6f2cf166b920740f206155bcc7d2f45665297fefa1a10d26c1a0821cd1befc4d1e842f9944973a99ac98fe508eadbd014ed6471057f747b3a5d5a9da0ac17829b0d6eaa47ffa44ca38804d2e32c12a6e9ab62d310964f97c05e9060f7758b2489e2b815d8942ce78e32130278a762133cba4d4ca9ba82d53b49f68b7a523e17ead7988a69629ccfe630517fcfd563971a403996895697bcdc59cd73f33b86d723e437afe6c3f8d30aa9ec17652aa73826eed60b78f7d1b46128b729467ea66db33b5cf1d93fecdc2d39c21e269e3027a641c46816c95d5e8a7aa57c68fee22e6cc00aef1772b9cbc8ddf5dc20ad9de39a19a5045584b31abddcabc1fe3275c1c6bf7bf4ddfff70666291e46e38825d7964a96a9bf105cb4a540cf8eb98b3e8e4414d734ffbb9a21da1bc271b24e52ae504d590a284678eb45ac72fda4065ff441ec243966315f3b7b00951b2d2f4573b702bba2f5f39bc885f17905ecee4ffb85f51ec89563263d62161be5cadb38cab3aec83e5b630b31096cd7e37b4218220206b0fea434977874b11ecb88f599666c7a2e585988c44037b31b56172b757c754182ccc259717f626377ac28b6f34d3ac8e97524fa5b0cd1e62599c50d691fc34394d50ae50610898ab79146f9e49f734c5bd05234e53715498e520b8a4f825e0846ceeb49f0ec85c225b8885d7e0d23cf5a8438e97302f07b24a7561f4b1734103d5736b26293901ae3998dee8ce262d65e6ad692e52c3cf1f542f59f59e4f2c5aeb30ff93a8b334acdfcdd4ef9159968a7a9e440e240ddd0c533b950aa06904843a29a7df2e819129bfba8f25d4cadfecb1b1fd8049f821fd5ad4b5d9bbe0c893eb41e62ab1e42696f2fe5358b06a03f980b2c996915f9a431241334d3d3320d021f3842e9590cbfba5131496d04a4105ef50cacc1a7c8e7ae4d725ae0f4bee5be440803568ec9a5f09fe082dda820ba57de08569f06367da23c8a5a4e0666f021f4acc974af00c59f9cf50efb7c6e97db14c3c72dfac1f68602233886d7dd0d1e2d7286a9d1eb1c48758a92171c7eec215c5d70747e86888ec02b3842788b90ba000000000000000000000000000000060e13172128
verified   : true

$ g++ -std=c++20 -O3 -I ./include -I ./sha3/include example/dilithium5.cpp && ./a.out

Dilithium @ NIST security level 5
seed      : deab598241a9080924fe8f015f9bbdaae9f31ce5878aab732db0f4178eeda7e2
pubkey    : 110ca09ce3ae4579cdc9942b4a2a3ef6a52e138f7e83ee8bb113fab5ea2ed5340d3ea1fad313ebcee40cde0a05b3a993a4f9af08ce866aeff5240706e66efa2668ed60b300efc16ae3c91cb7ab0fac4fff069f3d9d5371e3fc7943ea03db4f92f78b7eb07d9aa16f05df0f979002c2be3cd262c3181c884e92f926f779b8e0a6ef19eef0781bb1618ee3509bdd1f72bdb4f85af43224dacdc97146d5fa269a9197ca91c04b3098b9998e8450db76ec4a384873eec32a7a687055c529287eee475c98fb99556401f6ea65ce1b0b2fe80e8517596359bf34c975c943222ce85f618305cf9ba061cc02e6b074dd4f1253441875ba2e3c331d60721ca4ec3f39c2c198d70472fd9d2f5c64f658dc300d125c03067855b7141edc7b5812f5302d7109f04649643a52648eccce8217619beef048bfedd21288eb7fbde706ccdba8f4d0b13f257768af378f7cbfe6f34c03918a991c1a6edddd172e294323f30f9fb09031b16c536d615ae8c36af19d7436bcef9690addab78d5cbce2a70286957a783b5247e818d74974c9af99be3434332063018cfed5ad39df94ebe0e8ed955f784152a36a4f9ecaccb14d70f51a6def8f3a3d84600902b97d0ab9a35355d8391356c2d41da20e0595f5daf263cb9f960f8319ee89a425fb76b66bea02f956859b34cad12129b0934b3f79a86e2f834ee5fcf6e18c7b356d616ed5324c5b9cc29d91fb2ab1135f6c6a7946aa438bcc20f1b6318db43c51dad9d73d056c14d0c468cb22cf25aa87c25a3e91b16b73ec2a701dc8653edc4368a26c3bb0aa0f4e77bf18fd68f079d06373ee35ecdd446638a78ea233d6d19b7765b5028a89a200942c9ccc10a1d7a0228e19b5f8b2a01fd41960922109b0a4f957df1369d25c24ce91ab0bc383863e72b4871c8b132b0ab694dbb317429bc0d80b3ce2b5966ec08d0e57c28f520a57a6cccb888d61887fb8e9c349b4479012bd4bed7b65354f9c83cede04d48caec4b4a799ad4fbebefd9d7e15238a0f562a246931774babbcef0c5e2eb530cdc63663bf30306d03d28de7b97c51c0272e6ae73541f7821e13abecf203ca70830542a56baa4be40f05da85a0a66416ddc3e62baa357338eaa7569f1fddded4fd84496f5444a2c9048e546ee7bdd49fe80282c0ac08e5fb37474c62d6eb400088ed74af8fac2b109b0b048a7c4f061cf32951ceb922f6011c3c49164f482c3a3920f68a36c42ce73f1c86659856a8a60bb948213a8f494c282af90263beb933c5f4ac9a44307ab1bf50b126ae8fc6e2acae33764387299b9ededc9019c6dcdd62841e4d4197cb8f41b5622c8001e46be23d68dc079304a3cef43e568ec92742ab847822a371303c2154adacafd63a05c07a191906e3b52a9dc2ef39e2c290a534193c58a9fee17f0dcde2232120c3e77c622f2726817985ca71e2da4f727be94f2b5bf32a773283938530b2f71c596c68889f7261ca4a11fa28ff9c702b3dc6afa3f88b2741c47a82309967de81b2ee13a6d354ae266de57f7acaece4b09e03bd8498d9ab2b939a0da4f13d5678cec7f91db70d09341ed5007fbd238d4e3d06cc46252064c052b14b48f1b100416b7b7aaa9b6acb56e84a77f877a4e181d7c79d4d692f6d8df8d6eb9ffe14b226d65d8c3e0715025b30645095ab1fa7c789ac39b7c678f10d6dff6885b551a32470ab390b6d4e444b7a2dbc2f386935c2b1f5419f15d8fa9f2e5dc13300b7161bc7fd4005a8e9a71f9d7a78e2b37f4343f4aeb5ec20331d0e6b4be1b39bffb1fbb08c94dfc5113a6a38a6ac02c02d3e881640508bd58fb22c2b1364ef98ec9a1c5088dcab62678ad7bf906e6fed6a04cc57c0111a12940efd6ca9c2d56c8b976d0542fb70a08c33ffa5baf95a262b901e05957f6abbd56cb7e95276ca1b81057fbd83deb5ee6649c06f5615c72854524791150337288cea4e221ae1045c18b612c5b9a697db0a05a04c81ffe82f21a8a274c238e3f32c40f4a1d83f01f3fac759626cb11b4ba572c663c2329a6cbd190957e7757a86965160b0c01f170f0eff94cefe304252e2fea9b3ed74dc82c37cefae092f86d5578f2f541716ec09d68f957c97070cdf1fe6e090960bad3ec6a0d4240049c371d20cf41357d07b9f6cec28b36ce3586d00954c09b60c05c2561a3f7aff8ac4f04dcd70c7ac7ca35a3edfb6d0ff99ba5d657ae5b50551a22aa78d241bc150cd1f6699a307273e88d19e85e25d4c081ab22a746ad1ce8efbc0f7d8679e929cb7d46b8a71931a3e2795163414d0ed5baf0a6db9fb476416e559d2b898b44e152336fb53a6a919ac64a21f20b78af463f4b8092bc33bd5dde522a696f0b22474b8fed28e22400f304681452ac52985f7e479fea5e10247c93c6586a717472453250bccbff4719abbedd656ec749a1663c1864a7a5a56473fe93d70c042f119d760355dd7f45f60a9261bcf8d6210041ec58d90d7118219f0e9cfe2230aa4bbff5af320d40dd0b4b13acc70d2866621eca64f4d8caabc13a80216cd9b10c24a9e95e5b04a34069eb6b882a7a6bf1bbe005c58c400637dcec4e73e90174b02bc82ce99eb06d5bf285353dedae4456d2ed3e2bf425b817d0ee1e8b467dc6b018e54a87c62b5c784b2d7c16c5b87bc0df3fe9989a3834862ac19e5d0242cf019ff8b57d0221dd5f81d515abe813227937f1f9747a008ddf7ebfdf8a2d1a4ee4cab3330a530d8ba132751f06a2da4405cefc2a938d10df9a085830b974cdf24799e6b919b748b15ced841f832ecad23a80d86fd2ab7d5e00170e73451c16c63da9afc6d998d902681a0c48c0d9be3d80a3b46914791316bb81a577b3cd1e080e5571dfd768e6940350a36b6cbf56ea9aabb459121524bf516e171a6038667973eeca8ab23655455f4232042d2b9de784706a292e7db475fa8bc48da8596e9086cec21ebc7542366f7520e13d75e570f2724ad6dfc4e98d08128aba8a4dd2d3a5d33f442282f4eb3a756d7bdce183e3b0c4197ea630041b9fb29317bafa753f3b5bfc04f68c042a5bdd490f1fd0d2ac85a32858368c780b53b3e768aec1a60e4302c456f1322dbea45dde0b4f8e003f1791c639315901bc72aae58a7a023d35128d71d7001b7d63a9851fbb32fdab44010f3bc4096bc18be597737db1706ea6f5e33860999043375a5b22ab7a595a8ee016ef04221d075a6ed8b3e6a77b1f704a6b51fc4573c43576abf35ab2dddfe6f81867f658cc0bbd846ad530620430c2bc2ce176d882f36c9036242c9de7cfd5066667e8c3348385638e545dbe72891076fe575adb0687ef007c84853efc8164a10c655f8af24588d816310b422b0b8b133315d84629d393c1cb4087057e9f748690d8bc683a4596acd3f1acefea335b6c2650574b99ad001db83017ee73219cd011fad0bf1f0d7322cffa40449e4afd2564aec83b73be3c37988591ae98b5d9a3a7a1d7a9b1582f402b1cb3339f5ee4ae08eaacddfa70f00acd1246b7ef01e7d42fa41ec74988beb7c8abd65815e29cee31acd5e448f7eedaf83a563201fd2b17937595a33aa5f4729fd9479d7c15431069cc71d9ebd4bfe8c62e5faf889d22602f87385c608c33f7ea27ff4ab220867077434cd103827ed4fad3633b70d770dfcf32057739b20bf1320fa0f5adff7c9
seckey    : 110ca09ce3ae4579cdc9942b4a2a3ef6a52e138f7e83ee8bb113fab5ea2ed5341fee44bf7ea8679218000399284f267f5fa75bad14980cbfb93724aaaf079b5f06ee24231eb7ad6182682de148916f2b1a36175f7232fb8e7d1ef8fc9ce9fcbd843661cb342e8880848c466e60a270a2a60903324e02938989b60d09316d0bc129d8928c092385a1066dc216659c8080ccc850c04684803429d3364c8106889b942d5b0002da1068c30409a4888109176c4cb08c9830240a2484e1a860223031a11405230385614690821832632609a3c600a4406001066181328819c79193a4054822862225429c36640b415122070191a491023130c2188c1313700105281ab94544304a14464a12966c89b428188189243626024910db80648bb46d0894005a248943b849ca3428caa80d99b00c08a66881000c18098e42960c4310281395910838904c80010b2626ca466dd1160da48861913629c124710b9710c4029012484c1887480bb3481234689828110c884d021810400826c44646139131209630a3980562224ad306241117861b26108aa871213185502870c3480da1186e44b08100b8850840705b024618838dc9106122b269c3846181242e03340808381220c810c8b80058147014074900910d0246109106720b252554882520322943c86914407112122d21b10dc8948ce034509bb684d8a66dda92640b09811b436dd3967148307060182442a461a4b00ddc1830a3924003a96913c94043942c43a645d80089e1982d1bc74d20c164944666e0408c0b9369d1c48122b230203960041960e3828982442a20a24c0a0608c2c41100818424030e00a5681285081b460e1bb885003751493428c2320421060983989164087043a24d2312464a38461bb950a318661a22020b2852041491603471183948044666e22212dba804cc468809436e030286d438058b8430c1360c18b88182068521c6445902625a1692a132204b884513b841c4b08d604422042830201680181452d1a82ca21009e48664589680111945d8c24c5a464623264ec4a40119a38924c8601bc9409140686424528bc48949882c9306286396854b8464514071990260d2123114198199326e4c466009846c9a360d24848120a145a21471e1102ac404262200411c40814398645a162100c48503a90d14892903320c8a3602c1826c8c364190c4410b0808da402802818441c40d02014110c924c8200412352c9b389124902c0b4710da92652287095382715400464a96801c457292024813384ad1064560284c80104c44c21110b7700a26100b392982328858200192026a0ba9891c132958365091a224c8040c4944501a098d002361d9920088268904212c493465603892a304018a3430c4947181444d230621cc4401da228c492624a1868d824244934609231701c13065c3380223381242c281c98688010980a2824c51006c0c150423118199268964a029e2b44d60b484998609a2868c60366d1bc74d02a44dd8a8701a23488b286821943000b46c13a26c18498651942021002811b328642861e1488c1b820911b960c9385108a10114110451949000470958064960846809139021046d0b0865081746cb224e240081e4b6498b826912b844dca0898a206d0a402a84408ce2c86192407054c86909b8400c432210b72d5c844894446dd9402a804044db20609ca0410043490034668ab68422344ec1365061402609060c939628cc488e1a4602924880591210dc846909392d9c905090065112484a0ab370042084202752ca1072424232843425cc18644a8284d3b6314a228e1b364d0a0202e2404299424c2117285438011c240a10c86481800d22c72852b46044182c5028694bb63011980813050ce322518ac09004c5419a8051093286e10828800602093222c34642a1923022a129223669d8300c2091645b24220122714cc00ddb920499388221909191b045034031812282413292d2902083222064289064080800498ae4389091344dcc104858820de3b831a28651101745e24431211460d21824101701a446681bb090504429da44291b3801c1082a8b16254a046023c4450b94814c1085e0422411312cd1b89122a169cc360ec1c0680b9121d1164621494821960c180572898481a4c62c92061251265e6ca1215689db0ca41115710adbdc126cb46ce99244b2ddc50c0784540df41d6614222403734f8e01a45db1793b2216809f26988879a6da532f45ab01c0a2a09aacafa27308cc82a6bb461c1a12ea9da49d445d788ebbb06195cce84a09759aab6538bfd913f196b831c82e2927f803d409f8114ae8b00afb35b74271e49c281997feb2f212da1f451108e13bb009884267be1925ae55685c46fdb3d7b266e901a5f9ce9b25b4e7d5fa196e1dc012deb9345ed0e51a5b755aab16e39de99d468efbd9a1d7e043caf14539322f7f821661141339a5721772ddee0482bbb257a25c0e8a37c5da501505cc5749bfe32fa1db2335b89d859e140c1ebf268282c68340f0befc5b2d94eb802e67a7dc00e8fa67396a3e8e3f976c6dd74adf93129361591ab4a39ecb77828d32a50e2a0bee993ce10a868d72a4ff9eef02a9682682741b7a17387a9bbed24bd26bfc72ab8f3be3c5fea02d24bac040cd7d4f0ade11e61d9cfb7b1a75f9cf318103c144002699b04a1f3da83e0ad950448b0433bc73c7957436acb0a175596ad13cbad49dfdf2ed1d9d90187e9589bf888f65a6ff65e04cfe0dd9a4d8279a15f7a6215244f66a9ead4ecbb2c748d16c560c1de960364e1db7873080823cf4be4e91c1305b7592c8ee45195f40a0f318157ebcbfa0b3679a9e5a09276579b4740809a6e52308ccb76b6ea397bd2c86591e4abd17e3586d9158391e51d2d39ec2cb39fdb9ad5d923606b462eaf52f14e25a79d5813456cc5b570231ef613898900e34e684f9219fd042be031dc3330c0bb45ce084f433bfb1eda46ada1c409ca35ad886fe733f508e6a465174ad7b91032c41fa3564acddf91de93decc6396bc390d8956ed064981a9ba50405d9d0d7dd0013d6f252b10dcf23da4ddcdaffa9a6e245be6c7efc9acf62f8c626535ec95da9ebd34ecee779f29c006764f3904d495b5c60d6d6c62daa334599e4a4a2fb26a193ad56413a2c0ef32dddef8b166e43c04f306e788cd9cae9ce2deab2fafc7456423ce1458508736c126aebdc40d5a3d58e16056648885c0a20e614b778a5ed5ec4ffc19144f172e08a86522bb07489beb991bc0e8fd6ab5bcf4c4d03c18580357b5f9dd357c16d6e3e9b66b00970fdb9d067d31c67bc935df4519c13de5da59b4aee0e4b2fefe30b15f3281adebd75883a7c37febdb8a8e9fdeb3b0999d5993feabf548d0f470eb86a8b9df459fc0a7c35c4864e7515da5fb11dce004ce857f5002f4cb0e5ec055c800c6e2f88ab63feec741af406d621c204e636da0d93c4be24ed7184d4a2c14c478ab11cb83ac2674e3aa40f07e3e3a0742174ee3962908cb6c167d83282827dfa682268a67ea47412df3283859103a45888e5fd3ead3db8f92d2f4ba40ede0179abeec5a9e311a83db04260b530b71f57d4c20e7ea07f2d82bdd2e5d97fab0edf0a631bc742e851114dd115c388a9e42a0366a5633b386c410dcd743a736825412df1db08937dc7292a850492cc9f03d341e0f01e2234bcc2e8f4546359ab00fbc6ae0f94ed74e13ba7adba51e33757c13489b4f8d748f15b4658db1600915bced550781022568f6485acbac88babb0f105d22723275f59636bf50ff6135272c9cf3458282859d7f96a475b6f53048742dae111405cf8050f01d00cfb9d0ce7a288bc584d54dcf17dfc582d843dba5bff57821b62803dd318d38fdc783ba173ec760aa99040d970f95482c63fa08f46da7f753f4dab05afb391a21e42b55ce9ce95f1e8bc166df71a65986aaf4228bbad2ea2287bd155bb399981ba4efb8a0c75f022d02fd84d24848fde0896daa33d8eb46857c555649e57b691f8f4c59c3b0727fdcd65149dacd957e50576f85e654a82ab3ccaa5f6396ab1509f18fdd8e1a484bc0eb72952b2794d834eb8759e813f417833ce916938e08f5de6574ca2953e59aae2e9002fa40bff84de005087e5640d3b4c53c8a1d062c947c3fb33b9e7b82245fadcb90f78b0186dcf898bf39df23e30ec1e372cddc7b4452114ae42f2f390242903ba84e902a76dbdf6564ffb036995860b05f554a1ae6fae5a0e230ec007ea49075f13abcb0bad50fb205e4bdf5dc1e99924de16c898e0a0c655f86bf4c304b7730a8fe8da7bd79b1e803b65c0a2c3a872bbeeaf6e9a4941dd3df76ef33ab61139444d85106e481447d734ac80e9262f59ab365dcc7a79a84e760107a4141e32980a2ee2c9d177e1039e8fc702d2b17653c065980097ce79640515034bcae5ff2f4f3c651ad612a42dbce25c1c657ef362c04d6439c3303293625df89320543b23f50532a0023df952d438f1c37751b44a37493083f7ba22371ac23dd1ed31a0b92ce05b4098ed4bb4ca6813f1616e08504377abf72af982e2fc6c66a9776d9481f736247d9bf54fcca690c4fa0cfbe84c41a16b4479e58e31898c107d0539fc1afeae4d0d4b448f77b971ddf206aa40c3d8d583401f9a0c415ceda7093653f292cf68e27dfe3db85b53d251c000ace44fa7921528cffec4685f3e47b899f2f6bbffbb20e8f5a4f7ee19d42db97c23d349a0323d51f3b00593dfcbf76c806484f61626499f867e99a009340ba0c593f212edbff992933d8c84709b0c89d1541d9e160c24e44d1ddf68aa669af612a7629d38f665fc0f8c0567f91e548c85f62c8495eee7d638200f7130a5185b38f582b601968c9b66f73eb478317f0edc171d643d5ac2cc744c9b543627ddfd1adad08b555f85fcaac32c70e5085c6991a524be56a076cb58e23f0997a5766e760d2a301a651138479b3c62556b37a404e635e87d8617a44c4cc544efd22bb856fc86b55b5ba30b480d63ecf5014f3f9773d3a29ffccc36138f3cc73d0e1f3dea807d52858bdc7a808788c7145aa1b2d45d6884f0f3a7321b694d1814fb49ff58e962bf4edf8bac33b512e624045655d28070e69b036ee0ce90587390939b37f08c81eebbdd5549f4fa61e24b729aaa977655bbe9f74c4492c673d257084e65407e9443e7b8f72c90af04eacbd8a591348fb577d174024d0ef23068221999d0433b0791341a4e83ae2ca0ac904ece4c8a01e1918d5107e0bdb83dff7d58f636cc9bf91722349f494fafe06339dec2a70eb891e39142922272979f0e7b639df7aace41d8f915c788cc4445599e0bac517f0c8e28f31568003bd6d7de5bc2a15263d3389ff46d2f730a1b0ccc5892d1b765945c9b63015c64055df1d8863e6fdbd47e762d5894ff57f7adeedde16db3c6be1cdd5433454737c340e52186b72996e1f439a3c3addb90a2d67a9c1440c53aa86c922e8577801f76693ea5235d643d3b2596dd8894b760db48c6e7128bf1243dc89d62bddf1f54d6b1256751c7a25d361e9eebea0f0b44165f6fe052d89cbdbd176a224def1f119072602ae49b5fe3ec00b84798a6a5b1f92b61a27b4f21445251a07653551274b4bbc660025f2755cf7e8ded157cc71979012000e7dd432880532c6022e2be5324b5cc2ba35061c65670325af76cd60d32c7580cea97ad58d76e9171c56fa6ef76b9b750d7fa68eaee07837145ccc0f7d3c439e2a6d2eefae9369d4bc69723ca82dd9bf0af3bddc94ee23743b8555df9712bd507898a5fe8d6929b25a20fd5d9aba46cfabb1a6fd4f2bc410761173e104b09fbb3d7726b3f04ed4ea16b22ff31543d4157d724747d0cfdd3ada8fb0029a0dcc52d9e077c7f0d49bcd2e18823b2ad857bb5fc5e1c9f2db1ff7494b61803e5a8af2629594b8c96512cacd7440a4f40c89b7fcbfe4270d76a9fccbd71bf739e336bfba4de6018a241ad7f0dd06656e11d5d19a81bdf7f4caab04f471eea68e5c53bf8c3b5906b8d960f36ce07ed3d2dcea336a31cb291cfeca46f8a3d010f90a190e7864598dbbca153a9eb20b1b43cc3a80ecf697c95207f9d4b9afcc1b112ec9afaa532a7dc160efebbea25b9f3e4c7018dd0d5c17ee15ddc95445dffe01988b16c1903dd9f6c634ddc93c4815b2578a9e4d1b8e08e6e892e31804b63c7b048a78368edf8b8372fd81f58bd6ac34fd2a3de76833436ae39dea3f7637854f0ae8690667787f75b06c60135ecd0b16ca48ea92e024de7fbaa2c82587c765d3f6c0a3136bd0b3e04fe3bc8be37f2bebddd5dcb5b7b07491976311ca3ef8b9e0d5621415594985fac2fdf72d02616bb59bdf84516c10b062de92f2580b071abcd66692a01559bc2a9fc7d77bb89d6d0265e76160082de14aeabc473fbd7fa1fc6e5fab1d79f7147169f9c84bd2565271fb8b907695498d22c7e55b4bca4e81a6c3432445df7b519b22aa915423177303f72ed491db578ab9ee70c2f47578144f7c6a2d623269b04be09122d288491ae4e162e9642a48597c98e6110b015928e97eee1ef78dbfdef2e67042d4298d59e90867d355bfd5dd8b6b489a780842a08e23113c78df0cfc2908da6efdc9e2f9e2af890738368ef6119cbc6ec3ded0858cce8baf848832645286fd68b8005412b4c364271731cd3e981de0ec76d8638eaa786a622df6ccf022e3c4e1ef775175514e2164e838e86f0dcbc90eff8992ca85ebab3f612dd2ff9d00789728f9290b63ceef7aaf06b486b9ee1ffc56e969768937581a816e10d4ade5e4ebca2459a1247ef39f42dbb231590736107ab7b09ba427a802bb2fa500f7b34a3a13271724ca06512331744813eb6ea4390dd9a8d5be113cf
message   : 8c160851adc4864f04c7de399e2ff1604e099aa1424296e9f5e4733655c5b342
signature : ecda81e8e53ec02c6943d1a21b88904534c591efeade5bcf9fc12e235006faaec7c276f76495513d95b76c3aab4fd5e284e7643efdf8ac49771a9aa8eb7895e40b4f4e15e821a1fed5bfd19f429257a0803cd929661bda44dd2040effd1540e988e4a09c60b75c43f5ea069fc7feec327b3c8e82497d9603571dc627da354448937e3a11717636915d8782419f8cba9ff5d5833115b23c43be7f1d78e45dcfa880fc00e0d67ff9de1549c827eefe4d2473bbdfaa7047455b65cb2f238912d9518b46277a498f1cdbde4d2e216c07f9486add416116bcd534a3406014898f255de598ed3ca06a7062864c3dd0d9cb3661901914fb66f3212bd99c256e617f247bbc9685e305f11a4b130bce569bf4578db6bc3ed0524ec50faff0a7f64e109cdf8f343c60a058413d13da5b9e6cff6db8f85e9464ae580319de00843e8080681cfa929dda7c0ca2c4c308d2e7420539121cca29cb40a20f50bd94b20a06fb97999d5f34b5316067e1651e90668b8a956dd1cddcd6265a2db2f7d06a5808a88b6717cbffb1a1710fccb0c90042f015fd195c622ecc81907aca31938ecb1a01266675417dbaa732700d00c00805f0b79ba7c1178467e6d98b9cb9a7b5a75067a594036579dd3306fc959b09b59f07465d8f5f95d4a5f5f708cdebf5ca835727e69137db56b53cfe43dd28141ae5cdf75d347927eac05f540f7d4218dc92d6093603a47596dc0330ed183006cab7d4749f52edb644c02ad40b027d48075b704cb195f6540bbd678da73708f803e26b59fdd221e33c9339a448082bda1d1dbfc09c3d68a837a3b74aa40cf872173dc6ebb81997913362328e01aedf0f610b897d1828f90c4c6cd51a9b88dd708508f08cc18319473bbe7d1685f3a059df77614fcbfc9144ce5297862e516701eb07cc200e01d7832eacc87876612fdf1631b17ce06391201c88e327aec69448f48e022cd32fc119cee11910d17a944319dccfcf6b3db42b12d2c85021146dbf7e5c0a99db9ed5c0b604236c2c94d3e7b34db10f26595934f20eacf014f78cd029155b4d5aa42bca4c5bd2ed8b442575e901f0f411382b0dbfb922339427a37d806df80da6aaced05fe782bc77803539bbcdded05c5f31f7f4f02de4fe07483a2cc1252aff92661675070f371324ccc2b487fb37e3102d0541df3881cc8391ab365736983d58da420b371e8dd2a3dc15712ba659b1f4cde4803277ace5f793684b4c73ae82b058e0c15d714a663d26ee5f11b52b3ce2d1a513829def9dfcf8ecc8b5e72a06a6fe25e6e9fc97082f47fe1f9474b3c86f49cf8aa4f0d8736f18f03698b326578f8ec160767f412161469120f0b5507b929b0c6216fe026cd89f99222e6c468163519122313d199b2ac9b36315c8c77aff7fa396b183a07fff16f2cd6bd45b4c4754abd7edb2094b8126b86d95726f6118d5a8d76454fdd8370d8b37333363fdfc3d12d87cd34d6f097c73d763a416841018a4810aded7f07c7df8506a62d49507036af59894d8dce1c5875194995b24a7adc9b796f55d7cb22bbaaad4baf79bd2a8c606585dc0fb7765ccedd6559e1936ce390fcc8e4077cb3c7bbac09f7749e783811e9712bb12d44d12103f3037f56825597db62cbc78836dc87cc4830b8d51b94911ed9f6ca83650f7c3dcbab42100a236928a3b96aee0d3c6f7dc1725c83716f15ca8f3780fc73bfbd1cdb973ef5b2e9d41b2f9162586728b3e12b769a99fed3133c8b84a7f24a52d49f2d9a0a65ccfb3e75640af993f5595a4fce39c5691c8bb69a52536bca9feafa9eb5587c532faf2c42b457ea19f3629e82cd4dfc6fbe84923e0fc658f8d2c0b986b57c21fdd60b594e81102cc8d1db68196218213185d2b33c23b733519e996216b85343450ecac3b7e218bb5836b6950c8c7eb2c6f1b27134a2ca395716498673c08ebd44d47098e8582723091a50fde725e30f3c6cb9a71c5a74ce316bcce88bf6f7bce9295e5e5f9adc42f4f0339d98b20526aec56bea86c4f5730ccbec8618beb9064fb108503e92deaa7d780714b3338add78e8e79bb7432dee102d78e16448ef2cf80775a19aa045d7d4a61b58b5dff7bb716a65773f965d9e9e93df8a2833ca561292ea60a62454b0882b6fbd8015c70ddf6a8746e10a7e3f29691aad03e38d6d3b09edea5e2bd7c713874f225f8ee83c33323e598df38314035f0d1ab0d82200f8838cdda4df55fad07039a97c8481ff01868c5eeddbcef4eb7bb77f1e46f6ea4d68c46d182ea12e08b34bdcc52067a2ed30fc69d606dd1d34e3e047667e2d22b1e4a9a2da55ec3fd3c80ebf727812b541d7f2e71c5a0dcffcad698fba09c70255c16e369f8a8fb9587f5529e6a022e55782995a6e3b0cf3b09e95d7a87a0361248db1d06ff338b9ad691c1670e34962c2dacf6ba5dbe37f341e291306014d5e6763b454a56e3151b73b022473839dd1410387c36c5ba9cb25f7894499d8288a98fa090422a51d945f3945d045a64807fd956e78a0a4e3ff20fdd304b82986b997140dc3a92f6476ab1ea5f3abbab1789ff69dc602c4964973f21f50ba2222ef0b088ac5e3bb48bb85db9f28231badcb3b4947d94655421b60cbf9028d4da7ad8b6ef38c7c1e98529502c603fe774cb7b09b5617597f8d7ee94e8f16fadd19ccb0efe13a0987ed6dc06a62a5f2a112560e23f792622fbfe58de10b3510c00571909c7dbb0400a256f2c795bdd5b1905fc8d9b45fe559f93c0c9b7ce029d74e5d80ec2225cd694bf0558ceea62901f9c450ec05e403b1134a165897bcbb91c00b0c02bd7d0591c1c7e408aee3959edeb565a872adbfc3709eadc99f37f0f2bc65b2c5cb82d3627a92e7d91bc0eef65625b0a591c8bbd0dfbf30db44452e0358936aa96a4200488dc1cb62c464f0462bad35b2ce84c187b27546d7e482ef19d704603bd9fedcbf790edeed4c004744eae2b7124a4af28c48073eca66f928a9e701c7e34ce8a8560c405a75a4cad2a15c0e8bd49f0035356f9cf2037f0a48b675917e6d0d639e5937af73bde8de74592671f5f86a0fcdf189b4eec8487a4991527ec22c38e39b855c26ba49d9040fe890d1553676430ddd47864c9302cd9297638a4b27865bb8bed4ec24110c2a07b0f2993fc7088a3c6d6c24f88bd0c949aa74e5526ec67a9bd79caef88cc9d586b51470818caa5e28f4f7ab7824e19ce74e0bffafd3e6f2502960211b806037ee64bf174813f20c65708bb3f11002ae59d60a21fc6238b89d548519b78ee42a11d0e3740917aed8dbcd68a09777059a5611386f481b088b82aa08084e36d9879af992e244063a318b6d6dc87430a389ec82ee86508f9010f813f69310b3bef279d421f3aa07ab5d62d0439189803b398b35c106f1ac1edbe1abfe8a0d86913f47abf894d4e0f9d3f6c13c2e55dfa85a6715e98c4c219016ae90027562859909d207b539e48b9863693ca627a79c2075b6f2224efd2d416cba36db66846bfa55baf259587554cfac02149d12b8529da79cccb5655c1d73ce2ee93ea04ba800636c65f38c8bea852c6a31b246e0fc62136b7d1e5515b9ac68a46083fe6e9ec070eef69f8a0534188b1e02176cfb48440b1d78ced4e657e4d8c95de65fcac8b20a96b223433a991bb1b757777aca304c6af0142e9e5861a7f96e5f02f3b4e1000987faab1279638752c6ad3f35120b0356a522a40a8c143ac9683dc26f9d43e8a507887a1b4bd50dde04b6db62dcb36634cf22cd2c58ad7f321729d90171092c2d19b4caa1f2c6d27ce72a4ae15a1ffe8dbe1eb1486ecc5d60d6fc295d13f9ea99b5e071cd72eb3438c459e6503e8ff9e4c1fec8d728206ff4d325f02fc3f73923faa7d2b3610b5c7ca93e8aa75b7d90ea822385cec929ddd57e060e36e4086169862832a267f5ca57ca90bf775181cede794d03d7b95dd1dd4a4bc8d4e017802b12c3a9e3db00e53577ea30be77a19a83211d9b9be259cb550d8767bd013a30e1240acac4c815ecebdbbc22fb0c1e7a6970b9d48d811938f46c23f8ccfea4a95eff33aa3f11e96c5545ac1064b60f6cf7bef23d3ca1372488eca16449b02001a6c27c2836fe258343352c4b98d3839992760a4656da5b6cb4442cf244155567054fdfc4c7344bb7084bd3cae507e791bcda16df3e8fff67820cc816721be9eed684981c9f6c1a8600e15c0630928bd50568651f6045cfaae675956f9c694d4290ba5683c985da66f1c6665b19734a8a5b928ee06f336be97b4b9a837d36b66cce21f0cbbe6e8c0710305747f726d3520adc5aaca5c2a23f78dc5896b68fb2f30743e088a410a84a59785d7a11d17ee905d7d2538e42c70c78b80df65df3c8dc4255a58759f5bdddcf32e8d6d304b0dfd16b3e0734fbc6cdab3b6374bfc67cdd02976b653ce61312eacdc6c6c09f5189f72d7f1c8ae3d141c59e3c2f91917d659fc0c3e3d8771ce1c4bf447669bd5147728213a91eb6b7146db6d6d48da7ab4a1b507316e10815c39b04ab63b6bb903554a8d8e8aa33400cdc7f24a682818f225cd91d1974a8212128215ba652e2ce17815670fc40c5d38ca464466d2312d4d16b8d412e54820a8046fe62097ab60b09b07c217e6152d7e5dce9c1cb727cce243f8954843f984ac7796731c5c80440cf9977a44404b82b8d7c558be00376bef33ef6f6a54b037f326877bc3b65d9203f19af943c0c7a38e41efed2f1b83f5e760b1df7a177d37f82aba212583da0f7ff880bdeec5ca23620273c8a2469b524125b40a12c810e1a42fb5f171c31417a1743137268e949e21e86b9f9bda1fa4ef35aaa62f6c9ff60c452b9775d5b4118d87a612c59faac029b612a08d07296431cf6dc1827af0f905877e3378aa3a6060b599addff7a65c380c899f0321e3dc6410a1cd3ef00f6608d9d3178b9924fa2fa6745bf3658fca4c2e2ac4b44885d0b1327f967eeef45b472229de3ef6603c071e1102b5205a78eac607c8b1dad655e698afc98bc83f373619e4d72d4f2a0124cdabe81e2585385a349aea529b4a7a1d85ed65d6811ede130bf68735497cfffe6a0e600b957b64be2df3209b148c9915f6036a665666511562cb47a9b8cd989dc326baeca97ba2a603dce10b2ef2f6760674e660d2c7fbe2325d9762c827df98a0b93fbe6bf39ef4eb614b06a614ff83bfeb770d2c554294427cde50ae9ef7de151f86bf80e4fda76d589be1f1bfc94795466757c2a405c9828d630176809673dcdd7d40d0c062cbf0842a8e635408cc2adcdbc6a1ac0ef8d8a568f265b2f8bcfbf8ee03db9fbcea0ee890315766bcb1fd481110e0f94532e4667a5251dca276515217898dbb48e00754d1a5e9ae3cc25cc48289e4d356e315ab4ad1820b1f73889f52734f6e8130967aa69622ebe631844947221a564d783010f8a690ddd1163219c392f7a32a39c4a7c6717cf4fe7b575872be96c1ba0085728bb993160775b61f548adc96556219a255ef052e92b7658532bd4d69d45ebe98ca8efe3c13810bdbb45ab1b25dd26f5532cf51918770942d8f4a6e8988faccf6aa573ca044686a61b8828de04b1efbf1c448b3ed6637aaa16f33f251b8b8bead3b0fed08a24e88ea5298d5e01f9e3ee502fbd523100a5c8d4a4a9de3c5cd594a18c312e8c2a55d35f05f9bc7269ccd436ce7c60ad6a627494d406fbe249e7f79a3bff0d3f1b66365f9f5909993b88b094048acc383e1e92c5b5f430c90b060306814125c39f62f003ee7a3adb675ea2653a61080688784ef4081625149a80f95d3f19d11623bc279a13cf257fb1cf71ddd3a60f166ad6b806aaabf87e031538a9fbc24cc7cd6e05c43311329a20116f7a7eace010cd168e191426ec9cb0dc09f2d9055447167ee5150adf2e2d1c21898e17ef87b9db52713431ee48fe1123d180109854486516ed08577ce80355ebdb69c7218c2788be0821b22ad1b3aac73eba942cb2c0756f59d5a0b41a34384f734be0d21dfbac165d1d17090cf41ef486335a1f5b47256bb9a3698e2720cf3267f2e2d1bf947073a931ce65c17cb15d91e3c03ebf9bb0e109bb2d6af8be3cc4fda8997dcc0a88b778647bf42f0afc002e3ccdbc8d587a0722a6b4d05ee8f19dc4d8e7b0033f4912416098892ec5e1d8e3d4c463f61bcff6d22cbd9690f845b2e645a5660f996abeb72f54f31df6ff1d5779f3e880ab04d11c35890908cde99af71ad4e6ed19c53db51ed7181fc6d3601877e233473d2820f56fa51ca3629fdf1464cee5f3ce5d0eea14abd70eda757e064998e0b04b2fd086d437abbb3ea7680efd12f2eeeaf44f9d964f880a3ad952782946aabadb2c7c14a81a8255ad9013ae2fe05d8fcca66d4e835c35a367b375dc57fd38e54386525124cb8e7865707318dcad6e3e83d40515d6e83afb1c9d90e3e61757faee7fc2b3c4e526c7b7fc3d4eb1f2a40596e9caad3f31724628615433d68accae10000000000000000000000000000000000000000071119232c303237
verified   : true
```

## Important Implementation Note

You'll notice Dilithium 

- key generation API accepts a 32 -bytes seed
- while randomized signing API accepts a 64 -bytes seed ( it's safe to pass `nullptr` when using default deterministic signing )

for all three instantiations of Dilithium signing scheme, this is what you, as an API user, need to ensure that you always call those routines with uniformly random sampled seeds.

I provide you with a PRNG implementation, which lives in [include/prng.hpp](./include/prng.hpp). Before you start using that, I want you to take a moment and understand what can be the implication of using the default constructor of `prng::prng_t`. 

- In case default constructor is used, `std::random_device` is requested for 32 random bytes ( in form of eight `uint32_t`s ), which is hashed using SHAKE128 XOF. When you request ( using `read()` function ) arbitrary many random bytes from that initialized PRNG, it's actually squeezed out from SHAKE128 XOF state. Now one thing to note here is `std::random_device` itself is not guaranteed to provide you with system randomness in all possible usecases/ targets. It's an implementation defined behaviour. So it's better to be careful. Read https://en.cppreference.com/w/cpp/numeric/random/random_device/random_device 's notes section. 
- But there's another way of using `prng::prng_t` - you can use its explicit constructor for creating a PRNG by hashing N -many random bytes, supplied as input. These N bytes input can be presampled from any secure randomness source that you may have access to. After that same underlying SHAKE128 XOF is used for squeezing arbitrary many bytes arbitrary many times from PRNG.

```cpp
#include "prng.hpp"

// Prefer N to be >= 32
constexpr size_t slen = 32; // = N bytes
uint8_t seed[slen];

// fill `seed` with N many random bytes

// default initialization ( recommended only if you're sure that target system provides you with reliable randomness source when accessing `std::random_device` )
prng::prng_t prng0;
// explicit initialization ( safer alternative )
prng::prng_t prng1{seed, slen};
```
