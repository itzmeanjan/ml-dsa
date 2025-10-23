# ML-DSA (formerly known as Dilithium)

NIST FIPS 204 (ML-DSA) standard compliant, C++20, fully `constexpr`, header-only library

> [!CAUTION]
> This ML-DSA implementation is conformant with ML-DSA standard @ <https://doi.org/10.6028/NIST.FIPS.204>. I also try to make it timing leakage free, but be informed that this implementation is not yet audited. **If you consider using it in production, be careful !**

## Motivation

ML-DSA has been standardized by NIST as post-quantum secure digital signature algorithm (DSA), which can be used for verifying the authenticity of digital messages, giving recipient party confidence that the message indeed came from the known sender. ML-DSA's security is based on hardness of finding short vectors in lattice i.e. it's a lattice-based Post Quantum Cryptographic (PQC) construction.

ML-DSA offers following three algorithms.

Algorithm | Input | Output | What does it do ?
--- | --- | --- | ---
KeyGen | 32 -bytes seed | Public and Secret Key | 32 -bytes seed is used for *deterministically* deriving a ML-DSA keypair.
Sign | 32 -bytes optional seed, N (>=0) -bytes message and an optional context string (of max 255 -bytes) | Signature | For default and recommended **hedged** (i.e. randomized) message signing, one must provide with 32 -bytes random seed. For **deterministic** message signing, one should simply fill seed with 32 zero bytes.
Verify | Public Key, N(>=) bytes message and an optional context (of max 255 -bytes) | Boolean value | Truth value denotes successful signature verification.

Here I'm maintaining `ml-dsa` - a C++20, header-only, fully `constexpr` library, implementing NIST FIPS 204 ML-DSA standard, supporting ML-DSA-{44, 65, 87} parameter sets, as defined in table 1 of ML-DSA standard. For more details on using this library, see [below](#usage). It shows following performance characteristics on desktop and server grade CPUs.

ML-DSA-65 Algorithm | `(a)` Time taken on "12th Gen Intel(R) Core(TM) i7-1260P" | `(b)` Time taken on "AWS EC2 Instance c8g.large" | Ratio `(a / b)`
--- | --: | --: | --:
keygen | 92.9 us | 126.2 us | 0.74
sign | 604 us | 879 us | 0.69
verify | 94.8 us | 134.4 us | 0.71

