> [!CAUTION]
> This ML-DSA implementation is conformant with ML-DSA draft standard @ https://doi.org/10.6028/NIST.FIPS.204.ipd. I also try to make it timing leakage free, using `dudect` (see https://github.com/oreparaz/dudect) -based tests, but be informed that this implementation is not yet audited. *If you consider using it in production, be careful !*

# ML-DSA (formerly known as Dilithium)

Module-Lattice-Based Digital Signature Standard by NIST.

## Motivation

ML-DSA is being standardized by NIST as post-quantum secure digital signature algorithm (DSA), which can be used for verifying the authenticity of digital messages, giving recipient party confidence that the message indeed came from the known sender. ML-DSA's security is based on hardness of finding short vectors in lattice i.e. it's a lattice-based Post Quantum Cryptographic (PQC) construction.

ML-DSA offers following three algorithms.

Algorithm | What does it do ?
--- | --:
KeyGen | It takes a 32 -bytes seed, which is used for *deterministically* computing a ML-DSA keypair i.e. both public key and secret key.
Sign | It takes a 32 -bytes seed, a ML-DSA secret key and a N (>=0) -bytes message as input, producing ML-DSA signature bytes. For default and recommended **hedged** message signing, one must provide with 32B random seed. For **deterministic** message signing, one should simply fill seed with 32 zero bytes.
Verify | It takes a ML-DSA public key, N (>=0) -bytes message and ML-DSA signature, returning boolean value, denoting status of successful signature verification operation.

Here I'm maintaining `ml-dsa` as a C++20 header-only constexpr library, implementing NIST FIPS 204 ML-DSA, supporting ML-DSA-{44, 65, 87} parameter sets, as defined in table 1 of ML-DSA draft standard. For more details on using this library, see [below](#usage).

> [!NOTE]
> Find ML-DSA draft standard @ https://doi.org/10.6028/NIST.FIPS.204.ipd, which you should refer to when understanding intricate details of this implementation.

## Prerequisites

- A C++ compiler with C++20 standard library such as `clang++`/ `g++`.

```bash
$ clang++ --version
Ubuntu clang version 17.0.6 (9ubuntu1)
Target: x86_64-pc-linux-gnu
Thread model: posix
InstalledDir: /usr/bin

$ g++ --version
g++ (Ubuntu 14-20240412-0ubuntu1) 14.0.1 20240412 (experimental) [master r14-9935-g67e1433a94f]
```

- System development utilities such as `make`, `cmake`.

```bash
$ make --version
GNU Make 4.3

$ cmake --version
cmake version 3.28.3
```

- For testing correctness and compatibility of this ML-DSA implementation, you need to globally install `google-test` library and headers. Follow guide @ https://github.com/google/googletest/tree/main/googletest#standalone-cmake-project, if you don't have it installed.
- For benchmarking ML-DSA algorithms, you must have `google-benchmark` header and library globally installed. I found guide @ https://github.com/google/benchmark#installation helpful.

> [!NOTE]
> If you are on a machine running GNU/Linux kernel and you want to obtain CPU cycle count for ML-DSA routines, you should consider building `google-benchmark` library with `libPFM` support, following https://gist.github.com/itzmeanjan/05dc3e946f635d00c5e0b21aae6203a7, a step-by-step guide. Find more about `libPFM` @ https://perfmon2.sourceforge.net.

> [!TIP]
> Git submodule based dependencies will normally be imported automatically, but in case that doesn't work, you can manually initialize and update them by issuing `$ git submodule update --init` from inside the root of this repository.

## Testing

For ensuring functional correctness of this library implementation of ML-DSA and conformance with the ML-DSA draft standard, issue following command.

> [!NOTE]
> ML-DSA Known Answer Tests, living in this [directory](./kats), are generated following the procedure, described in https://gist.github.com/itzmeanjan/d14afc3866b82119221682f0f3c9822d.

```bash
make -j            # Run tests without any sort of sanitizers
make asan_test -j  # Run tests with AddressSanitizer enabled
make ubsan_test -j # Run tests with UndefinedBehaviourSanitizer enabled
```

```bash
PASSED TESTS (12/12):
       3 ms: build/test.out ML_DSA.HintBitPolynomialEncodingDecoding
       4 ms: build/test.out ML_DSA.HashingToABall
       4 ms: build/test.out ML_DSA.PolynomialEncodingDecoding
      13 ms: build/test.out ML_DSA.Power2Round
      42 ms: build/test.out ML_DSA.MakingAndUsingOfHintBits
      78 ms: build/test.out ML_DSA.ML_DSA_44_KeygenSignVerifyFlow
     116 ms: build/test.out ML_DSA.ML_DSA_44_KnownAnswerTests
     123 ms: build/test.out ML_DSA.ML_DSA_87_KeygenSignVerifyFlow
     126 ms: build/test.out ML_DSA.ML_DSA_65_KeygenSignVerifyFlow
     170 ms: build/test.out ML_DSA.ML_DSA_65_KnownAnswerTests
     276 ms: build/test.out ML_DSA.ML_DSA_87_KnownAnswerTests
     767 ms: build/test.out ML_DSA.ArithmeticOverZq
```

You can run timing leakage tests, using `dudect`, execute following

