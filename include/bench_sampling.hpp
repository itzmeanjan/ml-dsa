#pragma once
#include "sampling.hpp"
#include "utils.hpp"
#include <benchmark/benchmark.h>

// Benchmark Dilithium PQC suite implementation on CPU, using google-benchmark
namespace bench_dilithium {

// Benchmarks routine which computes matrix A by rejection sampling over output
// of a XOF ( read SHAKE128 ), seeded with 32 -bytes randomness.
template<const size_t k, const size_t l>
void
expand_a(benchmark::State& state)
{
  constexpr size_t slen = 32;
  constexpr size_t mlen = k * l * ntt::N * sizeof(ff::ff_t);

  uint8_t* seed = static_cast<uint8_t*>(std::malloc(slen));
  ff::ff_t* mat = static_cast<ff::ff_t*>(std::malloc(mlen));

  dilithium_utils::random_data<uint8_t>(seed, 32);

  for (auto _ : state) {
    dilithium_utils::expand_a<k, l>(seed, mat);

    benchmark::DoNotOptimize(seed);
    benchmark::DoNotOptimize(mat);
    benchmark::ClobberMemory();
  }

  std::free(seed);
  std::free(mat);
}

}
