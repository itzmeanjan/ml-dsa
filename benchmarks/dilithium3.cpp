#include "bench_helper.hpp"
#include "ml_dsa/ml_dsa_65.hpp"
#include <benchmark/benchmark.h>

// Benchmark Dilithium3 key generation algorithm's performance
inline void
dilithium3_keygen(benchmark::State& state)
{
  constexpr size_t slen = 32;
  constexpr size_t pklen = ml_dsa_65::PubKeyLen;
  constexpr size_t sklen = ml_dsa_65::SecKeyLen;

  std::vector<uint8_t> seed(slen, 0);
  std::vector<uint8_t> pubkey(pklen, 0);
  std::vector<uint8_t> seckey(sklen, 0);

  auto _seed = std::span<uint8_t, slen>(seed);
  auto _pubkey = std::span<uint8_t, pklen>(pubkey);
  auto _seckey = std::span<uint8_t, sklen>(seckey);

  ml_dsa_prng::prng_t<192> prng;
  prng.read(_seed);

  for (auto _ : state) {
    ml_dsa_65::keygen(_seed, _pubkey, _seckey);

    benchmark::DoNotOptimize(_seed);
    benchmark::DoNotOptimize(_pubkey);
    benchmark::DoNotOptimize(_seckey);
    benchmark::ClobberMemory();
  }

  state.SetItemsProcessed(state.iterations());
}

// Benchmark Dilithium3 signing algorithm's performance
inline void
dilithium3_sign(benchmark::State& state)
{
  const size_t mlen = state.range(0);
  constexpr size_t slen = 32;
  constexpr size_t pklen = ml_dsa_65::PubKeyLen;
  constexpr size_t sklen = ml_dsa_65::SecKeyLen;
  constexpr size_t siglen = ml_dsa_65::SigLen;

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

  ml_dsa_prng::prng_t<192> prng;
  prng.read(_seed);
  prng.read(_msg);

  ml_dsa_65::keygen(_seed, _pkey, _skey);

  for (auto _ : state) {
    ml_dsa_65::sign(_skey, _msg, _sig, {});

    benchmark::DoNotOptimize(_skey);
    benchmark::DoNotOptimize(_msg);
    benchmark::DoNotOptimize(_sig);
    benchmark::ClobberMemory();
  }

  state.SetItemsProcessed(state.iterations());
  assert(ml_dsa_65::verify(_pkey, _msg, _sig));
}

// Benchmark Dilithium3 signature verification routine's performance
inline void
dilithium3_verify(benchmark::State& state)
{
  const size_t mlen = state.range(0);
  constexpr size_t slen = 32;
  constexpr size_t pklen = ml_dsa_65::PubKeyLen;
  constexpr size_t sklen = ml_dsa_65::SecKeyLen;
  constexpr size_t siglen = ml_dsa_65::SigLen;

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

  ml_dsa_prng::prng_t<192> prng;
  prng.read(_seed);
  prng.read(_msg);

  ml_dsa_65::keygen(_seed, _pkey, _skey);
  ml_dsa_65::sign(_skey, _msg, _sig, {});

  for (auto _ : state) {
    bool flg = ml_dsa_65::verify(_pkey, _msg, _sig);

    benchmark::DoNotOptimize(flg);
    benchmark::DoNotOptimize(_pkey);
    benchmark::DoNotOptimize(_msg);
    benchmark::DoNotOptimize(_sig);
    benchmark::ClobberMemory();
  }

  state.SetItemsProcessed(state.iterations());
}

BENCHMARK(dilithium3_keygen)->ComputeStatistics("min", compute_min)->ComputeStatistics("max", compute_max);
BENCHMARK(dilithium3_sign)->Arg(32)->ComputeStatistics("min", compute_min)->ComputeStatistics("max", compute_max);
BENCHMARK(dilithium3_verify)->Arg(32)->ComputeStatistics("min", compute_min)->ComputeStatistics("max", compute_max);
