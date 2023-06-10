#pragma once
#include "dilithium3.hpp"
#include <benchmark/benchmark.h>

// Benchmark Dilithium PQC DSA suite implementation targeting CPU, using
// google-benchmark
namespace bench_dilithium {

// Benchmark Dilithium3 key generation algorithm's performance
inline void
dilithium3_keygen(benchmark::State& state)
{
  constexpr size_t slen = 32;
  constexpr size_t pklen = dilithium3::PubKeyLen;
  constexpr size_t sklen = dilithium3::SecKeyLen;

  uint8_t* seed = static_cast<uint8_t*>(std::malloc(slen));
  uint8_t* pubkey = static_cast<uint8_t*>(std::malloc(pklen));
  uint8_t* seckey = static_cast<uint8_t*>(std::malloc(sklen));

  prng::prng_t prng;
  prng.read(seed, slen);

  for (auto _ : state) {
    dilithium3::keygen(seed, pubkey, seckey);

    benchmark::DoNotOptimize(seed);
    benchmark::DoNotOptimize(pubkey);
    benchmark::DoNotOptimize(seckey);
    benchmark::ClobberMemory();
  }

  state.SetItemsProcessed(state.iterations());

  std::free(seed);
  std::free(pubkey);
  std::free(seckey);
}

// Benchmark Dilithium3 signing algorithm's performance
inline void
dilithium3_sign(benchmark::State& state)
{
  const size_t mlen = state.range(0);
  constexpr size_t slen = 32;
  constexpr size_t pklen = dilithium3::PubKeyLen;
  constexpr size_t sklen = dilithium3::SecKeyLen;
  constexpr size_t siglen = dilithium3::SigLen;

  uint8_t* seed = static_cast<uint8_t*>(std::malloc(slen));
  uint8_t* pkey = static_cast<uint8_t*>(std::malloc(pklen));
  uint8_t* skey = static_cast<uint8_t*>(std::malloc(sklen));
  uint8_t* sig = static_cast<uint8_t*>(std::malloc(siglen));
  uint8_t* msg = static_cast<uint8_t*>(std::malloc(mlen));

  prng::prng_t prng;
  prng.read(seed, slen);
  prng.read(msg, mlen);

  dilithium3::keygen(seed, pkey, skey);

  for (auto _ : state) {
    dilithium3::sign(skey, msg, mlen, sig, nullptr);

    benchmark::DoNotOptimize(skey);
    benchmark::DoNotOptimize(msg);
    benchmark::DoNotOptimize(sig);
    benchmark::ClobberMemory();
  }

  state.SetItemsProcessed(state.iterations());

  const bool flg = dilithium3::verify(pkey, msg, mlen, sig);

  std::free(seed);
  std::free(pkey);
  std::free(skey);
  std::free(sig);
  std::free(msg);

  assert(flg);
}

// Benchmark Dilithium3 signature verification routine's performance
inline void
dilithium3_verify(benchmark::State& state)
{
  const size_t mlen = state.range(0);
  constexpr size_t slen = 32;
  constexpr size_t pklen = dilithium3::PubKeyLen;
  constexpr size_t sklen = dilithium3::SecKeyLen;
  constexpr size_t siglen = dilithium3::SigLen;

  uint8_t* seed = static_cast<uint8_t*>(std::malloc(slen));
  uint8_t* pkey = static_cast<uint8_t*>(std::malloc(pklen));
  uint8_t* skey = static_cast<uint8_t*>(std::malloc(sklen));
  uint8_t* sig = static_cast<uint8_t*>(std::malloc(siglen));
  uint8_t* msg = static_cast<uint8_t*>(std::malloc(mlen));

  prng::prng_t prng;
  prng.read(seed, slen);
  prng.read(msg, mlen);

  dilithium3::keygen(seed, pkey, skey);
  dilithium3::sign(skey, msg, mlen, sig, nullptr);

  for (auto _ : state) {
    bool flg = dilithium3::verify(pkey, msg, mlen, sig);

    benchmark::DoNotOptimize(flg);
    benchmark::DoNotOptimize(pkey);
    benchmark::DoNotOptimize(msg);
    benchmark::DoNotOptimize(sig);
    benchmark::ClobberMemory();
  }

  state.SetItemsProcessed(state.iterations());

  std::free(seed);
  std::free(pkey);
  std::free(skey);
  std::free(sig);
  std::free(msg);
}

}
