#include "bench_helper.hpp"
#include "ml_dsa/ml_dsa_87.hpp"
#include "randomshake/randomshake.hpp"
#include <benchmark/benchmark.h>

// Benchmark performance of ML-DSA-87 key generation algorithm.
void
ml_dsa_87_keygen(benchmark::State& state)
{
  std::array<uint8_t, ml_dsa_87::KeygenSeedByteLen> seed{};
  std::array<uint8_t, ml_dsa_87::PubKeyByteLen> pubkey{};
  std::array<uint8_t, ml_dsa_87::SecKeyByteLen> seckey{};

  randomshake::randomshake_t csprng;
  csprng.generate(seed);

  for (auto _ : state) {
    benchmark::DoNotOptimize(seed);
    benchmark::DoNotOptimize(pubkey);
    benchmark::DoNotOptimize(seckey);

    ml_dsa_87::keygen(seed, pubkey, seckey);

    benchmark::ClobberMemory();
  }

  state.SetItemsProcessed(state.iterations());
}

// Benchmark performance of ML-DSA-87 signing algorithm.
void
ml_dsa_87_sign(benchmark::State& state)
{
  const size_t mlen = static_cast<size_t>(state.range(0));

  std::vector<uint8_t> msg(mlen, 0);
  auto msg_span = std::span(msg);

  std::array<uint8_t, ml_dsa_87::KeygenSeedByteLen> seed{};
  std::array<uint8_t, ml_dsa_87::PubKeyByteLen> pubkey{};
  std::array<uint8_t, ml_dsa_87::SecKeyByteLen> seckey{};
  std::array<uint8_t, ml_dsa_87::SigningSeedByteLen> rnd{};
  std::array<uint8_t, ml_dsa_87::SigByteLen> sig{};

  constexpr std::array<uint8_t, randomshake::randomshake_t<>::seed_byte_len> CSPRNG_SEED{ 0 };
  randomshake::randomshake_t csprng(CSPRNG_SEED);

  csprng.generate(seed);
  csprng.generate(rnd);
  csprng.generate(msg_span);

  ml_dsa_87::keygen(seed, pubkey, seckey);

  bool has_signed = true;
  for (auto _ : state) {
    benchmark::DoNotOptimize(has_signed);
    benchmark::DoNotOptimize(rnd);
    benchmark::DoNotOptimize(seckey);
    benchmark::DoNotOptimize(msg_span);
    benchmark::DoNotOptimize(sig);

    has_signed &= ml_dsa_87::sign(rnd, seckey, msg_span, {}, sig);

    benchmark::ClobberMemory();
  }

  assert(has_signed);
  assert(ml_dsa_87::verify(pubkey, msg_span, {}, sig));

  state.SetItemsProcessed(state.iterations());
}

// Benchmark performance of ML-DSA-87 signature verification algorithm.
void
ml_dsa_87_verify(benchmark::State& state)
{
  const size_t mlen = static_cast<size_t>(state.range(0));

  std::vector<uint8_t> msg(mlen, 0);
  auto msg_span = std::span(msg);

  std::array<uint8_t, ml_dsa_87::KeygenSeedByteLen> seed{};
  std::array<uint8_t, ml_dsa_87::PubKeyByteLen> pubkey{};
  std::array<uint8_t, ml_dsa_87::SecKeyByteLen> seckey{};
  std::array<uint8_t, ml_dsa_87::SigningSeedByteLen> rnd{};
  std::array<uint8_t, ml_dsa_87::SigByteLen> sig{};

  randomshake::randomshake_t csprng;
  csprng.generate(seed);
  csprng.generate(rnd);
  csprng.generate(msg_span);

  ml_dsa_87::keygen(seed, pubkey, seckey);
  const bool has_signed = ml_dsa_87::sign(rnd, seckey, msg_span, {}, sig);
  assert(has_signed);
  (void)has_signed;

  bool has_verified = true;
  for (auto _ : state) {
    benchmark::DoNotOptimize(has_verified);
    benchmark::DoNotOptimize(pubkey);
    benchmark::DoNotOptimize(msg_span);
    benchmark::DoNotOptimize(sig);

    has_verified &= ml_dsa_87::verify(pubkey, msg_span, {}, sig);

    benchmark::ClobberMemory();
  }

  assert(has_verified);

  state.SetItemsProcessed(state.iterations());
}

BENCHMARK(ml_dsa_87_keygen)->ComputeStatistics("min", compute_min)->ComputeStatistics("max", compute_max);
BENCHMARK(ml_dsa_87_sign)->Arg(32)->ComputeStatistics("min", compute_min)->ComputeStatistics("max", compute_max);
BENCHMARK(ml_dsa_87_verify)->Arg(32)->ComputeStatistics("min", compute_min)->ComputeStatistics("max", compute_max);
