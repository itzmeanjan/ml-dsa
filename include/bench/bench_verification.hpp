#pragma once
#include "dilithium.hpp"
#include "utils.hpp"
#include "x86_64_cpu_cycles.hpp"
#include <benchmark/benchmark.h>

// Benchmark Dilithium PQC suite implementation on CPU, using google-benchmark
namespace bench_dilithium {

// Benchmark Dilithium signature verification routine
template<const size_t k,
         const size_t l,
         const size_t d,
         const uint32_t η,
         const uint32_t γ1,
         const uint32_t γ2,
         const uint32_t τ,
         const uint32_t β,
         const size_t ω>
void
verify(benchmark::State& state)
{
  const size_t mlen = state.range(0);
  constexpr size_t slen = 32;
  constexpr size_t pklen = dilithium_utils::pubkey_length<k, d>();
  constexpr size_t sklen = dilithium_utils::seckey_length<k, l, η, d>();
  constexpr size_t siglen = dilithium_utils::signature_length<k, l, γ1, ω>();

  uint8_t* seed = static_cast<uint8_t*>(std::malloc(slen));
  uint8_t* pkey = static_cast<uint8_t*>(std::malloc(pklen));
  uint8_t* skey = static_cast<uint8_t*>(std::malloc(sklen));
  uint8_t* sig = static_cast<uint8_t*>(std::malloc(siglen));
  uint8_t* msg = static_cast<uint8_t*>(std::malloc(mlen));

  dilithium_utils::random_data<uint8_t>(seed, slen);
  dilithium_utils::random_data<uint8_t>(msg, mlen);

  dilithium::keygen<k, l, d, η>(seed, pkey, skey);
  dilithium::sign<k, l, d, η, γ1, γ2, τ, β, ω>(skey, msg, mlen, sig);

#if defined __x86_64__
  uint64_t total_cycles = 0ul;
#endif

  for (auto _ : state) {
    bool flg = false;

#if defined __x86_64__
    const uint64_t start = cpu_cycles();
#endif

    flg = dilithium::verify<k, l, d, γ1, γ2, τ, β, ω>(pkey, msg, mlen, sig);

#if defined __x86_64__
    const uint64_t end = cpu_cycles();
    total_cycles += (end - start);
#endif

    assert(flg);

    benchmark::DoNotOptimize(flg);
    benchmark::DoNotOptimize(pkey);
    benchmark::DoNotOptimize(msg);
    benchmark::DoNotOptimize(sig);
    benchmark::ClobberMemory();
  }

  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));

#if defined __x86_64__
  total_cycles /= static_cast<uint64_t>(state.iterations());
  state.counters["average_cpu_cycles"] = static_cast<double>(total_cycles);
#endif

  std::free(seed);
  std::free(pkey);
  std::free(skey);
  std::free(sig);
  std::free(msg);
}

}