> [!NOTE]
> All numbers in the table above represent the median time required to execute a specific algorithm, except for signing. In the case of signing, the number represents the minimum time required to sign a 32 -bytes message. To understand why this is done for signing, please refer to [this](#benchmarking) section.

> [!NOTE]
> Find ML-DSA standard @ <https://doi.org/10.6028/NIST.FIPS.204>, which you should refer to when understanding intricate details of this implementation.

> [!CAUTION]
> This implementation of ML-DSA doesn't yet implement "prehash" version i.e. where hashing of message can be outsourced to another cryptographic module and hash of the mesasge is signed instead of signing the original message.

## Prerequisites

- A C++ compiler, which can compile C++20 programs.

```bash
# I was using Clang-19 when developing this library.
$ clang++ --version
Ubuntu clang version 19.1.7 (3ubuntu1)
Target: x86_64-pc-linux-gnu
Thread model: posix
InstalledDir: /usr/lib/llvm-19/bin
```

- Build utilities such as `make`, `cmake`.
- For testing correctness and compatibility of this ML-DSA implementation, you need to globally install `google-test` library and headers. Follow guide @ <https://github.com/google/googletest/tree/main/googletest#standalone-cmake-project>, if you don't have it installed.
- For benchmarking ML-DSA algorithms, you must have `google-benchmark` header and library globally installed. I found guide @ <https://github.com/google/benchmark#installation> helpful.

> [!NOTE]
> If you are on a machine running GNU/Linux kernel and you want to obtain CPU cycle count for ML-DSA routines, you should consider building `google-benchmark` library with `libPFM` support, following <https://gist.github.com/itzmeanjan/05dc3e946f635d00c5e0b21aae6203a7>, a step-by-step guide. Find more about `libPFM` @ <https://perfmon2.sourceforge.net>.

> [!TIP]
> Git submodule based dependencies will normally be imported automatically, but in case that doesn't work, you can manually initialize and update them by issuing `$ git submodule update --init --recursive` from inside the root of this repository.

## Testing

For ensuring functional correctness and conformance with ML-DSA standard (FIPS 204), this library
comes with property-based tests and known answer tests (KATs). KATs are taken from following two sources.

- ML-DSA reference implementation by the proposing team. More @ <https://gist.github.com/itzmeanjan/d14afc3866b82119221682f0f3c9822d>.
- NIST ACVP Server @ <https://github.com/usnistgov/ACVP-Server>. ACVP KATs can be synchronized by running `$ make sync_acvp_kats`.

```bash
make test -j               # Run tests without any sort of sanitizers, with default C++ compiler.
CXX=clang++ make test -j   # Switch to non-default compiler, by setting variable `CXX`.

make debug_asan_test -j    # Run tests with AddressSanitizer enabled, with `-O1`.
make release_asan_test -j  # Run tests with AddressSanitizer enabled, with `-O3 -march=native`.
make debug_ubsan_test -j   # Run tests with UndefinedBehaviourSanitizer enabled, with `-O1`.
make release_ubsan_test -j # Run tests with UndefinedBehaviourSanitizer enabled, with `-O3 -march=native`.
```

```bash
PASSED TESTS (37/37):
       1 ms: build/test/test.out ML_DSA.HintBitPolynomialEncodingDecodingFor_ML_DSA_87
       1 ms: build/test/test.out ML_DSA.PolynomialEncodingDecodingWithSignificantBitWidth6
       1 ms: build/test/test.out ML_DSA.PolynomialEncodingDecodingWithSignificantBitWidth13
       1 ms: build/test/test.out ML_DSA.HintBitPolynomialEncodingDecodingFor_ML_DSA_44
       1 ms: build/test/test.out ML_DSA.HintBitPolynomialEncodingDecodingFor_ML_DSA_65
       1 ms: build/test/test.out ML_DSA.SampleInBallFor_ML_DSA_44
       1 ms: build/test/test.out ML_DSA.SampleInBallFor_ML_DSA_65
       1 ms: build/test/test.out ML_DSA.SampleInBallFor_ML_DSA_87
       2 ms: build/test/test.out ML_DSA.PolynomialEncodingDecodingWithSignificantBitWidth4
       2 ms: build/test/test.out ML_DSA.PolynomialEncodingDecodingWithSignificantBitWidth20
       2 ms: build/test/test.out ML_DSA.PolynomialEncodingDecodingWithSignificantBitWidth18
       3 ms: build/test/test.out ML_DSA.PolynomialEncodingDecodingWithSignificantBitWidth3
       3 ms: build/test/test.out ML_DSA.PolynomialEncodingDecodingWithSignificantBitWidth10
       4 ms: build/test/test.out ML_DSA.ML_DSA_44_Verify_Internal_ACVP_KnownAnswerTests
       4 ms: build/test/test.out ML_DSA.ML_DSA_44_Verify_ACVP_KnownAnswerTests
       5 ms: build/test/test.out ML_DSA.Power2Round
       6 ms: build/test/test.out ML_DSA.ML_DSA_65_Verify_Internal_ACVP_KnownAnswerTests
       7 ms: build/test/test.out ML_DSA.ML_DSA_44_Keygen_ACVP_KnownAnswerTests
       8 ms: build/test/test.out ML_DSA.ML_DSA_87_Verify_Internal_ACVP_KnownAnswerTests
       8 ms: build/test/test.out ML_DSA.ML_DSA_65_Verify_ACVP_KnownAnswerTests
      10 ms: build/test/test.out ML_DSA.ML_DSA_87_Verify_ACVP_KnownAnswerTests
      13 ms: build/test/test.out ML_DSA.ML_DSA_87_Keygen_ACVP_KnownAnswerTests
      14 ms: build/test/test.out ML_DSA.ML_DSA_65_Keygen_ACVP_KnownAnswerTests
      16 ms: build/test/test.out ML_DSA.MakingAndUsingOfHintBits
      27 ms: build/test/test.out ML_DSA.ML_DSA_44_Sign_Internal_ACVP_KnownAnswerTests
      31 ms: build/test/test.out ML_DSA.ML_DSA_44_Sign_ACVP_KnownAnswerTests
      41 ms: build/test/test.out ML_DSA.ML_DSA_65_Sign_Internal_ACVP_KnownAnswerTests
      43 ms: build/test/test.out ML_DSA.ML_DSA_87_Sign_Internal_ACVP_KnownAnswerTests
      47 ms: build/test/test.out ML_DSA.ML_DSA_87_Sign_ACVP_KnownAnswerTests
      52 ms: build/test/test.out ML_DSA.ML_DSA_65_Sign_ACVP_KnownAnswerTests
      80 ms: build/test/test.out ML_DSA.ML_DSA_44_KnownAnswerTests
     151 ms: build/test/test.out ML_DSA.ML_DSA_87_KnownAnswerTests
     151 ms: build/test/test.out ML_DSA.ML_DSA_65_KnownAnswerTests
     322 ms: build/test/test.out ML_DSA.ArithmeticOverZq
    6726 ms: build/test/test.out ML_DSA.ML_DSA_44_KeygenSignVerifyFlow
   10662 ms: build/test/test.out ML_DSA.ML_DSA_65_KeygenSignVerifyFlow
   15431 ms: build/test/test.out ML_DSA.ML_DSA_87_KeygenSignVerifyFlow
```

> [!NOTE]
> There is a help menu, which introduces you to all available commands; just run `make` from the root directory of this project.

## Benchmarking

> [!WARNING]
> Relying only on average timing measurement for understanding performance characteristics of ML-DSA `sign` algorithm may not be a good idea, given that it's a post-quantum digital signature scheme of **"Fiat-Shamir with Aborts"** paradigm - simply put, during signing procedure it may need to abort and restart again, multiple times, based on what message is being signed or what random seed is being used for default **hedged** signing. So it's a better idea to also compute other statistics such as minimum, maximum and median when timing execution of `sign` procedure. In following benchmark results, you'll see such statistics demonstrating broader performance characteristics of ML-DSA `sign` procedure for various parameter sets.

Benchmarking key generation, signing and verification algorithms for various instantiations of ML-DSA can be done, by running following command.

```bash
make benchmark -j  # If you haven't built google-benchmark library with libPFM support.
make perf -j       # If you have built google-benchmark library with libPFM support.
```

> [!CAUTION]
> Ensure you've put all CPU cores on **performance** mode, before running benchmarks, follow guide @ <https://github.com/google/benchmark/blob/main/docs/reducing_variance.md>.

### On 12th Gen Intel(R) Core(TM) i7-1260P

Benchmark result in JSON format @ [bench_result_at_commit_25f3161_on_Linux_6.17.0-5-generic_x86_64_with_g++_15.json](./bench_result_at_commit_25f3161_on_Linux_6.17.0-5-generic_x86_64_with_g++_15.json).

### On AWS EC2 Instance `c8g.large` i.e. AWS Graviton4

Benchmark result in JSON format @ [bench_result_at_commit_25f3161_on_Linux_6.14.0-1011-aws_aarch64_with_g++_13.json](./bench_result_at_commit_25f3161_on_Linux_6.14.0-1011-aws_aarch64_with_g++_13.json).

More about this EC2 instance @ <https://aws.amazon.com/ec2/instance-types/c8g>.

## Usage

`ml-dsa` is a zero-dependency, header-only, C++20, fully `constexpr` library, mainly targeting 64 -bit mobile/ desktop/ server platforms. It is also very easy to use. Let's see how to get started with it.

- Clone `ml-dsa` repository.

```bash
cd

# Single step cloning and importing of submodules
git clone https://github.com/itzmeanjan/ml-dsa.git --recurse-submodules
# Or clone and then run tests, which will automatically bring in dependencies
git clone https://github.com/itzmeanjan/ml-dsa.git && pushd ml-dsa && make test -j && popd
```

- Write a program which makes use of ML-DSA-{44, 65, 87} key generation, signing and verification API ( all of these functions and constants, representing how many bytes of memory to allocate for holding seeds, public/ secret key and signature, live under `ml_dsa_{44,65,87}::` namespace ), while importing proper header files.

```c++
// main.cpp

// In case interested in using ML-DSA-65 or ML-DSA-87 API, import "ml_dsa_65.hpp" or "ml_dsa_87.hpp"
// and use keygen/ sign/ verify functions living either under `ml_dsa_65::` or `ml_dsa_87::` namespace.
#include "ml_dsa/ml_dsa_44.hpp"
#include "randomshake/randomshake.hpp"

int main() {
    // --- --- --- Key Generation --- --- ---

    std::array<uint8_t, ml_dsa_44::KeygenSeedByteLen> seed{};
    std::array<uint8_t, ml_dsa_44::PubKeyByteLen> pubkey{};
    std::array<uint8_t, ml_dsa_44::SecKeyByteLen> seckey{};

    randomshake::randomshake_t<128> csprng;
    csprng.generate(seed);

    ml_dsa_44::keygen(seed, pubkey, seckey);

    // --- --- --- Message Signing --- --- ---

    std::array<uint8_t, ml_dsa_44::SigningSeedByteLen> rnd{};
    std::array<uint8_t, ml_dsa_44::SigByteLen> sig{};

    // 32 -bytes randomness, for default and recommended *hedged* message signing.
    csprng.generate(rnd);
    // For deterministic message signing, uncomment following statement and comment above statement.
    // std::fill(rnd.begin(), rnd.end(), 0);

    constexpr size_t msg_byte_len = 32; // message byte length can be >= 0
    std::array<uint8_t, msg_byte_len> msg{};

    // Sample a psuedo-random message, to be signed.
    csprng.generate(msg);

    // Note, we're passing an empty context.
    const bool has_signed = ml_dsa_44::sign(rnd, seckey, msg, {}, sig);
    assert(has_signed);

    // --- --- --- Signature Verification --- --- ---

    const bool is_valid = ml_dsa_44::verify(pubkey, msg, {}, sig);
    assert(is_valid);

    return 0;
}
```

- Finally compile your program, while letting your compiler know where it can find `ml-dsa` and its dependency headers.

```bash
# Assuming `ml-dsa` was cloned just under $HOME

ML_DSA_HEADERS=~/ml-dsa/include
SHA3_HEADERS=~/ml-dsa/sha3/include
RANDOMSHAKE_HEADERS=~/ml-dsa/RandomShake/include

g++ -std=c++20 -Wall -Wextra -pedantic -O3 -march=native -I $ML_DSA_HEADERS -I $SHA3_HEADERS -I $RANDOMSHAKE_HEADERS main.cpp
```

ML-DSA Variant | Namespace | Header
--- | --- | ---
ML-DSA-44 Routines | ml_dsa_44:: | include/ml_dsa/ml_dsa_44.hpp
ML-DSA-65 Routines | ml_dsa_65:: | include/ml_dsa/ml_dsa_65.hpp
ML-DSA-87 Routines | ml_dsa_87:: | include/ml_dsa/ml_dsa_87.hpp

---

✨

All the functions, in this ML-DSA header-only library, are implemented as `constexpr` functions. Hence you should be able to evaluate ML-DSA key generation, signing and verification at compile-time itself, given that all inputs are known at compile-time.

I present you with following demonstration program, which generates a ML-DSA-44 keypair, signs a message, producing a ML-DSA-44 signature and finally verifies the signature - all at program compile-time. Notice, the static assertion.

```c++
/**
 * Filename: compile_time_ml_dsa_44.cpp
 *
 * Compile and run this program with
 * $ g++ -std=c++20 -Wall -Wextra -pedantic -fconstexpr-ops-limit=195000000 -I include -I sha3/include -I RandomShake/include compile_time_ml_dsa_44.cpp && ./a.out
 * or
 * $ clang++ -std=c++20 -Wall -Wextra -pedantic -fconstexpr-steps=30000000 -I include -I sha3/include -I RandomShake/include compile_time_ml_dsa_44.cpp && ./a.out
 */

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
  ml_dsa_44::sign(rnd, skey, msg, {}, sig);
  return ml_dsa_44::verify(pkey, msg, {}, sig);
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

See example [ml_dsa_44.cpp](./examples/ml_dsa_44.cpp), which demonstrates how to use ML-DSA-44 API, similarly you can use ML-DSA-{65, 87} API. Execute the example by running following command.

```bash
make example -j
```

```bash
ML-DSA-44 @ NIST security level 2
Seed      : 23785cb5e49c8a44718083a3f19f7a9c69c096630dc9338440274ae174a4e79f
Pubkey    : 2548f9a2f2143eda977cd74400bbae30f7a18db4c29e6ae5dde876f3098f2ad3de0de84de70e48ac6d2670fe904e50fc2c35c215be8d3a3add72b837bd43c8f419615b3d05c0881ae520c945a0a3b7d32bda0076708bdd873ad5140e90f7319e756eec21eeb3213fc12d021f9e1b995f685fde7217c2ae52b226b56c25128579f2571520050e1f2579f37abde7eb4b60a1d8ca777aa8cd48adb28582bc4a1e656ab568f79f1c1e08e3c0314623ecfb1ed74cebc26b64a4014aa02029b9ced61dc9647ccd4e84d79d905be6b5128ca5b833d9f7c6ccd3ab7980f54b9ed1f2c31f3dad31665b07150ee30dae41937a0a2402be0e61958e69f58c8c9d64f80403ff6d761281f319e687ea9661c890db255ebd6eec0bda79019c0d153f074f7c39717acc35caffd88a79f9497189d7631e767f13694fb408128e2747a098e9408df3585801c7ff34c16d92f927ec1a1385090cfb977c3d76a7e50116c773a08183008da216453dcb7d108382f375e9861048ad5e7a23a919852ffd6abd46ac0f8c25407bdb242e6505106c244a65b57f28f3a7a9283a97f5527092df5b2a48bc54cc5e5a435f23c8e00ed9eb2dcc0fbde21678c6af6c394b297cf308e9db99ec8d1d34dacc6e514f7fcd76b3f55e07e53df995fd214abf78f9445967e45153db4555a9783999ac3a81962fbf4e8c8e6babe8e4289e2bee642d368bb878898b741de96cba3673d2d6734f75aedd4424daaa057639c487b9c75be8758c503e07f5d85c8e6263ebda86e0b86d806a9eea012d39b9ed250857487e54dde3a3f1abffbe62d439f82910a86de96a754815dfb237ba0f2e8619f478785bd3a7149b1c5d84956d5aa76ec1f402769dae2a376de2444384093931aa40adae8a43a27473e650c27a73a363bfd5060179bae4cdec2b8c106054436f913f68753a7da510fce3a19d88d09e9b1ffc3e3765a0e7316ea0a0f476e9c66bb0219138c689bc636eed615e371b9c1a90609d1438f9e2bad558ecea49b7b7a0dd7416c247f6baf4fef8292af8892c22503facd1c0aa7900cb63bdc24058013d3786465c090f288b00abdf2a376c4377ac0c51c175b36100a58a5a79ca259730d2b7282ffde5c449aab328bc83e73891184d0647a6b8824076b818505437b4a1590c0450e497a0af3d86de9e6013050bb8e29fc035faf6bffc30080afd9e73a883e3a072c4b04357d4d0a048b872c6ab728dbc952fef032c748edc5636ab1df97a1b7e40e22e882addc003fc3ebb70d37c7db59c6e0bd788e2edaf81c5080ef1cebb2a1b2ce58face5de233ee0cb452c976b2619e4e396aa90b109a8f40f6de21e1ebf846dd64823d48c1149db121bb124a94b3af081bcef298b08df88b1d0ea0f6275738d63dae48562dc08fddbef48e1be89fa41d80942bb61f6ed76a31c2f9ccf1954554e380424d4a19ef66e100d6e5c89cd91e600b1e0e3cfeadb441ee03d13186758f37f928530a9abb24162d9d1def13643027d1476290bdbac67c3eb69658925d6a250176c12870fd0d2d37bc60c2d4c7b5a7fd313f84e65ad910667ff7281dcc035f1cb2ca1135a065fae13515d1ae3704e4e0dd28932ce61b4d426e7579fa2756f05393d2453c1d7f7d14585f89008ff7bf0e140b152fb585f346496deb8fce6468a8b265c6315430de98299665522c48be311d007fd105b43e3ed083576a3cdd0fbe0d5de2590766eb35b92fe3823ceddcc1a15d9d308bcc88d85f76c305828b9d42727bd9a5f3045a949d47cc27d4f838576834233087a97db59fc05491c6aada8e47c8e4ff964170a864fbd161e74ae68665de2585bf22a61454c100e9b48595139a1a6868e
Seckey    : 2548f9a2f2143eda977cd74400bbae30f7a18db4c29e6ae5dde876f3098f2ad31125784d1df7a3e644fc32b6c4b121e86da4a397059ad1dec4d3ee636f61cfdf6b313b47fb04afacad862cda46913683d29501bea56fbb30ac2fecb81b2c531c952476b8f2c832ebc1763b67acbafe68afebb90b081ebd0b6bca9bac96f29fc00b160a0c28450ab404db384ad048262210005ab22c10096d93b4889b9251014412d18421a0986514132003136a52b6900000311c1041a1228a6146219a964c8a8080c8a020100312a212265c3489c308681a49442115424b446462308560461263a02004c40d9c420da0c22800b64c18233058a850944004442421234160a2822c102625543461a3441013c3104908509906318c286da32870d2466219014e114331cca2051498888c860151146859308c03404593c800cc2488d4322518b04c80022019c38ddb840904b02401a83149043162400201074ec2362051b881dc16040c933182384410c88813a27190288ce4448e13c94414316113336ec232641001288480848a8241c386711a200ed1c661131552c0228d5006920a483114c82ddb108a60402c0b30858c3449440668209888990432e0388c54360edb284a58123112b30458046918c4641c172c5ba410208601e1b4082396291142410b16425ac49193c264c9865009b46191c040c2221021968860c8011c1540d1a6204c388ea0068e01226a22999000302a031792c0106614356209b4688ba4215aa22093406051183104b684811685d0486952c0614042495aa8319886898282101a429008a311db3482a0c2855416525836268a440e0303514992286402515986700b304ca328060134060cc50102194663102918a529d21071d8040544b81102a2049c12250043111bb62562c82151342444164e48106d909489c29440029644e2183024358c53b20c02a34954444e44a82942382da3b0902040521b032d64b2048a36821c42080c924448366042820554482dca843020c408a312300cb86ddb325083068c1ba46c81204424c86ce4c851c08049a1b670c8026de1384009c070c2124640a210998448db24240b1670e2342418112d044589d8c681190640138980e1148da1a4901099100030829a2032a39628d1122d592650482641cb988c0c098910c12d4942480ba884a2122823034c49241113898d4a182d0bc6292315451292411b266283a8200415658b2411001469cc2290cac26114a80c4b94856032300aa42893499d8242c16529592fc10abef211edf7dbb3add12b966873c0aa4e727ed215741af7c901e3eca293ef8a680defa4650ac73f6639ffb9dbbc74d5f92e05958909ec01ae5ce63ae6528b8d2a38a261e0ef6c43544c0d0083e4d84c17777305d3026b49eafb632e27dc8240df0d0aae33e20fdab4a9cbd3423d390dabd63bdffe4f6b308eb2a29b90dd364bc5a4c86e05980eb42a5991c9348fbd3102e60f2a7ba9d98ef7dbd0534a1ced71ab51ad3a695aa57bb83e6a705e958ce6a8d5bf4ad43efd2e46f44487ec65b1d1fbf2a7ee940483b8dd32e03762ed1f0b99b8c156aaf4f6a8771d3f81e1d6bf80141a445e627e69a78bbaf6457c87d8b1e97705a61df7f66831b6812c2ebc1b255a08c4b6d876e297dcf9da10033b726647412a34e332c1724c76ea3c80c0f53b24b7b0f48f777cf500fd3911daf710eb02c7e43756353451b0d37c01fbe0402eddaac5bc392b19f7b14a04d15f96cfc2246a3fbf23abfdfc10f12623b658bf13c3a0256a22b4597b7573730cb3db8d0202a2242681817dfbac398234511dc030ce16dc51716980f5ad461cf486884ce0a8e42390c996d7feec9ac242727b5a3998853bc2f3cf2473f74035edcc33cabbfc0c08162ab00a6e7be8c5428fc52673bfd02a3d9b80eb50a8d8f1afbfaa8fdb016f873edbc709fe417474e7392af2c8b44b6f5ff9d634e0f5c597418c400296e1321a24443f07a7b7d23e3d0576f2b938cef04ac1bc1c4bbc5c9711c34ea0ad74fb08df69e49d8741039c6e76e86529342bf294f6c1ee0256d3ab42251c7e9b5571bbfe51f853351bbcba513825b9400e1b2d58b947fa841eb1c9907e8e429d7fb2d98f571420f6b79bf30857acd2d247d5db6b91a748ce8794ea8a1122edd99f79dbc21960040756aa395fa756a9751fa7812c4bbd26b2ff8a5edf03b702b197aa40e550697fb5292083aad00e62efd0288cfddd1ec796cc9b0f60a03c7ec68cb3296a2b44603ea82db07dd8c8bbbaa4f89ae5a5f8758d0076e15d11deb59eaebacd83676040afdaaa35a0c044d0d97e98ee92a88c6cf01ca9e23733048de42afa0d8817f324d221900feb8d6bf35c9535734eae59366de3f1d99367d19967af8361a13bebb9c838c5c24b2fdcaef7eea91f619b1079fb7445e3defd93d6bcc0df1891ade5bee6cd2c3dd542b703a11dbafa5a3223ca8b3e833b12bcd412dd8612139fa2390e0834f8f70e4e6fe85c8e55fa0c0850fd697cf35206a3647732afdd3709443f5f9d6eedc7ba1431dd06b8786b37852fba08ee8b9132b662455ca1b30799cf326d7d39c881877b33011bd352c42d01aaf435a95d55f8b6b5469dc1395e619d87686bf7eb60d1ce160348ca8600abcc4e64c191f81eb2ffbf2ec0ce67b9a5ceb6dda395d27010a3e1b0a66391dab80cbb1226ab5a169af5a3281fc29d364bdba27e0b78fb5650954706b059a28d5ac3405a8c7ab0ab4432a47507ffc0d90f90ac93df22c6a47fe54b477a0e59494d71fca2a94979a89bc0d2f6d5db251eb7e7aee3014b243eac2531b95113bedffac210d031f7a4a8d5c401d2949537ebba72aa4b54d3b60dd62643d4e83f0cd935868f51071cd8b9912918a4d1609631af3249e4160ee631ecd949f03263d1ed57f1bddb9fe1d6c1069db786b239bfeb36f697c8b43fb905bf21a04a7773a1a947f4948b31cc11f21d6c7dc3641372c4683a3f0f291a2a5291a88fc77fee8ebbfe63cdb5c2e31a165535c62394465409db70f86157eff00e94b018b6c994612e39f60397722aecef8d54877b02f4a9d6c5124ecce1c0a506df34d61b05c4fd97f30f959ba6e714e9cd07d84b05f11a43218fa57de47529a6b39b027dc34464f3b866d91261a4a924543f95eaf4c58c793633d83b1a46f72a06cb0147383bf21792ee0dceabb72c6823551a17c61b15877f3c496ed823a0099bbd086df39d9304bd3a1c4f1418b96c4e11aa5c2e1eb69181f07257037bd5ec16e6c544424a64a508a368c35965387b6ec1ab0a547f271e7efc3855d3b13daaaf4094f60e80dd891741d75f0e8052580b4182f13b8d6b3e56942f679b100e7bea477b67428197d9e512f0925cbf51db28980eeb6e8ccf5371281dfdb0a207de670ed8bb341f35849117ebc07506574c36b78912fdd5689154dedad65e499a7827e67bd8964e3597e234e50da33b734c48d6e8da4679db0f2b5e85b6a1401eb82fa3a43903506f3b3e6a95d61d51584e3b5cfafe177653a53670b5f6d6ea5fb68c97deef85aeafe696e98e26ad46701ebe9015e43ced3d459108b8e25f634f4d4082febba1d394295845735d94bfe6ad51cbbda37ce8cb7359d3
Message   : 8ead27a38a3ab13397215c033fe3d5b7a37d30eed7794ec13243fdf2d40d9f83
Context   : cf8c8f937a314d03
Signature : 7f6ed54ccc3e3b76f400c8e61b2afb1ef0b170a34910cd269964152a09eb0258dfd77a44f7435b7021e562e5c17cf923fbe0ae0716a87d2ecb4c4b9e01f09346424fff1de1c0a487faa55a93f43b0988afa1a7351efcbaf0e26757e95a55cbe116d5a93717308505c572cf2f212386cf300335562d7af7ecd878407456dcf90e3d651490ae96d31485e6fdb09fa2d859823ee07ee2eea31a5f9c162c2050f3b7284b4fc687bfed29e32b8e5b9d02ea42deab1a619f2a9e4583b92198f5fb0a87c6ac734630987f3c54e98e49dd80ade03ad48c053107fd382d133a14baa0a17909cf5ace770ed3e6ffe1b59318566293b8cc008175ca65e6c0d62502b3f73b8c1b42b0866dc1e70bc70dea4f3b8fa42297ee2825bb7af153f667f6fd21fc080899989af28da0c078605a89f4a8d8d268aa48f8e0a9f0d53dd998dcd7acdebc12305f16092d8ecac21e115b610b2f4b900912bc0e02d7ea323842f15e2cc1f984d64198a5908b6f281c13450f93d82ee10b4a18f163a9f9f9ea9291fcad550b2e5e334fb354b010b70328ed5b6c9040b625b3be68b53b8613370f9bff6dd1b79de5a9eea9c1831d36e2b3928f052a7bff755b97c7dde6a0ad127771ee8c5f4989ef96eb0077793d6bce25bc6f73add1652733a6832fd259f18c3892fc72c9665738d04f0c81a6f3a8c33c0b65d9dad079fb483e284d14a736928f0ab62c83251cc476610d322de34270ee433e4c911ccaaa4b9c2ab16041b058e1f4f69270028d09507fb3e885c45c4f692956caaa57219be8591a25c474ff69d62775c40d9c304d7269e797a03c2c2fe354b8a2da5a627fef831b2301ac26763557fd3fae1758b669c415f5959479c163f5e88338838ef228700b07cc4455d71a5db2ced27267fdef343e1d7a84c76f5ad87c7718eb50b9e67adf1bf8dd098eac716eb6f21efce5c367a813bc352648c3a4d898cf87ad1bc63b0cd4fde9424b2c832fdf3e1d8716fe66f836f238a519009941dee8a19f7e22bb80c2aeb0f306464fe330eaa276c62ac735b1f9e58dacd4961a7cb192214a36e7ae20b4d58699f1d47eb8a6bb4bdace893f0c38cf2272e04cad074732ce7faf38752f5f16f3458c4023c2fac2205b601a3bc64fe5f6346f92dd9f3817722f10d5ca16be04665eac3f5addaee3a14642bc99f995c08832c7307415ada7dcc6a5623d71ce39619431e7ee76345e52098428d7eeb8a32aa2767d14cdd91840b683a48d2718e94c95714e52c8f70ab19e42fec519d0586409f0996d3fd18318325606c6c448202de5b742e500096f7084a369dcfa070980783961cf7dc8de38a737a182e429fa9bdaae577e72195bc2d9bca4f0d94c599733a2be3202f77e4ed3fa1b0826444fadc1fd18330f4dd58fafaddad4eefe9e661b1a47defeb48de2584d395655b3a83ba8613ad4b298929838926b3878158c3c3894e1421cfe75b54658d977901cfa30aadf52f979b2f174d4758464b83a3e46ec32fb7834904fcf90d512c1786751428fe9eb202b14ba00e774962b3da22c51e607665d445760c874c5487c45700a7724e7490a45c5f9a3b05561c4af4217df496a7329b0e874a3acebcfdb2930f9231b2f6b6c9fef0bb2864f891fcaee7155e3255daeaa194bfc98d701f9003051c53bf7e132cbff289f3284fd590ad4c07485c4e470189dc439748e33776bdd58085bc93da662265335d06321bf67318f79bca79da38af475aecd0ac9ed0e1c131d88b9d539015e513a0928036e38747d48651ad43f4bb266f607787d7c7c4295a7ae4a9e08315030129601c86c440b9e6f6a11f9257b0739db62dae6d5e297b39c4dac0c4bb2b803a92f25063e439439fa19b3143caefb91fa541a2635f78c875fe72853f5b8daee9acfe9d7877392e4dab6d72273acc7e08ceef3aebbe9e61e4982008aa1aa99930bf6153c3c0e58d8324ddd2445c58db32ada104f605679dac82764a7c8f8439248fe06f02554b5e0698b92561f3863ebcdb1b2ba8ce95675f13cb32c981ff11e426397116eda453913bcf5cce566c56ed6d99fe42dd707f648a6a913cdb181cba32d180105d7d99a3990cfef725dfbb107f01c4cf5268e29354ab0809e39b673439b69204d89148c61470c96a34e91193e39476ccc936c05e21b1ef41f91435b9681c2cee1258419ec56e832124a9d61fc97a95c9b155e07f39203231e51da1fd644a29890c4d5531679bb79999b21f1b334693e0059034fca0e3b3e03f9e2d7d5bb6ca7df381a38e746f6830677e165555d0fa16e45ad43e7b28f9f3880b7c47ddc98693004e54b4d86c8a4ee48ee05a2b42551357707bc7178610105c375b1a506300149d53cbff720b726778ad7b2a1b44a7cdf2e671e29a73db2a5b28362069f0ccbdd76a21126fb40212f74ac526f412676d7a7407d7b3854f635576a208869a893899d3cf92ac20effd6cde6751a97cf62ccfe5114462495ffc4c24f58866e311ae85f70f1eff5b6dbe85383680eb44179dba20d7544f4cd09bc216895cd9ab755ad685e28e22d8628fdce1c871535145661908ce0c06383a3989baf76d3057be46187bac4a40dd6df0f12f1e4e6450d1b0292349253b006919c947c27b66a4dc73574345280bd486633faab2e49f00df70f08278c0b25dd394fdae3066fabf97ca11c299daf0fab04e3d32d8f464fabf80c0014ca0f7c7d6c3063ad83f560747c8c6684d46ff35598092e3e1e1d4ab8239878129b4571ea65f7685ae11f10403820483ea946ea014b8110e9c9d2aa2006556e8e75436345db8c380e3a63ae2f9b39eb8e7f3711e4a2e0714c74fc056dad239cf39fc68230b99595ea97a2ad6d24ae7de7c45bc6a209faf0dc4fdbd0114b915177f1c5e2e47502ec4b87d09c869222ff92300683ea6594167ca7c4235b912a6f3a0853a398826eab8062b7a37014df6143cc78ddc1da4ad02a29797aa6c76636591655dd2ec005c83d0ceacfb65e7a55aa57bcb1860d90c7457eb8cf1039a5a5ec2bc3e93ffcc1e6aeeb0c7caea4d5b908372bd800edd7a1abce142f6cd094bbaa100abad4b1d1bd92171c2aa41da79dd261be29be452bc9139667366718cd4661d9dc5493e220f6095e0de7fb344128a967e57c6693f500a5e677da2cea7e44b472df938719337c7e06ffe299f3db6069493dd9db96e48219af947581fda66d6534cd81557704def4d192af562d446f8d1f8838507f272be80af4a8b37cf954eecaf88096aca90cb78e49cb30d797c85d9159f8dbe26b5c7b0d6f2477a713fc87f86137bd21621164ec7a3e464f52545d5f697d839ba3aaabbecbd3dce7ebf2f506070e4f65818889aab0babcc3e1fb030d25616a8d939a9cbeccd4f2f4f5f9fe0b11131a212a353d545961676e7b7c889fa1a8d9e0f9fa0000001625364d
Verified   : true
```

> [!NOTE]
> Looking at the API documentation, in header files i.e. `include/ml_dsa/ml_dsa_{44,65,87}.hpp`, can give you a good idea of how to use ML-DSA API. Note, this library doesn't expose any raw pointer -based interface, rather *almost* everything is wrapped under statically defined `std::span` - which one can easily create from `std::{array, vector}`. I opt for using statically defined `std::span` -based function interfaces, because we always know, at compile-time, how many bytes the seeds/ keys/ signatures are, for various different ML-DSA instantiations. *This gives much better type safety and compile-time error reporting.*
