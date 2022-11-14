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

// Benchmark decompose routine, with randomly generated Z_q element
// | q = 2^23 - 2^13 + 1
template<const uint32_t alpha>
void
decompose(benchmark::State& state)
{
  ff::ff_t r = ff::ff_t::random();

  for (auto _ : state) {
    const auto s = dilithium_utils::decompose<alpha>(r);

    benchmark::DoNotOptimize(r);
    benchmark::DoNotOptimize(s);
    benchmark::ClobberMemory();
  }
}

// Benchmarks performance of computation of hint bit, given one arbitrary Z_q
// element r and another small Z_q element z
template<const uint32_t alpha>
void
make_hint(benchmark::State& state)
{
  constexpr uint32_t m = (ff::Q - 1u) / alpha;
  constexpr ff::ff_t z{ m - 1u };

  ff::ff_t r = ff::ff_t::random();

  for (auto _ : state) {
    const ff::ff_t h = dilithium_utils::make_hint<alpha>(z, r);

    benchmark::DoNotOptimize(z);
    benchmark::DoNotOptimize(r);
    benchmark::DoNotOptimize(h);
    benchmark::ClobberMemory();
  }
}

// Benchmarks performance of routine which uses hint bit and attempts to recover
// high order bits of r + z s.t. r is arbitrary element ∈	Z_q and z is
// small element ∈ Z_q
template<const uint32_t alpha>
void
use_hint(benchmark::State& state)
{
  constexpr uint32_t m = (ff::Q - 1u) / alpha;
  constexpr ff::ff_t z{ m - 1u };

  ff::ff_t r = ff::ff_t::random();
  ff::ff_t h = dilithium_utils::make_hint<alpha>(z, r);

  for (auto _ : state) {
    const ff::ff_t rz = dilithium_utils::use_hint<alpha>(h, r);

    benchmark::DoNotOptimize(h);
    benchmark::DoNotOptimize(r);
    benchmark::DoNotOptimize(rz);
    benchmark::ClobberMemory();
  }
}

}