> [!NOTE]
> `dudect` is integrated into this library implementation of ML-DSA to find any sort of timing leakages. It checks for constant-timeness of most of the vital internal functions. Though it doesn't check constant-timeness of functions which use uniform rejection sampling, such as expansion of public matrix `A` or sampling of the vectors `s1`, `s2` or hashing to a ball etc..

```bash
# Can only be built and run on x86_64 machine.
make dudect_test_build -j

# Before running the constant-time tests, it's a good idea to put all CPU cores on "performance" mode.
# You may find the guide @ https://github.com/google/benchmark/blob/main/docs/reducing_variance.md helpful.

timeout 10m taskset -c 0 ./build/dudect/test_ml_dsa_44.out
timeout 10m taskset -c 0 ./build/dudect/test_ml_dsa_65.out
timeout 10m taskset -c 0 ./build/dudect/test_ml_dsa_87.out
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

> [!WARNING]
> Relying only on average timing measurement for understanding performance characteristics of ML-DSA `sign` algorithm may not be a good idea, given that it's a post-quantum digital signature scheme of **"Fiat-Shamir with Aborts"** paradigm - simply put, during signing procedure it may need to abort and restart again, multiple times, based on what message is being signed or what random seed is being used for default **hedged** signing. So it's a better idea to also compute other statistics such as minimum, maximum and *median* ( pretty useful ) when timing execution of `sign` procedure. In following benchmark results, you'll see such statistics demonstrating broader performance characteristics of ML-DSA `sign` procedure for various parameter sets.

Benchmarking key generation, signing and verification algorithms for various instantiations of ML-DSA can be done, by issuing

```bash
make benchmark -j  # If you haven't built google-benchmark library with libPFM support.
make perf -j       # If you have built google-benchmark library with libPFM support.
```

> [!CAUTION] 
> Ensure you've put all CPU cores on **performance** mode, before running benchmarks, follow guide @ https://github.com/google/benchmark/blob/main/docs/reducing_variance.md.

### On 12th Gen Intel(R) Core(TM) i7-1260P

Compiled with **gcc version 14.0.1 20240412**.

```bash
$ uname -srm
Linux 6.8.0-39-generic x86_64
```

```bash
2024-08-01T21:31:03+05:30
Running ./build/perf.out
Run on (16 X 399.747 MHz CPU s)
CPU Caches:
  L1 Data 48 KiB (x8)
  L1 Instruction 32 KiB (x8)
  L2 Unified 1280 KiB (x8)
  L3 Unified 18432 KiB (x1)
