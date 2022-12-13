#pragma once
#include "bench_common.hpp"
#include "dilithium.hpp"
#include "utils.hpp"

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

  std::vector<uint64_t> durations;

  for (auto _ : state) {
    // use random seed for key generation
    dilithium_utils::random_data<uint8_t>(seed, slen);
    // use random message ( to be signed )
    dilithium_utils::random_data<uint8_t>(msg, mlen);
    // generate keypair ( from random sampled seed )
    dilithium::keygen<k, l, d, η>(seed, pkey, skey);
    // sign message
    dilithium::sign<k, l, d, η, γ1, γ2, τ, β, ω>(skey, msg, mlen, sig);

    const auto t0 = std::chrono::high_resolution_clock::now();
    bool flg = false;
    flg = dilithium::verify<k, l, d, γ1, γ2, τ, β, ω>(pkey, msg, mlen, sig);
    const auto t1 = std::chrono::high_resolution_clock::now();

    const auto sdur = std::chrono::duration_cast<seconds_t>(t1 - t0);
    const auto nsdur = std::chrono::duration_cast<nano_t>(t1 - t0);

    state.SetIterationTime(sdur.count());
    durations.push_back(nsdur.count());

    assert(flg);

    benchmark::DoNotOptimize(flg);
    benchmark::DoNotOptimize(pkey);
    benchmark::DoNotOptimize(msg);
    benchmark::DoNotOptimize(sig);
    benchmark::ClobberMemory();
  }

  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));

  const auto min_idx = std::min_element(durations.begin(), durations.end());
  const auto min = durations.at(std::distance(durations.begin(), min_idx));
  state.counters["min_exec_time (ns)"] = static_cast<double>(min);

  const auto max_idx = std::max_element(durations.begin(), durations.end());
  const auto max = durations.at(std::distance(durations.begin(), max_idx));
  state.counters["max_exec_time (ns)"] = static_cast<double>(max);

  const auto lenby2 = durations.size() / 2;
  const auto mid_idx = durations.begin() + lenby2;
  std::nth_element(durations.begin(), mid_idx, durations.end());
  const auto mid = durations[lenby2];
  state.counters["median_exec_time (ns)"] = static_cast<double>(mid);

  std::free(seed);
  std::free(pkey);
  std::free(skey);
  std::free(sig);
  std::free(msg);
}

}
