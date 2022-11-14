#pragma once
#include "ntt.hpp"
#include <benchmark/benchmark.h>
#include <cstring>

// Benchmark Dilithium PQC suite implementation on CPU, using google-benchmark
namespace bench_dilithium {

// Benchmark Number Theoretic Transform of degree-255 polynomial over
// Z_q | q = 2^23 - 2^13 + 1
void
ntt(benchmark::State& state)
{
  ff::ff_t poly_a[ntt::N]{};
  ff::ff_t poly_b[ntt::N]{};

  for (size_t i = 0; i < ntt::N; i++) {
    poly_a[i] = ff::ff_t::random();
  }

  std::memcpy(poly_b, poly_a, sizeof(ff::ff_t) * ntt::N);

  for (auto _ : state) {
    ntt::ntt(poly_b);

    benchmark::DoNotOptimize(poly_b);
    benchmark::ClobberMemory();
  }

  const size_t itr = state.iterations();

  for (size_t i = 0; i < itr; i++) {
    ntt::ntt(poly_a);
  }

  for (size_t i = 0; i < ntt::N; i++) {
    assert(poly_a[i] == poly_b[i]);
  }
}

// Benchmark inverse Number Theoretic Transform of degree-255 polynomial over
// Z_q | q = 2^23 - 2^13 + 1
void
intt(benchmark::State& state)
{
  ff::ff_t poly_a[ntt::N]{};
  ff::ff_t poly_b[ntt::N]{};

  for (size_t i = 0; i < ntt::N; i++) {
    poly_a[i] = ff::ff_t::random();
  }

  std::memcpy(poly_b, poly_a, sizeof(ff::ff_t) * ntt::N);

  for (auto _ : state) {
    ntt::intt(poly_b);

    benchmark::DoNotOptimize(poly_b);
    benchmark::ClobberMemory();
  }

  const size_t itr = state.iterations();

  for (size_t i = 0; i < itr; i++) {
    ntt::intt(poly_a);
  }

  for (size_t i = 0; i < ntt::N; i++) {
    assert(poly_a[i] == poly_b[i]);
  }
}

}
