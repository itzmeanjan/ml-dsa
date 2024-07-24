#include "bench_helper.hpp"
#include "ml_dsa/ml_dsa_44.hpp"
#include <benchmark/benchmark.h>

// Benchmark performance of ML-DSA-44 key generation algorithm.
void
ml_dsa_44_keygen(benchmark::State& state)
{
  std::array<uint8_t, ml_dsa_44::KeygenSeedByteLen> seed{};
  std::array<uint8_t, ml_dsa_44::PubKeyByteLen> pubkey{};
  std::array<uint8_t, ml_dsa_44::SecKeyByteLen> seckey{};

  ml_dsa_prng::prng_t<128> prng;
  prng.read(seed);

  for (auto _ : state) {
    ml_dsa_44::keygen(seed, pubkey, seckey);

    benchmark::DoNotOptimize(seed);
    benchmark::DoNotOptimize(pubkey);
    benchmark::DoNotOptimize(seckey);
    benchmark::ClobberMemory();
  }

  state.SetItemsProcessed(state.iterations());
}

// Benchmark performance of ML-DSA-44 signing algorithm.
void
ml_dsa_44_sign(benchmark::State& state)
{
  const size_t mlen = state.range(0);

  std::vector<uint8_t> msg(mlen, 0);
  auto msg_span = std::span(msg);

  std::array<uint8_t, ml_dsa_44::KeygenSeedByteLen> seed{};
  std::array<uint8_t, ml_dsa_44::PubKeyByteLen> pubkey{};
  std::array<uint8_t, ml_dsa_44::SecKeyByteLen> seckey{};
  std::array<uint8_t, ml_dsa_44::SigningSeedByteLen> rnd{};
  std::array<uint8_t, ml_dsa_44::SigByteLen> sig{};

  ml_dsa_prng::prng_t<128> prng;
  prng.read(seed);
  prng.read(rnd);
  prng.read(msg_span);

  ml_dsa_44::keygen(seed, pubkey, seckey);

  for (auto _ : state) {
    ml_dsa_44::sign(rnd, seckey, msg_span, sig);

    benchmark::DoNotOptimize(rnd);
    benchmark::DoNotOptimize(seckey);
    benchmark::DoNotOptimize(msg_span);
    benchmark::DoNotOptimize(sig);
    benchmark::ClobberMemory();
  }

  state.SetItemsProcessed(state.iterations());
  assert(ml_dsa_44::verify(pubkey, msg_span, sig));
}

// Benchmark performance of ML-DSA-44 signature verification algorithm.
void
ml_dsa_44_verify(benchmark::State& state)
{
  const size_t mlen = state.range(0);

  std::vector<uint8_t> msg(mlen, 0);
  auto msg_span = std::span(msg);

  std::array<uint8_t, ml_dsa_44::KeygenSeedByteLen> seed{};
  std::array<uint8_t, ml_dsa_44::PubKeyByteLen> pubkey{};
  std::array<uint8_t, ml_dsa_44::SecKeyByteLen> seckey{};
  std::array<uint8_t, ml_dsa_44::SigningSeedByteLen> rnd{};
  std::array<uint8_t, ml_dsa_44::SigByteLen> sig{};

  ml_dsa_prng::prng_t<128> prng;
  prng.read(seed);
  prng.read(rnd);
  prng.read(msg_span);

  ml_dsa_44::keygen(seed, pubkey, seckey);
  ml_dsa_44::sign(rnd, seckey, msg_span, sig);

  for (auto _ : state) {
    bool is_valid = ml_dsa_44::verify(pubkey, msg_span, sig);

    benchmark::DoNotOptimize(is_valid);
    benchmark::DoNotOptimize(pubkey);
    benchmark::DoNotOptimize(msg_span);
    benchmark::DoNotOptimize(sig);
    benchmark::ClobberMemory();
  }

  state.SetItemsProcessed(state.iterations());
}

BENCHMARK(ml_dsa_44_keygen)->ComputeStatistics("min", compute_min)->ComputeStatistics("max", compute_max);
BENCHMARK(ml_dsa_44_sign)->Arg(32)->ComputeStatistics("min", compute_min)->ComputeStatistics("max", compute_max);
BENCHMARK(ml_dsa_44_verify)->Arg(32)->ComputeStatistics("min", compute_min)->ComputeStatistics("max", compute_max);
