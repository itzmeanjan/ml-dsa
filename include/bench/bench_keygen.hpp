#pragma once
#include "keygen.hpp"
#include "utils.hpp"
#include "x86_64_cpu_cycles.hpp"
#include <benchmark/benchmark.h>

// Benchmark Dilithium PQC suite implementation on CPU, using google-benchmark
namespace bench_dilithium {

// Benchmark Dilithium Key Generation Algorithm
template<const size_t k, const size_t l, const size_t d, const uint32_t η>
void
keygen(benchmark::State& state)
{
  constexpr size_t slen = 32;
  constexpr size_t pklen = dilithium_utils::pubkey_length<k, d>();
  constexpr size_t sklen = dilithium_utils::seckey_length<k, l, η, d>();

  uint8_t* seed = static_cast<uint8_t*>(std::malloc(slen));
  uint8_t* pubkey = static_cast<uint8_t*>(std::malloc(pklen));
  uint8_t* seckey = static_cast<uint8_t*>(std::malloc(sklen));

  dilithium_utils::random_data<uint8_t>(seed, slen);

#if defined __x86_64__
  uint64_t total_cycles = 0ul;
#endif

  for (auto _ : state) {
#if defined __x86_64__
    const uint64_t start = cpu_cycles();
#endif

    dilithium::keygen<k, l, d, η>(seed, pubkey, seckey);

#if defined __x86_64__
    const uint64_t end = cpu_cycles();
    total_cycles += (end - start);
#endif

    benchmark::DoNotOptimize(seed);
    benchmark::DoNotOptimize(pubkey);
    benchmark::DoNotOptimize(seckey);
    benchmark::ClobberMemory();
  }

  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));

#if defined __x86_64__
  total_cycles /= static_cast<uint64_t>(state.iterations());
  state.counters["average_cpu_cycles"] = static_cast<double>(total_cycles);
#endif

  std::free(seed);
  std::free(pubkey);
  std::free(seckey);
}

}
