#pragma once
#include "keygen.hpp"
#include "utils.hpp"
#include <benchmark/benchmark.h>

// Benchmark Dilithium PQC suite implementation on CPU, using google-benchmark
namespace bench_dilithium {

// Benchmark Dilithium Key Generation Algorithm, for different parameter sets
template<const size_t k, const size_t l, const uint32_t η, const size_t d>
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

  for (auto _ : state) {
    dilithium::keygen<k, l, η, d>(seed, pubkey, seckey);

    benchmark::DoNotOptimize(seed);
    benchmark::DoNotOptimize(pubkey);
    benchmark::DoNotOptimize(seckey);
    benchmark::ClobberMemory();
  }

  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));

  std::free(seed);
  std::free(pubkey);
  std::free(seckey);
}

}
