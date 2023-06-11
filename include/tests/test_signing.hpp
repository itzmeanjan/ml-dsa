#pragma once
#include "dilithium2.hpp"
#include "dilithium3.hpp"
#include "dilithium5.hpp"
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

// Test functional correctness of Dilithium signature scheme for parameters
// providing security equivalent to NIST security level 2, by generating random
// key pair, signing random message ( of mlen -bytes ) and finally attempting to
// verify using respective public key.
//
// In case when signature is not mutated ( the good case ), it should be able to
// verify successfully, while in the case when random bit flip is introduced
// in signature/ public key/ message ( the bad case ) verification algorithm
// must fail.
inline void
test_dilithium2_signing(const size_t mlen)
{
  constexpr size_t slen = 32;
  constexpr size_t pklen = dilithium2::PubKeyLen;
  constexpr size_t sklen = dilithium2::SecKeyLen;
  constexpr size_t siglen = dilithium2::SigLen;

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

  dilithium2::keygen(seed, pkey0, skey);
  dilithium2::sign(skey, msg0, mlen, sig0, nullptr);

  std::memcpy(sig1, sig0, siglen);
  std::memcpy(pkey1, pkey0, pklen);
  std::memcpy(msg1, msg0, mlen);

  random_bit_flip(sig1, siglen);
  random_bit_flip(pkey1, pklen);
  random_bit_flip(msg1, mlen);

  flg0 = dilithium2::verify(pkey0, msg0, mlen, sig0);
  flg1 = dilithium2::verify(pkey0, msg0, mlen, sig1);
  flg2 = dilithium2::verify(pkey1, msg0, mlen, sig0);
  flg3 = dilithium2::verify(pkey0, msg1, mlen, sig0);

  std::free(pkey0);
  std::free(pkey1);
  std::free(skey);
  std::free(sig0);
  std::free(sig1);
  std::free(msg0);
  std::free(msg1);

  assert(flg0 & !flg1 & !flg2 & !flg3);
}

// Test functional correctness of Dilithium signature scheme for parameters
// providing security equivalent to NIST security level 3, by generating random
// key pair, signing random message ( of mlen -bytes ) and finally attempting to
// verify using respective public key.
//
// In case when signature is not mutated ( the good case ), it should be able to
// verify successfully, while in the case when random bit flip is introduced
// in signature/ public key/ message ( the bad case ) verification algorithm
// must fail.
inline void
test_dilithium3_signing(const size_t mlen)
{
  constexpr size_t slen = 32;
  constexpr size_t pklen = dilithium3::PubKeyLen;
  constexpr size_t sklen = dilithium3::SecKeyLen;
  constexpr size_t siglen = dilithium3::SigLen;

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

  dilithium3::keygen(seed, pkey0, skey);
  dilithium3::sign(skey, msg0, mlen, sig0, nullptr);

  std::memcpy(sig1, sig0, siglen);
  std::memcpy(pkey1, pkey0, pklen);
  std::memcpy(msg1, msg0, mlen);

  random_bit_flip(sig1, siglen);
  random_bit_flip(pkey1, pklen);
  random_bit_flip(msg1, mlen);

  flg0 = dilithium3::verify(pkey0, msg0, mlen, sig0);
  flg1 = dilithium3::verify(pkey0, msg0, mlen, sig1);
  flg2 = dilithium3::verify(pkey1, msg0, mlen, sig0);
  flg3 = dilithium3::verify(pkey0, msg1, mlen, sig0);

  std::free(pkey0);
  std::free(pkey1);
  std::free(skey);
  std::free(sig0);
  std::free(sig1);
  std::free(msg0);
  std::free(msg1);

  assert(flg0 & !flg1 & !flg2 & !flg3);
}

// Test functional correctness of Dilithium signature scheme for parameters
// providing security equivalent to NIST security level 5, by generating random
// key pair, signing random message ( of mlen -bytes ) and finally attempting to
// verify using respective public key.
//
// In case when signature is not mutated ( the good case ), it should be able to
// verify successfully, while in the case when random bit flip is introduced
// in signature/ public key/ message ( the bad case ) verification algorithm
// must fail.
inline void
test_dilithium5_signing(const size_t mlen)
{
  constexpr size_t slen = 32;
  constexpr size_t pklen = dilithium5::PubKeyLen;
  constexpr size_t sklen = dilithium5::SecKeyLen;
  constexpr size_t siglen = dilithium5::SigLen;

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

  dilithium5::keygen(seed, pkey0, skey);
  dilithium5::sign(skey, msg0, mlen, sig0, nullptr);

  std::memcpy(sig1, sig0, siglen);
  std::memcpy(pkey1, pkey0, pklen);
  std::memcpy(msg1, msg0, mlen);

  random_bit_flip(sig1, siglen);
  random_bit_flip(pkey1, pklen);
  random_bit_flip(msg1, mlen);

  flg0 = dilithium5::verify(pkey0, msg0, mlen, sig0);
  flg1 = dilithium5::verify(pkey0, msg0, mlen, sig1);
  flg2 = dilithium5::verify(pkey1, msg0, mlen, sig0);
  flg3 = dilithium5::verify(pkey0, msg1, mlen, sig0);

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
