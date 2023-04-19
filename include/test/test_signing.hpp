#pragma once
#include "dilithium.hpp"
#include "prng.hpp"
#include <cassert>

// Test functional correctness of Dilithium PQC suite implementation
namespace test_dilithium {

// Given a byte array, this routine randomly selects a bit and flips it. This
// routine is used for generating faulty data during testing.
//
// Collects inspiration from
// https://github.com/itzmeanjan/gift-cofb/blob/0bd9baa/wrapper/python/test_gift_cofb.py#L79-L101
inline void
random_bit_flip(uint8_t* const arr, const size_t alen)
{
  std::random_device rd;
  std::mt19937_64 gen(rd());
  std::uniform_int_distribution<size_t> dis{ 0, alen - 1 };

  const size_t idx = dis(gen);
  const size_t bidx = dis(gen) & 7ul;

  const uint8_t mask0 = 0xff << (bidx + 1);
  const uint8_t mask1 = 0xff >> (8 - bidx);
  const uint8_t mask2 = 1 << bidx;

  const uint8_t msb = arr[idx] & mask0;
  const uint8_t lsb = arr[idx] & mask1;
  const uint8_t bit = (arr[idx] & mask2) >> bidx;

  arr[idx] = msb | ((1 - bit) << bidx) | lsb;
}

// Test functional correctness of Dilithium signature scheme by generating
// random key pair, signing random message ( of mlen -bytes ) and finally
// attempting to verify using respective public key.
//
// In case when signature is not mutated ( the good case ), it should be able to
// verify successfully, while in the case when random bit flip is introduced
// in signature/ public key/ message ( the bad case ) verification algorithm
// must fail.
template<const size_t k,
         const size_t l,
         const size_t d,
         const uint32_t η,
         const uint32_t γ1,
         const uint32_t γ2,
         const uint32_t τ,
         const uint32_t β,
         const size_t ω>
static void
test_signing(const size_t mlen)
{
  constexpr size_t slen = 32;
  constexpr size_t pklen = dilithium_utils::pubkey_length<k, d>();
  constexpr size_t sklen = dilithium_utils::seckey_length<k, l, η, d>();
  constexpr size_t siglen = dilithium_utils::signature_length<k, l, γ1, ω>();

  uint8_t* seed = static_cast<uint8_t*>(std::malloc(slen));
  uint8_t* pkey0 = static_cast<uint8_t*>(std::malloc(pklen));
  uint8_t* pkey1 = static_cast<uint8_t*>(std::malloc(pklen));
  uint8_t* skey = static_cast<uint8_t*>(std::malloc(sklen));
  uint8_t* sig0 = static_cast<uint8_t*>(std::malloc(siglen));
  uint8_t* sig1 = static_cast<uint8_t*>(std::malloc(siglen));
  uint8_t* msg0 = static_cast<uint8_t*>(std::malloc(mlen));
  uint8_t* msg1 = static_cast<uint8_t*>(std::malloc(mlen));

  prng::prng_t prng;

  prng.read(seed, slen);
  prng.read(msg0, mlen);

  bool flg0 = false, flg1 = false, flg2 = false, flg3 = false;

  dilithium::keygen<k, l, d, η>(seed, pkey0, skey);
  dilithium::sign<k, l, d, η, γ1, γ2, τ, β, ω>(skey, msg0, mlen, sig0);

  std::memcpy(sig1, sig0, siglen);
  std::memcpy(pkey1, pkey0, pklen);
  std::memcpy(msg1, msg0, mlen);

  random_bit_flip(sig1, siglen);
  random_bit_flip(pkey1, pklen);
  random_bit_flip(msg1, mlen);

  flg0 = dilithium::verify<k, l, d, γ1, γ2, τ, β, ω>(pkey0, msg0, mlen, sig0);
  flg1 = dilithium::verify<k, l, d, γ1, γ2, τ, β, ω>(pkey0, msg0, mlen, sig1);
  flg2 = dilithium::verify<k, l, d, γ1, γ2, τ, β, ω>(pkey1, msg0, mlen, sig0);
  flg3 = dilithium::verify<k, l, d, γ1, γ2, τ, β, ω>(pkey0, msg1, mlen, sig0);

  std::free(pkey0);
  std::free(pkey1);
  std::free(skey);
  std::free(sig0);
  std::free(sig1);
  std::free(msg0);
  std::free(msg1);

  assert(flg0 & !flg1 & !flg2 & !flg3);
}

}