Load Average: 0.66, 0.51, 0.68
-------------------------------------------------------------------------------------------------
Benchmark                           Time             CPU   Iterations     CYCLES items_per_second
-------------------------------------------------------------------------------------------------
ml_dsa_44_sign/32_mean            256 us          256 us           32   1.19738M       5.15337k/s
ml_dsa_44_sign/32_median          208 us          208 us           32   975.694k       4.80075k/s
ml_dsa_44_sign/32_stddev          153 us          153 us           32   718.524k       2.41135k/s
ml_dsa_44_sign/32_cv            59.92 %         59.92 %            32     60.01%           46.79%
ml_dsa_44_sign/32_min             111 us          111 us           32    519.64k       1.54879k/s
ml_dsa_44_sign/32_max             646 us          646 us           32   3.02393M       9.00608k/s
ml_dsa_65_sign/32_mean            528 us          528 us           32   2.47044M        2.7681k/s
ml_dsa_65_sign/32_median          435 us          435 us           32   2.03615M       2.29703k/s
ml_dsa_65_sign/32_stddev          323 us          323 us           32   1.50973M       1.77477k/s
ml_dsa_65_sign/32_cv            61.17 %         61.16 %            32     61.11%           64.12%
ml_dsa_65_sign/32_min             168 us          168 us           32   786.787k        689.632/s
ml_dsa_65_sign/32_max            1450 us         1450 us           32   6.74411M       5.95098k/s
ml_dsa_65_keygen_mean            96.4 us         96.4 us           32    445.21k       10.3759k/s
ml_dsa_65_keygen_median          95.8 us         95.8 us           32   445.059k        10.434k/s
ml_dsa_65_keygen_stddev          1.45 us         1.45 us           32    954.675        153.336/s
ml_dsa_65_keygen_cv              1.51 %          1.51 %            32      0.21%            1.48%
ml_dsa_65_keygen_min             95.0 us         95.0 us           32   443.683k       9.92974k/s
ml_dsa_65_keygen_max              101 us          101 us           32    448.46k       10.5263k/s
ml_dsa_44_verify/32_mean         63.0 us         63.0 us           32   292.696k        15.879k/s
ml_dsa_44_verify/32_median       62.7 us         62.7 us           32    293.31k       15.9509k/s
ml_dsa_44_verify/32_stddev       1.08 us         1.07 us           32   3.13005k        250.737/s
ml_dsa_44_verify/32_cv           1.71 %          1.71 %            32      1.07%            1.58%
ml_dsa_44_verify/32_min          62.5 us         62.5 us           32   275.657k       14.5621k/s
ml_dsa_44_verify/32_max          68.7 us         68.7 us           32    294.23k       15.9932k/s
ml_dsa_44_keygen_mean            57.3 us         57.3 us           32   265.696k       17.4677k/s
ml_dsa_44_keygen_median          57.3 us         57.3 us           32   265.677k       17.4477k/s
ml_dsa_44_keygen_stddev         0.715 us        0.714 us           32   2.68481k        218.859/s
ml_dsa_44_keygen_cv              1.25 %          1.25 %            32      1.01%            1.25%
ml_dsa_44_keygen_min             55.7 us         55.7 us           32   260.727k       17.1027k/s
ml_dsa_44_keygen_max             58.5 us         58.5 us           32    271.89k         17.95k/s
ml_dsa_87_verify/32_mean          167 us          167 us           32   772.107k       6.00369k/s
ml_dsa_87_verify/32_median        166 us          166 us           32   772.107k       6.02761k/s
ml_dsa_87_verify/32_stddev       1.68 us         1.68 us           32   3.49109k        60.0648/s
ml_dsa_87_verify/32_cv           1.01 %          1.01 %            32      0.45%            1.00%
ml_dsa_87_verify/32_min           164 us          164 us           32   765.234k       5.84894k/s
ml_dsa_87_verify/32_max           171 us          171 us           32   777.966k       6.08969k/s
ml_dsa_87_sign/32_mean            567 us          567 us           32   2.65035M       2.30625k/s
ml_dsa_87_sign/32_median          450 us          450 us           32   2.10825M        2.2221k/s
ml_dsa_87_sign/32_stddev          334 us          334 us           32   1.56144M       1.06567k/s
ml_dsa_87_sign/32_cv            58.84 %         58.84 %            32     58.91%           46.21%
ml_dsa_87_sign/32_min             260 us          260 us           32   1.21113M        632.573/s
ml_dsa_87_sign/32_max            1581 us         1581 us           32   7.39516M        3.8513k/s
ml_dsa_65_verify/32_mean          101 us          101 us           32   470.568k       9.86459k/s
ml_dsa_65_verify/32_median        101 us          101 us           32   470.285k       9.89717k/s
ml_dsa_65_verify/32_stddev      0.898 us        0.897 us           32   1.23881k        86.4831/s
ml_dsa_65_verify/32_cv           0.89 %          0.88 %            32      0.26%            0.88%
ml_dsa_65_verify/32_min           100 us          100 us           32   468.725k       9.66729k/s
ml_dsa_65_verify/32_max           103 us          103 us           32   473.467k       9.95904k/s
ml_dsa_87_keygen_mean             160 us          160 us           32   735.698k       6.23867k/s
ml_dsa_87_keygen_median           160 us          160 us           32   735.071k       6.24064k/s
ml_dsa_87_keygen_stddev          2.92 us         2.92 us           32   6.17827k        113.438/s
ml_dsa_87_keygen_cv              1.82 %          1.82 %            32      0.84%            1.82%
ml_dsa_87_keygen_min              155 us          155 us           32    723.74k       6.00106k/s
ml_dsa_87_keygen_max              167 us          167 us           32   748.988k       6.44194k/s
```

## Usage

`ml-dsa` is a header-only C++20 constexpr library, mainly targeting 64 -bit desktop/ server grade platforms, which is also pretty easy to use. Let's see how to get started with it.

- Clone `ml-dsa` repository.

```bash
cd

# Multi-step cloning and importing of submodules.
git clone https://github.com/itzmeanjan/ml-dsa.git && pushd ml-dsa && git submodule update --init && popd
# Or do single step cloning and importing of submodules.
git clone https://github.com/itzmeanjan/ml-dsa.git --recurse-submodules
# Or clone and then run tests, which will automatically bring in dependencies.
git clone https://github.com/itzmeanjan/ml-dsa.git && pushd ml-dsa && make -j && popd
```

- Write a program which makes use of ML-DSA-{44, 65, 87} key generation, signing and verification API ( all of these functions and constants, representing how many bytes of memory to allocate for holding seeds, public/ secret key and signature, live under `ml_dsa_{44,65,87}::` namespace ), while importing proper header files.


```c++
// main.cpp

// In case interested in using ML-DSA-65 or ML-DSA-87 API, import "ml_dsa_65.hpp" or "ml_dsa_87.hpp" 
// and use keygen/ sign/ verify functions living either under `ml_dsa_65::` or `ml_dsa_87::` namespace.
#include "ml_dsa/ml_dsa_44.hpp"
#include "ml_dsa/internals/rng/prng.hpp"

