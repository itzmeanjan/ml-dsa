#pragma once
#include "bit_packing.hpp"

// Test functional correctness of Dilithium PQC suite implementation
namespace test_dilithium {

// Check for functional correctness of
//
// - polynomial to byte array encoding
// - decoding of polynomial from byte array
template<const size_t sbw>
static void
test_encode_decode() requires(dilithium_utils::check_sbw(sbw))
{
  constexpr size_t alen = sbw * 32;
  constexpr size_t plen = sizeof(ff::ff_t) * ntt::N;

  ff::ff_t* polya = static_cast<ff::ff_t*>(std::malloc(plen));
  ff::ff_t* polyb = static_cast<ff::ff_t*>(std::malloc(plen));
  uint8_t* arra = static_cast<uint8_t*>(std::malloc(alen));
  uint8_t* arrb = static_cast<uint8_t*>(std::malloc(alen));

  for (size_t i = 0; i < ntt::N; i++) {
    polya[i] = ff::ff_t::random();
  }

  dilithium_utils::encode<sbw>(polya, arra);
  dilithium_utils::decode<sbw>(arra, polyb);
  dilithium_utils::encode<sbw>(polyb, arrb);

  constexpr size_t mask = (1u << sbw) - 1u;
  bool flg0 = false;
  bool flg1 = false;

  for (size_t i = 0; i < ntt::N; i++) {
    flg0 |= static_cast<bool>((polya[i].v & mask) ^ polyb[i].v);
  }

  for (size_t i = 0; i < alen; i++) {
    flg1 |= static_cast<bool>(arra[i] ^ arrb[i]);
  }

  const bool flg2 = !flg0 & !flg1;
  assert(flg2);

  std::free(polya);
  std::free(polyb);
  std::free(arra);
  std::free(arrb);
}

}
