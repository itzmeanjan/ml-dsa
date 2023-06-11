#pragma once
#include "bit_packing.hpp"
#include "field.hpp"
#include "prng.hpp"
#include <cassert>

// Test functional correctness of Dilithium PQC suite implementation
namespace test_dilithium {

// Check for functional correctness of
//
// - polynomial to byte array encoding
// - decoding of polynomial from byte array
template<const size_t sbw>
void
test_encode_decode()
  requires(dilithium_params::check_sbw(sbw))
{
  constexpr size_t alen = sbw * 32;
  constexpr size_t plen = sizeof(field::zq_t) * ntt::N;

  field::zq_t* polya = static_cast<field::zq_t*>(std::malloc(plen));
  field::zq_t* polyb = static_cast<field::zq_t*>(std::malloc(plen));
  uint8_t* arr = static_cast<uint8_t*>(std::malloc(alen));

  prng::prng_t prng;

  for (size_t i = 0; i < ntt::N; i++) {
    polya[i] = field::zq_t::random(prng);
  }

  bit_packing::encode<sbw>(polya, arr);
  bit_packing::decode<sbw>(arr, polyb);

  constexpr size_t mask = (1u << sbw) - 1u;
  bool flg = false;

  for (size_t i = 0; i < ntt::N; i++) {
    flg |= static_cast<bool>((polya[i].v & mask) ^ polyb[i].v);
  }

  std::free(polya);
  std::free(polyb);
  std::free(arr);

  assert(!flg);
}

// Generates random hint bit polynomial vector of dimension k x 1, with <= ω
// coefficients set to 1.
template<const size_t k, const size_t ω>
void
generate_random_hint_bits(field::zq_t* const __restrict poly)
{
  for (size_t i = 0; i < k * ntt::N; i++) {
    poly[i] = field::zq_t::zero();
  }

  constexpr size_t frm = 0;
  constexpr size_t to = k * ntt::N - 1;

  std::random_device rd;
  std::mt19937_64 gen(rd());
  std::uniform_int_distribution<size_t> dis{ frm, to };

  for (size_t i = 0; i < ω; i++) {
    const size_t idx = dis(gen);
    poly[idx] = field::zq_t{ 1u };
  }
}

// Test functional correctness of encoding and decoding of hint bit polynomial
// vector.
template<const size_t k, const size_t ω>
void
test_encode_decode_hint_bits()
{
  constexpr size_t hlen = sizeof(field::zq_t) * k * ntt::N;
  constexpr size_t alen = ω + k;

  field::zq_t* h0 = static_cast<field::zq_t*>(std::malloc(hlen));
  field::zq_t* h1 = static_cast<field::zq_t*>(std::malloc(hlen));
  field::zq_t* h2 = static_cast<field::zq_t*>(std::malloc(hlen));
  uint8_t* arr0 = static_cast<uint8_t*>(std::malloc(alen));
  uint8_t* arr1 = static_cast<uint8_t*>(std::malloc(alen));

  generate_random_hint_bits<k, ω>(h0);

  bit_packing::encode_hint_bits<k, ω>(h0, arr0);
  std::memcpy(arr1, arr0, alen);
  arr1[alen - 1] = ~arr1[alen - 1];

  const bool failed0 = bit_packing::decode_hint_bits<k, ω>(arr0, h1);
  const bool failed1 = bit_packing::decode_hint_bits<k, ω>(arr1, h2);

  bool flg = true;

  for (size_t i = 0; i < k * ntt::N; i++) {
    flg &= (h0[i] == h1[i]);
  }

  std::free(h0);
  std::free(h1);
  std::free(h2);
  std::free(arr0);
  std::free(arr1);

  assert(flg & !failed0 & failed1);
}

}