int main() {
    // --- --- --- Key Generation --- --- ---

    std::array<uint8_t, ml_dsa_44::KeygenSeedByteLen> seed{};
    std::array<uint8_t, ml_dsa_44::PubKeyByteLen> pubkey{};
    std::array<uint8_t, ml_dsa_44::SecKeyByteLen> seckey{};

    // PRNG.
    // Be careful, read API documentation in `ml_dsa/internals/rng/prng.hpp` before you consider using it in production.
    ml_dsa_prng::prng_t<128> prng;
    prng.read(seed);

    ml_dsa_44::keygen(seed, pubkey, seckey);

    // --- --- --- Message Signing --- --- ---

    std::array<uint8_t, ml_dsa_44::SigningSeedByteLen> rnd{};
    std::array<uint8_t, ml_dsa_44::SigByteLen> sig{};

    // 32 -bytes randomness, for default and recommended *hedged* message signing.
    prng.read(rnd);
    // For deterministic message signing, uncomment following statement, while commenting above statement.
    // std::fill(rnd.begin(), rnd.end(), 0);

    constexpr size_t msg_byte_len = 32; // message byte length can be >= 0
    std::array<uint8_t, msg_byte_len> msg{};

    // Sample a psuedo-random message, to be signed.
    prng.read(msg);

    ml_dsa_44::sign(rnd, seckey, msg, sig);

    // --- --- --- Signature Verification --- --- ---

    const bool is_valid = ml_dsa_44::verify(pubkey, msg, sig);
    assert(is_valid);

    return 0;
}
```

- Finally compile your program, while letting your compiler know where it can find `ml-dsa` and its dependency headers.

```bash
# Assuming `ml-dsa` was cloned just under $HOME

ML_DSA_HEADERS=~/ml-dsa/include
SHA3_HEADERS=~/ml-dsa/sha3/include

g++ -std=c++20 -Wall -Wextra -pedantic -O3 -march=native -I $ML_DSA_HEADERS -I $SHA3_HEADERS main.cpp
```

ML-DSA Variant | Namespace | Header
--- | --- | ---
ML-DSA-44 Routines | ml_dsa_44:: | include/ml_dsa/ml_dsa_44.hpp
ML-DSA-65 Routines | ml_dsa_65:: | include/ml_dsa/ml_dsa_65.hpp
ML-DSA-87 Routines | ml_dsa_87:: | include/ml_dsa/ml_dsa_87.hpp

---

✨

All the functions, in this ML-DSA header-only library, are implemented as `constexpr` functions. Hence you should be able to evaluate ML-DSA key generation, signing and verification at compile-time itself, given that all inputs are known at compile-time, of course.

I present you with following demonstration program, which generates a ML-DSA-44 keypair, signs a message, producing a ML-DSA-44 signature and finally verifies the signature - all at program compile-time. Notice, the static assertion.

```c++
// compile_time_ml_dsa_44.cpp
//
// Compile and run this program with
// $ g++ -std=c++20 -Wall -Wextra -pedantic -fconstexpr-ops-limit=125000000 -I include -I sha3/include compile_time_ml_dsa_44.cpp && ./a.out
// or
// $ clang++ -std=c++20 -Wall -Wextra -pedantic -fconstexpr-steps=19000000 -I include -I sha3/include compile_time_ml_dsa_44.cpp && ./a.out

#include "ml_dsa/ml_dsa_44.hpp"

// Compile-time
//
// - Generation of a new keypair, given seed
// - Signing of a known message
// - Verification of signature
//
// for ML-DSA-44, using KAT no. (1). See kats/ml_dsa_44.kat.
constexpr auto
ml_dsa_44_keygen_sign_verify() -> auto
{
  // 7c9935a0b07694aa0c6d10e4db6b1add2fd81a25ccb148032dcd739936737f2d
  constexpr std::array<uint8_t, ml_dsa_44::KeygenSeedByteLen> ξ = { 124, 153, 53, 160, 176, 118, 148, 170, 12, 109, 16,  228, 219, 107, 26,  221, 47,  216, 26, 37,  204, 177, 72,  3,   45, 205, 115, 153, 54,  115, 127, 45 };
  // 0000000000000000000000000000000000000000000000000000000000000000
  constexpr std::array<uint8_t, ml_dsa_44::SigningSeedByteLen> rnd{};
  // d81c4d8d734fcbfbeade3d3f8a039faa2a2c9957e835ad55b22e75bf57bb556ac8
  constexpr std::array<uint8_t, 33> msg = { 216, 28,  77, 141, 115, 79,  203, 251, 234, 222, 61,  63, 138, 3,  159, 170, 42, 44,  153, 87, 232, 53,  173, 85,  178, 46,  117, 191, 87, 187, 85, 106, 200 };

  std::array<uint8_t, ml_dsa_44::PubKeyByteLen> pkey{};
  std::array<uint8_t, ml_dsa_44::SecKeyByteLen> skey{};
  std::array<uint8_t, ml_dsa_44::SigByteLen> sig{};

  ml_dsa_44::keygen(ξ, pkey, skey);
  ml_dsa_44::sign(rnd, skey, msg, sig);
  return ml_dsa_44::verify(pkey, msg, sig);
}

