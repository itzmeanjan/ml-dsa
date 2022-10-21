#pragma once
#include "reduction.hpp"
#include <benchmark/benchmark.h>

// Benchmark Dilithium PQC suite implementation on CPU, using google-benchmark
namespace bench_dilithium {

// Benchmark rounding algorithm, which can extract out high and low order bits
// from an element ∈ Z_q | q = 2^23 - 2^13 + 1
//
// This routine is used for reducing size of public key.
void
power2round(benchmark::State& state)
{
  constexpr size_t d = 13;
  ff::ff_t r = ff::ff_t::random();

  for (auto _ : state) {
    const auto t = dilithium_utils::power2round<d>(r);

    benchmark::DoNotOptimize(r);
    benchmark::DoNotOptimize(t);
    benchmark::ClobberMemory();
  }
}

}
