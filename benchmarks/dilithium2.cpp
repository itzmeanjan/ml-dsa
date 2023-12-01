#include "dilithium2.hpp"
#include "bench_helper.hpp"
#include <benchmark/benchmark.h>

// Benchmark Dilithium2 key generation algorithm's performance
inline void
dilithium2_keygen(benchmark::State& state)
{
  constexpr size_t slen = 32;
  constexpr size_t pklen = dilithium2::PubKeyLen;
  constexpr size_t sklen = dilithium2::SecKeyLen;

  std::vector<uint8_t> seed(slen, 0);
  std::vector<uint8_t> pubkey(pklen, 0);
  std::vector<uint8_t> seckey(sklen, 0);

  auto _seed = std::span<uint8_t, slen>(seed);
  auto _pubkey = std::span<uint8_t, pklen>(pubkey);
  auto _seckey = std::span<uint8_t, sklen>(seckey);

  prng::prng_t prng;
  prng.read(_seed);

  for (auto _ : state) {
    dilithium2::keygen(_seed, _pubkey, _seckey);

    benchmark::DoNotOptimize(_seed);
    benchmark::DoNotOptimize(_pubkey);
    benchmark::DoNotOptimize(_seckey);
    benchmark::ClobberMemory();
  }

  state.SetItemsProcessed(state.iterations());
}

// Benchmark Dilithium2 signing algorithm's performance
inline void
dilithium2_sign(benchmark::State& state)
{
  const size_t mlen = state.range(0);
  constexpr size_t slen = 32;
  constexpr size_t pklen = dilithium2::PubKeyLen;
  constexpr size_t sklen = dilithium2::SecKeyLen;
  constexpr size_t siglen = dilithium2::SigLen;

  std::vector<uint8_t> seed(slen, 0);
  std::vector<uint8_t> pkey(pklen, 0);
  std::vector<uint8_t> skey(sklen, 0);
  std::vector<uint8_t> sig(siglen, 0);
  std::vector<uint8_t> msg(mlen, 0);

  auto _seed = std::span<uint8_t, slen>(seed);
  auto _pkey = std::span<uint8_t, pklen>(pkey);
  auto _skey = std::span<uint8_t, sklen>(skey);
  auto _sig = std::span<uint8_t, siglen>(sig);
  auto _msg = std::span(msg);

  prng::prng_t prng;
  prng.read(_seed);
  prng.read(_msg);

  dilithium2::keygen(_seed, _pkey, _skey);

  for (auto _ : state) {
    dilithium2::sign(_skey, _msg, _sig, {});

    benchmark::DoNotOptimize(_skey);
    benchmark::DoNotOptimize(_msg);
    benchmark::DoNotOptimize(_sig);
    benchmark::ClobberMemory();
  }

  state.SetItemsProcessed(state.iterations());
  assert(dilithium2::verify(_pkey, _msg, _sig));
}

// Benchmark Dilithium2 signature verification routine's performance
inline void
dilithium2_verify(benchmark::State& state)
{
  const size_t mlen = state.range(0);
  constexpr size_t slen = 32;
  constexpr size_t pklen = dilithium2::PubKeyLen;
  constexpr size_t sklen = dilithium2::SecKeyLen;
  constexpr size_t siglen = dilithium2::SigLen;

  std::vector<uint8_t> seed(slen, 0);
  std::vector<uint8_t> pkey(pklen, 0);
  std::vector<uint8_t> skey(sklen, 0);
  std::vector<uint8_t> sig(siglen, 0);
  std::vector<uint8_t> msg(mlen, 0);

  auto _seed = std::span<uint8_t, slen>(seed);
  auto _pkey = std::span<uint8_t, pklen>(pkey);
  auto _skey = std::span<uint8_t, sklen>(skey);
  auto _sig = std::span<uint8_t, siglen>(sig);
  auto _msg = std::span(msg);

  prng::prng_t prng;
  prng.read(_seed);
  prng.read(_msg);

  dilithium2::keygen(_seed, _pkey, _skey);
  dilithium2::sign(_skey, _msg, _sig, {});

  for (auto _ : state) {
    bool flg = dilithium2::verify(_pkey, _msg, _sig);

    benchmark::DoNotOptimize(flg);
    benchmark::DoNotOptimize(_pkey);
    benchmark::DoNotOptimize(_msg);
    benchmark::DoNotOptimize(_sig);
    benchmark::ClobberMemory();
  }

  state.SetItemsProcessed(state.iterations());
}

BENCHMARK(dilithium2_keygen)->ComputeStatistics("min", compute_min)->ComputeStatistics("max", compute_max);
BENCHMARK(dilithium2_sign)->Arg(32)->ComputeStatistics("min", compute_min)->ComputeStatistics("max", compute_max);
BENCHMARK(dilithium2_verify)->Arg(32)->ComputeStatistics("min", compute_min)->ComputeStatistics("max", compute_max);
