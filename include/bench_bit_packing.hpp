#pragma once
#include "bit_packing.hpp"
#include "test_bit_packing.hpp"
#include <benchmark/benchmark.h>
#include <cassert>

// Benchmark Dilithium PQC suite implementation on CPU, using google-benchmark
namespace bench_dilithium {

// Benchmark serialization of degree-255 polynomial's lower `sbw` -many
// significant bits to byte array of length 32 * sbw.
template<const size_t sbw>
void
encode(benchmark::State& state)
{
  constexpr size_t alen = sbw * 32;
  constexpr size_t plen = sizeof(ff::ff_t) * ntt::N;

  ff::ff_t* polya = static_cast<ff::ff_t*>(std::malloc(plen));
  uint8_t* arr = static_cast<uint8_t*>(std::malloc(alen));
  ff::ff_t* polyb = static_cast<ff::ff_t*>(std::malloc(plen));

  for (size_t i = 0; i < ntt::N; i++) {
    polya[i] = ff::ff_t::random();
  }

  for (auto _ : state) {
    dilithium_utils::encode<sbw>(polya, arr);

    benchmark::DoNotOptimize(polya);
    benchmark::DoNotOptimize(arr);
    benchmark::ClobberMemory();
  }

  dilithium_utils::decode<sbw>(arr, polyb);

  constexpr size_t mask = (1u << sbw) - 1u;
  bool flg = false;

  for (size_t i = 0; i < ntt::N; i++) {
    flg |= ((polya[i].v & mask) ^ polyb[i].v);
  }

  assert(!flg);

  std::free(polya);
  std::free(polyb);
  std::free(arr);
}

// Benchmark deserialization of byte array ( of length 32 * sbw ) to degree-255
// polynomial's lower `sbw` -many significant bits.
template<const size_t sbw>
void
decode(benchmark::State& state)
{
  constexpr size_t alen = sbw * 32;
  constexpr size_t plen = sizeof(ff::ff_t) * ntt::N;

  ff::ff_t* polya = static_cast<ff::ff_t*>(std::malloc(plen));
  uint8_t* arr = static_cast<uint8_t*>(std::malloc(alen));
  ff::ff_t* polyb = static_cast<ff::ff_t*>(std::malloc(plen));

  for (size_t i = 0; i < ntt::N; i++) {
    polya[i] = ff::ff_t::random();
  }

  dilithium_utils::encode<sbw>(polya, arr);

  for (auto _ : state) {
    dilithium_utils::decode<sbw>(arr, polyb);

    benchmark::DoNotOptimize(arr);
    benchmark::DoNotOptimize(polyb);
    benchmark::ClobberMemory();
  }

  constexpr size_t mask = (1u << sbw) - 1u;
  bool flg = false;

  for (size_t i = 0; i < ntt::N; i++) {
    flg |= ((polya[i].v & mask) ^ polyb[i].v);
  }

  assert(!flg);

  std::free(polya);
  std::free(polyb);
  std::free(arr);
}

// Benchmark encoding of vector ( of dimension k x 1 ) of degree-255 polynomials
// ( with hint bits ) to byte array to length (ω + k) s.t. there are at max ω
// -many set bits in hint bit array.
template<const size_t k, const size_t ω>
void
encode_hint_bits(benchmark::State& state)
{
  constexpr size_t hlen = sizeof(ff::ff_t) * k * ntt::N;
  constexpr size_t alen = ω + k;

  ff::ff_t* h0 = static_cast<ff::ff_t*>(std::malloc(hlen));
  ff::ff_t* h1 = static_cast<ff::ff_t*>(std::malloc(hlen));
  uint8_t* arr = static_cast<uint8_t*>(std::malloc(alen));

  test_dilithium::generate_random_hint_bits<k, ω>(h0);

  for (auto _ : state) {
    dilithium_utils::encode_hint_bits<k, ω>(h0, arr);

    benchmark::DoNotOptimize(h0);
    benchmark::DoNotOptimize(arr);
    benchmark::ClobberMemory();
  }

  const bool failed = dilithium_utils::decode_hint_bits<k, ω>(arr, h1);
  bool flg = true;

  for (size_t i = 0; i < k * ntt::N; i++) {
    flg &= (h0[i] == h1[i]);
  }

  std::free(h0);
  std::free(h1);
  std::free(arr);

  assert(flg & !failed);
}

// Benchmark decoding of byte array of lenth (ω + k) to a vector ( of dimension
// k x 1 ) of degree-255 polynomials ( with hint bits ) s.t. there are at max ω
// -many set bits in hint bit array.
template<const size_t k, const size_t ω>
void
decode_hint_bits(benchmark::State& state)
{
  constexpr size_t hlen = sizeof(ff::ff_t) * k * ntt::N;
  constexpr size_t alen = ω + k;

  ff::ff_t* h0 = static_cast<ff::ff_t*>(std::malloc(hlen));
  ff::ff_t* h1 = static_cast<ff::ff_t*>(std::malloc(hlen));
  uint8_t* arr = static_cast<uint8_t*>(std::malloc(alen));

  test_dilithium::generate_random_hint_bits<k, ω>(h0);
  dilithium_utils::encode_hint_bits<k, ω>(h0, arr);

  for (auto _ : state) {
    const bool failed = dilithium_utils::decode_hint_bits<k, ω>(arr, h1);
    assert(!failed);

    benchmark::DoNotOptimize(failed);
    benchmark::DoNotOptimize(arr);
    benchmark::DoNotOptimize(h1);
    benchmark::ClobberMemory();
  }

  bool flg = true;
  for (size_t i = 0; i < k * ntt::N; i++) {
    flg &= (h0[i] == h1[i]);
  }

  std::free(h0);
  std::free(h1);
  std::free(arr);

  assert(flg);
}

}
