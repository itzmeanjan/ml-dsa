#pragma once
#include "bench_common.hpp"
#include "dilithium3.hpp"

// Benchmark Dilithium PQC suite implementation on CPU, using google-benchmark
namespace bench_dilithium {

// Benchmark Dilithium3 key generation algorithm's performance
inline void
dilithium3_keygen(benchmark::State& state)
{
  constexpr size_t slen = 32;
  constexpr size_t pklen = dilithium3::PubKeyLength;
  constexpr size_t sklen = dilithium3::SecKeyLength;

  uint8_t* seed = static_cast<uint8_t*>(std::malloc(slen));
  uint8_t* pubkey = static_cast<uint8_t*>(std::malloc(pklen));
  uint8_t* seckey = static_cast<uint8_t*>(std::malloc(sklen));

  prng::prng_t prng;

  std::vector<uint64_t> durations;

  for (auto _ : state) {
    // use random seed for key generation
    prng.read(seed, slen);

    const auto t0 = std::chrono::high_resolution_clock::now();
    dilithium3::keygen(seed, pubkey, seckey);
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

// Benchmark Dilithium3 signing algorithm's performance
inline void
dilithium3_sign(benchmark::State& state)
{
  const size_t mlen = state.range(0);
  constexpr size_t slen = 32;
  constexpr size_t pklen = dilithium3::PubKeyLength;
  constexpr size_t sklen = dilithium3::SecKeyLength;
  constexpr size_t siglen = dilithium3::SigLength;

  uint8_t* seed = static_cast<uint8_t*>(std::malloc(slen));
  uint8_t* pkey = static_cast<uint8_t*>(std::malloc(pklen));
  uint8_t* skey = static_cast<uint8_t*>(std::malloc(sklen));
  uint8_t* sig = static_cast<uint8_t*>(std::malloc(siglen));
  uint8_t* msg = static_cast<uint8_t*>(std::malloc(mlen));

  prng::prng_t prng;

  std::vector<uint64_t> durations;

  for (auto _ : state) {
    // use random seed for key generation
    prng.read(seed, slen);
    // use random message ( to be signed )
    prng.read(msg, mlen);
    // generate keypair ( from random sampled seed )
    dilithium3::keygen(seed, pkey, skey);

    const auto t0 = std::chrono::high_resolution_clock::now();
    dilithium3::sign(skey, msg, mlen, sig);
    const auto t1 = std::chrono::high_resolution_clock::now();

    const auto sdur = std::chrono::duration_cast<seconds_t>(t1 - t0);
    const auto nsdur = std::chrono::duration_cast<nano_t>(t1 - t0);

    state.SetIterationTime(sdur.count());
    durations.push_back(nsdur.count());

    benchmark::DoNotOptimize(skey);
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

  bool flg = dilithium3::verify(pkey, msg, mlen, sig);

  std::free(seed);
  std::free(pkey);
  std::free(skey);
  std::free(sig);
  std::free(msg);

  assert(flg);
}

// Benchmark Dilithium3 signature verification routine's performance
inline void
dilithium3_verify(benchmark::State& state)
{
  const size_t mlen = state.range(0);
  constexpr size_t slen = 32;
  constexpr size_t pklen = dilithium3::PubKeyLength;
  constexpr size_t sklen = dilithium3::SecKeyLength;
  constexpr size_t siglen = dilithium3::SigLength;

  uint8_t* seed = static_cast<uint8_t*>(std::malloc(slen));
  uint8_t* pkey = static_cast<uint8_t*>(std::malloc(pklen));
  uint8_t* skey = static_cast<uint8_t*>(std::malloc(sklen));
  uint8_t* sig = static_cast<uint8_t*>(std::malloc(siglen));
  uint8_t* msg = static_cast<uint8_t*>(std::malloc(mlen));

  prng::prng_t prng;

  std::vector<uint64_t> durations;

  for (auto _ : state) {
    // use random seed for key generation
    prng.read(seed, slen);
    // use random message ( to be signed )
    prng.read(msg, mlen);
    // generate keypair ( from random sampled seed )
    dilithium3::keygen(seed, pkey, skey);
    // sign message
    dilithium3::sign(skey, msg, mlen, sig);

    const auto t0 = std::chrono::high_resolution_clock::now();
    bool flg = false;
    flg = dilithium3::verify(pkey, msg, mlen, sig);
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
