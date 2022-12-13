#pragma once
#include "bench_common.hpp"
#include "keygen.hpp"
#include "utils.hpp"

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

  std::vector<uint64_t> durations;

  for (auto _ : state) {
    // use random seed for key generation
    dilithium_utils::random_data<uint8_t>(seed, slen);

    const auto t0 = std::chrono::high_resolution_clock::now();
    dilithium::keygen<k, l, d, η>(seed, pubkey, seckey);
    const auto t1 = std::chrono::high_resolution_clock::now();

    const auto sdur = std::chrono::duration_cast<seconds_t>(t1 - t0);
    const auto nsdur = std::chrono::duration_cast<nano_t>(t1 - t0);

    state.SetIterationTime(sdur.count());
    durations.push_back(nsdur.count());

    benchmark::DoNotOptimize(seed);
    benchmark::DoNotOptimize(pubkey);
    benchmark::DoNotOptimize(seckey);
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
  std::free(pubkey);
  std::free(seckey);
}

}