int
main()
{
  // Notice static_assert, yay !
  static_assert(ml_dsa_44_keygen_sign_verify() == true, "Must be able to generate a new keypair, sign a message and verify the signature at program compile-time !");
  return 0;
}
```

---

See example [program](./examples/ml_dsa_44.cpp), which demonstrates how to use ML-DSA-44 API, similarly you can use ML-DSA-{65, 87} API.

```bash
$ g++ -std=c++20 -Wall -Wextra -pedantic -O3 -march=native -I ./include -I ./sha3/include examples/ml_dsa_44.cpp && ./a.out
ML-DSA-44 @ NIST security level 2
Seed      : afc6c351c70775e04b4ece579e72400afbb31fe8bad3d1d8ed0ba40526b0d528
Pubkey    : 0405c237bfd3436898f31ffcd6e3510ca38b6dbe1772943a59ebabb83e7f3400ee064e5f4c63fc7347c4598011a638911e3a0815dc7f11b663006fd8a5871e1ec46d7f6230996c64eb9ae192aba4c5b3fc6cf8cf9b8e2af33ad46f39775c00e2437a5bc6a9f92839c8d09b755f003e5b59f798844965aff3ed2198282d933976a0bb65c62b3f7774f082037c7a23cd632c95765783ee7e95c3a750ab99a0f22aab658e434d60a1d17dd9ab9d37ea1c0b36b856abcf5028551799725bcc39226863f3971c5a9b98f3999c6763b221222e19deb63414e9fc56dad230fbaef157df3f23f9f229aa2bc22b2b445bf464bd5aee24d6b4a8ba1e916fc1e40ba61b405212584246f0888724b4f8f79449d9cb4757a91fd35775d7e08715cfce703fbe7d69cf260e9617601fff3945101934e9a979c47ebd88de30c7c434dfc9e955136063ab01654548bc9767faaaa7dd6bf6d79264217fc8187cc884cf96844d28c63e4334c35b1aa4fa80b37b1ea8a6aae40006112eaed4bfab952adc2621a00b090072108119985bb14485ce9a8193829f8d739081c632e665e9074a43c2f7ebc2207aae38d1b2ab101a09dc4df322c23035fc67258a67d644b12d1674af3ac151f64618f533ea85aedc3308e3d439003ee908e960f846852d61722f11cc24b8dce16a9a857a2fd407d90196558e01c3d32c65aea09acceab6f42a971f44608d67a432616793c57c7cdf4f8a2db115fa0ea14538399246755a3347bb54285b5a11b7f3672949b998f7ca54e7afae23b54babca52302b4318e7fb8c4daa92c9a74dac9c26738e47f4e094075b7a251e375038d9aa7ef821930feac261c83cdd7cde3c5aaeb21a9a17c1812983db7c0c1dba3d7b8a4790d4016b4f1688a75e6273cfbc13c73f5bb6f2ed1e368294ae1cd819877f9a2f91e51d459283a6bceb09ac2c3ba48a53da892082091a896c272aea9036d5045fc30f91384e613f427efd3bebbda5e4fe54b53ec123e048f5739f88bba2507295f476e0ef0208a5ee89845a7be33f269329aa74366ccb89473900367de9a63b3ccd6a88e5e3834b453bb5edf33301acdc2ac1e9884b5dfbf4ca39546062568bb6e58c77fbccf12294a518e624d4d1c2f6d806e4e27582c7756ba6301504af4e755266141f329dd67b4f89155dae840ccbe7cdae96830ff64c24eee30bee5a0ff55c4c8453fe63a75f7a6eb4588def226866c93f656db4c74d25fab26fe1dd07fb0dc1567f1887e63e90a00046f7cc31adf13b4454bae760709cb730e1ac774a7e7642651b4e631e2c10bec32d966fd9671030259c151cd39773381fb10ed25f4397bfc55258d55561f37e1901239cd05016a30fb95953e5644819ae3fc8c2c5df4fb33f6621fb5a8ac2a74b1cc57226b65fb35d3af3030233af845c2b3f78a20056d497352931777cb90350a6de9a4719585b8b5732526621cb77d9b41f3f54d05ee4df1e176c1bc0da781ccf4fa49bd5ec231834cb808b7dd97be8994c38ca1bbf176b9ed159d82d660ebc28511dd63bd4c38ff33cbaf8b15976fdd17f9e94dabeaa6336c2661677405764d25c7fc62f3e367426e286520e2d5be867cd069fcc6d9dd746fd18b76b4c47ab71b7f8218d2f2dfea0d23ef5cbdd3b5503f94f4828ab78a33d180bfcd24600ab51a4a6022df89a781865e3d597adf4a20e9dc5f02ff34ead0aa8baeee79e54c1ac10fc5c0852e82e41860c7b48af16120c977be7522bc9ad66563f74e76956ce6c5031eae8d5ff55964788332ac6971466bc426b82293f81227be20bfcaacd7700fd729c0c25dbeda4097da84b2edd565a49fcb2e8503a90d6fd13a6e17cd4beb322
Seckey    : 0405c237bfd3436898f31ffcd6e3510ca38b6dbe1772943a59ebabb83e7f340094a5e972e060c891e14abc20bb2c74179a645564e462ef870305b86c9e0a0c0df4c1bef4a9b97b23bbc37e1c11c4e5b7d8c65bacf9ebbcca4ea7b7b09d5261f7399f9ef7160253f6badad2e4ef98bd0e89f980f725438cddecff945e366f6903112270012049db30620b146a41966110160e02b490089281a234881c3786ca983001370180128211982193360a41248520b2841aa16d61165024870d0c3191c48028192630d82672da221140920888288e03a3515c440e62928d1206495920604c8225100152a20208181211109505da046422231221849019964549046c1a266a18c160143524c0260e1bc281db1822030545044124a44251a18091190804c8044edb222edb36810b010a1a92709344129822651b03208210289422814298405a822d83102554966849b69100432d61047052064ea18650a1888cca344cd446210c816d1813910b9145912068e104202391440c8250244705c4428ccb345254860400144983846809a0655a0605481232000086d13020a31002a0228203a2110c0541149105c3c46ccc8650c9165141c064e30446184509083708e4281002999182b0491a9471dac68c14400520918dc0b07121371100854c111386e2166a23c40562c8201ab110194032c832220a976d9bc4301336306226520aa65004242e93266ce2460a8834288b2204913660d808460021018b9648d4269220b36013c34559068103c6019aa488c8960c0cc63150381161146603b7681a22080bc63124822de0202c83c03082c61100c31009008419b5840a49661b032183c251d1b61003152908940984826d9ca44004496ccaa220422228543886cb92710ac180992092d10062832232e4948d94946124122599466c88206652944d43265214998d0a832918206d11094911371290a6844194898a428c60124a60980404479220c8408b000c1aa1048c8444583849d030040a846c98306ec9b25189042e89c6208ab0211a25045244311b086a19229284023181822d1840518ca450db0646dc304459a204d21689d30061a2484ec8906122468104216d248048c344729bc06498048c522425c11200c436321bb06401170604390512a804d1266058226809a5888932300118021183495132490c09291a270420868001022002a040223241dc06221aa74d8cc8451ca9250a360ec09605d9a850d9462e6388710ba365149381022288eee511767dcd290bf6ee1519b9307c1deaf3331a49cd9f6a1335c0ed950974a87c2479bfe27dd681bc72836535914d75d0a715a6ed8fd58a5cae45b89550e486c6ab80e73e687f5c865a72c35f63d3a7116f417258645268ed26723127d2724fe5ff65db9a41ff1a13cfc4f6b182e932a0ef3f2cb5375bfd9bb0c602ace85ab7b85bec9f3368195cbc11b105eacd4f0c1da51653bb1b0cfbd4ce9c4670524bc735c02686c8fd2190c6cd0de7dda55cb90193d983ad33b589caa23ef051a5d3aaf23661e045f9e143ac3a2abef34aeac67d8ffb86d37db855b4477f9dae2b221c4e5f4bf8f591cf3f42e3a06acff9b70d06c90f7a3710e453f68c4afa12701255053a9658d9c6dfaf22e3943b33299c4708d8f24283cfd2ded96f5bbf6b7c0e9e1a8b7bea9273db4d8dffcca213e447e59a1f66bbe57741161bcd558f902c31715664608b67ef5aa44ff0cbbd4e6b2afbafb437719d5e5d3b04281ee8ed32fc6c00a502190007e85e843bca59ca0cd21ee269bce56cf15793c61d30f89840b3c39c6a5b5385773f76d24410cc6f080e766e841d0bc458103bbe4a89fedb3a755489032ac63441217984777476fbb5766cd4f9abbc46a0b0e899a968a7d20492c434c69b98ac3f888af7a186c29ee08b66659e9fbab15814d277e1b4b60c1ab83650646128a894552598e129c0d6bd3cd09636460144200d8c2f28331e17665c32fe20870eab70f3e439af45831b09a7dba48d0ec044836e29b830c4243e490a3a1f0a6e027123b8132d7653c24f3c7e7cea4ef624ca932324be4665fe441550108ffc025ebfbb9e097ab3a40f7a50f70d9c84d4c096d8e9e5906969a847db65f1ca313731f311a75ee84a7c33319c2444cc6f2b9a6907a2778d0dbd0b6c029042b2f3b179dc0c0bbb15413e68c3a5432eae76edc0fab8c0aac7ebf0cb9a8affe8ddc698729344642573b83934ecd46202bbe5bafba7cf69322fd4789f9db7ffd6a8a26e011ba1a3e13d36962f9f99661140c0dbee5d493686a57de3c4911094224d51e6f4cc1ff6e5de7817f8a7a2f0bd72d048f4f394868dc5aee4afbec2c5e90dcbeaff20d86edf496fc945818a4ce0e435f6d44c20d8b0d62ccfd611f184dcd4041ed74625a15b318850b82934ff5f2c8f76eb0568ae2885bef1660ef7385c70666fcce0c3f153da8e031ab05f3af2828c2d62a465cc2d02a0771a68898c2df7c3ccec26305694a665f0384ac42fa48f4f5e4bc473aaccb80f5da9e1e868bc0284c91f7fe8e1f0195066f05cdb1dac6d56a10c2323069f0765005c4d68e96e0b06ad8955f7f3a7d3ccf5f537a6aea40f7c9d7f4a9d17a0b7022ad79322a3b4aee04e8a7ea8bc70dbba245c4e1c1a74248c36df727c7349f09a392daaacd2190fd743cdd3fde0d6ddb6766734ac73c13434441f65713764458360fb905ac9ddbc0a151cde4938ad24ffb3c4fec13152e6937014c121001b4531d92898bc10d2881815080c50f120e1ca69033b302df5badbe5c7d8a7300cf8af1a1505d4d8ffde676eaf2125652e84407da961bfbf11382bde35f3bbd2a74d1b52424b95c76fbb71054263915812876583ebe748786445f4a7a88ea544e29216621daf16a2de80fc9c99b9b1128ae8b6a74914ffafd25ac6c2d33bf1d2e856e10c3c47741d2646ec5b1947d9371fa2fd824fe3593142bb6561d35a26b7ec23b28f922bbe73999ce80abff40847bdb8efe0e281b2aa2f15c9ad3712f243cc8dd6c157e4669d7f11d827e3c6920b91a5d27dd7137ec78aa54c6c7ff101064a3e86dd3ae1fcf8900c39cb8d13deb9345bb070752c08170bc9153037f21e4b5a9239ea1efb484745d42a8136e5fb52a3a9bebf1873af39854f0e377e7cb178183a3ed9d9a2f2a99056b0a05c03cd95c49fdad85f981b15376f01237e25ed77f5d66ed5da2c5d76814b9c16b2f177612241509cb71deb5d499335c15e8ba7472d648b1bfc316f77ad7e51cfed3e48d95395bebcf7cfdae63c22793b86955708f39dc6d219d11dfda22d799bbb577e6eb33ce827f99dbdf2759995a1534e4ca186ca4048c401dd531f360fb2f3b3ba114fd67a7f30a7f31e7596bcf104327165572f98decf5df3e3ab62dc7b6ee08aafe8eefc710ec5fb11ff640dacb6cd19a112b1551c26f6c04b976f1020b4dce029489aa47ec5487eb5d4912902ece656f9ddb313557560c2ca016740b8dfa651ebd26233b9150c9ceb8706fa404a61b80ae2a1b89741c5b133ef9424a828402c8a80340f91c686c312f268ee455745f2096223a6ad4304e3c2945b1d4ce2897c58655baf0afb9333c1f723df36c583b69a1aa2d491095ef2794d
Message   : 8a556fe4a5e29a37e80f2ad8f3f8679f1cd3f22b1532bd171373f76aa1402158
Signature : 21ac8a09005d0101947b64865daf8559c175722647248ad3d8897d78ba0ea81e49d1130e9f830e9fbed5b1f4dd37517d5571bd70ba47494ab05fe4ec1a67e7343c929ea956638ca501f1b6ef21887b094ece20782410349b2dca72173ef1ba6e8384bdf83d97c2cfece5b3e083eb9165052ca04c1f921867999465d8bcee76df4bc8db038f90b6be7219cf1225e40ccb3e4bbfce01c823f3631a03dc04324053591b9a9460799a882eeb4aec3338dd7d7fe25b047350f94bb547ce9d33a60d41c4200af881a24be6759c2e0e9c204b95460c0597190b571220ff3db6881ea82b91766936dbf2016985a15f0b502e690cbfbd5c1d4a1634146347ac556657de1d54fb2f2c40b6cee3ceaf9118670afa14b707a4dea5c0a7cb2da67ffb29d32a045cbad61b5f96ac5715172bdba6f99b2452260205c59e90bb012577b3b5a4e5aa90e5f5edeef93c70e4ad8ca2552dbeb78d2aca528da4083f52bb57eb11e1e6c1d25c4307a949a7d1172e6a7d496a3c419627b7b26797cc07778cf5db5807dfd68c8e4893323fc0d39f9a25997493cb088e74ec8539f54b1eb988329c2c23310d671bb20c7f2bbadd7bc315f322b1d863674a796b519a52433545e3d428b83743d5edde7882f0048a2133d270bd836b62acd7f6715c8875af4de8d3aff0100bf386d241f6007ca363ade61052415951ab93f2a078ea8510dc951de2c22c604f944838c08122bb52eee5b3590518811b5a718ed54e45eac2b937a66e96e678295e648525187c07ec3a7580a2c30bd35fb95b497032876e3be1b98c8044f65873091b8de16b0ec312e853d9347e669b3ed53083e1f0dd69cbbd6d8d0dc81510df83cac2202435c17481f86aceef80ceae04859b0b1a4c63052202ccfdc2df0698297066c0cb7b32b446830a7fcb040b1563b43b72e6b69a8245d5f5a3b37d89fead3783ac7a64bedd6d2e24811489d134a68ecb55856698643aebb3fcfc8ccb3bb4b2d4f7803146baf8f11a644adce99a4ebc700993bf1425372538fb23edce62934dac6676c5ef97d3f9b05ef7a7cbd4fd54faacf368cacdebd01a8e0db394a42a973d3856c5a93d84b1a8287ae42d9a67a48340b4261c5ca7a72bb50f7a5edbaaaeeb8fb4bcd3edf5cb57c9bba1b8a46c179a880f62a8e4b7438ffbeb0c9520e3bda2d889d9ce575ae44c6f54653817fe6e5497c96dcf422e8296833af9358980592a905a26a1242d143be7c2b0a0fab5306942316dab224c93b451b42e0681fe589811fac5c27461a9b001682f3b737fefc2dabd9985f44ef099d39c09bf9df742cd87d64cc724a12fb38b4849093182270c34f9e863a8d63cccb724c8dba45172528ba308764e8ab15fd295f3656c5fb7f989c1df829e3356233bb81d90f57bd707688922c1adfcc0e157a5b23de37f4b46c1972482abc7380e45c6332d66b33518db901ed9a158f69bf650d1fb87021f4756934b5af82e1e8f60e6dff7e6c60fd4529160bfcacfde14552741d15458c1985491b2fc5d58a95b322083ef0ef9c09d99ee6227a4d58372650d5771fe9465000eab53d1a304a3711d0e9c695c0044450a318e7b0b30ffdde0fc551bd1aa7a757ae3326b7241575afb4ccc1c9fe357766474c02ce3a819e898ea883b62c657d5adea98e1033c85212d628059e55f512e2f70d3d41705b66420d98e68bca4ed4835798bb69eb36db77a6cf41bb2cd6ac2775532c309375230010e8b51a418d1fddcbd7e832a3c63096ff49a72dc201df38f8a777a1c7020d281139cff300726c4022dc8cf391eb9e7470353a636e7473ac3d69bc7b369bb5f0f427588eb7f1b104b51de3e2491b34c96c11c988b13c9c1821a4b552135e91dfacd4bbce7470be044a39b4fb712820312ead2e37943e61df1520a3c949998881ea8ce8bd9bfa7712bf269428a9cdaf31344d6117f8a2bb9cffd7bc65422bedc15c9df31c3565ab14fd7a2ede211db28036a675768192bdefd3e5daca8f9e1dc6879544e8e2c5c5364e8730c9e9d927b42a6f4e4b338db0979832cb57bb6d2df262d1303a90be408cc4cc599e00ff232fc56011199c80fae882e2215e3d56bc4870dd38c456884be9851d74bf590e30296ec63ec2d1b5035e925576da75e651734ecb8d833a040548c8cad1da19c45e8af09f9e317974a6130e65fdb53b47e21249f8f1917be9343745d9cdf6372239e82977750049066653352c54776d6cb76f116a7e813e07adbb9a1b22359e2153304fbf663b66e04fb839b4cec729ad3eb256ee7c4d590c53259e220a27559660437ff85575195174b2cfb73cd136c4b3b6d7c1f9344248e270f1484a08db82715857af9f7c3d5a0545ec0a9f9b8ee819450c5f6861814b8191bd590b1bcf366b173c84eacab317df4f5df6968757f51e3853993bdc41232117dc47c4b2a7eeb75016af941158ad1199aeed114af1b60cc10203f8bea42ae522e42ee0549ba1b114efb712eb785b482c639707ef3c1ca0513e00c8731fdbe19e54461ce99ea798ae232175108ef0a16fd3d5427a0f2d83fe5c99ca1299b3f356d9acf546b2a4e065baa2e3deab7868d1049e5f656306accaea3154afb11ef0ebac1bba37e164592b8634cf8dd8b9f2bb21153bb60c2638c4cc691cc8d16a7359a7b69bbdf7127c067da1c0ff4127ac368d86c3f8939740597145e121df57adbf7efeec9b91ce1a3c4ef89fbf8b050f9b5e994fff2b2eb5cfb6dee6b3bc45b567d36f3114ec2415870d8f09d83f48c55584d2279c083affd4dd3011fa0aed0d2759a8c69a6212ea32704a4b0f4eacf158ea3fb5d7d8d6c63d01efa964370a1f65082c3944b4524a3fb69ff5698fe0c9d9b7813f619d4802bb8713808a715639bfad50a7c359228b5ea02f0e776183adc93f9309576f8f215a58b68198c3b2af16bf095fd13f1204b34699addfe0882b52227f3e44f8a74519076e4c968b489f299964bc1d6dba1408f7e78635184ef614ddeaa44f02575c58f22990f48b3146b7f1c130323a66228ec363894b41ffbd81dd8fbf95171a513dfc014e017a31d9e5767fccf7cd2c1b14cd39a2e6feed4311579809dea3efccc374600745e9fd60dbf75e4b195f8d6342276cb7900bea531d0d71ff1ff9c307898614b22c355d9c362e66d4cdcf10f2fcd0c25b8f642e8dd50aa6c30eb40c6944a869f705f0e61bb3fa9d28323889794a45870d3aa848e889b7fbe010527c786b87f9686f9b29402612bdcfd4f10f53161bbd5cd601f46112e5f92907636bb2aeafc647baec193d958e6ed16a1741d97d9737f13cc9102101b225758829495a3aab6c3d4d6d8dadde2f3171823494b737c808390a5b3bfcfd3ebfe071f4a4c4f6673c0f0f8fe1d444a6e757ea6a9d4e4000000000000000000000000000000000000000000001425303a
Verified   : true
```

> [!CAUTION]
> Before you consider using Psuedo Random Number Generator which comes with this library implementation, I strongly advice you to go through [include/ml_dsa/internals/rng/prng.hpp](./include/ml_dsa/internals/rng/prng.hpp).

> [!NOTE]
> Looking at the API documentation, in header files i.e. `include/ml_dsa/ml_dsa_{44,65,87}.hpp`, can give you a good idea of how to use ML-DSA API. Note, this library doesn't expose any raw pointer -based interface, rather *almost* everything is wrapped under statically defined `std::span` - which one can easily create from `std::{array, vector}`. I opt for using statically defined `std::span` -based function interfaces, because we always know, at compile-time, how many bytes the seeds/ keys/ signatures are, for various different ML-DSA instantiations. *This gives much better type safety and compile-time error reporting.*
