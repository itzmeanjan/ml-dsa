#include "dilithium2.hpp"
#include "dilithium3.hpp"
#include "dilithium5.hpp"
#include <gtest/gtest.h>

// Given a byte array, this routine randomly selects a bit and flips it. This
// routine is used for generating faulty data during testing.
//
// Collects inspiration from
// https://github.com/itzmeanjan/gift-cofb/blob/0bd9baa/wrapper/python/test_gift_cofb.py#L79-L101
static inline void
random_bit_flip(std::span<uint8_t> arr)
{
  std::random_device rd;
  std::mt19937_64 gen(rd());
  std::uniform_int_distribution<size_t> dis{ 0, arr.size() - 1 };

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

  std::vector<uint8_t> seed(slen, 0);
  std::vector<uint8_t> pkey0(pklen, 0);
  std::vector<uint8_t> pkey1(pklen, 0);
  std::vector<uint8_t> skey(sklen, 0);
  std::vector<uint8_t> sig0(siglen, 0);
  std::vector<uint8_t> sig1(siglen, 0);
  std::vector<uint8_t> msg0(mlen, 0);
  std::vector<uint8_t> msg1(mlen, 0);

  auto _seed = std::span<uint8_t, slen>(seed);
  auto _pkey0 = std::span<uint8_t, pklen>(pkey0);
  auto _pkey1 = std::span<uint8_t, pklen>(pkey1);
  auto _skey = std::span<uint8_t, sklen>(skey);
  auto _sig0 = std::span<uint8_t, siglen>(sig0);
  auto _sig1 = std::span<uint8_t, siglen>(sig1);
  auto _msg0 = std::span(msg0);
  auto _msg1 = std::span(msg1);

  prng::prng_t prng;

  prng.read(_seed);
  prng.read(_msg0);

  bool flg0 = false, flg1 = false, flg2 = false, flg3 = false;

  dilithium2::keygen(_seed, _pkey0, _skey);
  dilithium2::sign(_skey, _msg0, _sig0, {});

  std::copy(_sig0.begin(), _sig0.end(), _sig1.begin());
  std::copy(_pkey0.begin(), _pkey0.end(), _pkey1.begin());
  std::copy(_msg0.begin(), _msg0.end(), _msg1.begin());

  random_bit_flip(_sig1);
  random_bit_flip(_pkey1);
  random_bit_flip(_msg1);

  flg0 = dilithium2::verify(_pkey0, _msg0, _sig0);
  flg1 = dilithium2::verify(_pkey0, _msg0, _sig1);
  flg2 = dilithium2::verify(_pkey1, _msg0, _sig0);
  flg3 = dilithium2::verify(_pkey0, _msg1, _sig0);

  EXPECT_TRUE(flg0 & !flg1 & !flg2 & !flg3);
}

TEST(Dilithium, Dilithium2KeygenSignVerifyFlow)
{
  for (size_t mlen = 1; mlen < 33; mlen++) {
    test_dilithium2_signing(mlen);
  }
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

  std::vector<uint8_t> seed(slen, 0);
  std::vector<uint8_t> pkey0(pklen, 0);
  std::vector<uint8_t> pkey1(pklen, 0);
  std::vector<uint8_t> skey(sklen, 0);
  std::vector<uint8_t> sig0(siglen, 0);
  std::vector<uint8_t> sig1(siglen, 0);
  std::vector<uint8_t> msg0(mlen, 0);
  std::vector<uint8_t> msg1(mlen, 0);

  auto _seed = std::span<uint8_t, slen>(seed);
  auto _pkey0 = std::span<uint8_t, pklen>(pkey0);
  auto _pkey1 = std::span<uint8_t, pklen>(pkey1);
  auto _skey = std::span<uint8_t, sklen>(skey);
  auto _sig0 = std::span<uint8_t, siglen>(sig0);
  auto _sig1 = std::span<uint8_t, siglen>(sig1);
  auto _msg0 = std::span(msg0);
  auto _msg1 = std::span(msg1);

  prng::prng_t prng;

  prng.read(_seed);
  prng.read(_msg0);

  bool flg0 = false, flg1 = false, flg2 = false, flg3 = false;

  dilithium3::keygen(_seed, _pkey0, _skey);
  dilithium3::sign(_skey, _msg0, _sig0, {});

  std::copy(_sig0.begin(), _sig0.end(), _sig1.begin());
  std::copy(_pkey0.begin(), _pkey0.end(), _pkey1.begin());
  std::copy(_msg0.begin(), _msg0.end(), _msg1.begin());

  random_bit_flip(_sig1);
  random_bit_flip(_pkey1);
  random_bit_flip(_msg1);

  flg0 = dilithium3::verify(_pkey0, _msg0, _sig0);
  flg1 = dilithium3::verify(_pkey0, _msg0, _sig1);
  flg2 = dilithium3::verify(_pkey1, _msg0, _sig0);
  flg3 = dilithium3::verify(_pkey0, _msg1, _sig0);

  EXPECT_TRUE(flg0 & !flg1 & !flg2 & !flg3);
}

TEST(Dilithium, Dilithium3KeygenSignVerifyFlow)
{
  for (size_t mlen = 1; mlen < 33; mlen++) {
    test_dilithium3_signing(mlen);
  }
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

  std::vector<uint8_t> seed(slen, 0);
  std::vector<uint8_t> pkey0(pklen, 0);
  std::vector<uint8_t> pkey1(pklen, 0);
  std::vector<uint8_t> skey(sklen, 0);
  std::vector<uint8_t> sig0(siglen, 0);
  std::vector<uint8_t> sig1(siglen, 0);
  std::vector<uint8_t> msg0(mlen, 0);
  std::vector<uint8_t> msg1(mlen, 0);

  auto _seed = std::span<uint8_t, slen>(seed);
  auto _pkey0 = std::span<uint8_t, pklen>(pkey0);
  auto _pkey1 = std::span<uint8_t, pklen>(pkey1);
  auto _skey = std::span<uint8_t, sklen>(skey);
  auto _sig0 = std::span<uint8_t, siglen>(sig0);
  auto _sig1 = std::span<uint8_t, siglen>(sig1);
  auto _msg0 = std::span(msg0);
  auto _msg1 = std::span(msg1);

  prng::prng_t prng;

  prng.read(_seed);
  prng.read(_msg0);

  bool flg0 = false, flg1 = false, flg2 = false, flg3 = false;

  dilithium5::keygen(_seed, _pkey0, _skey);
  dilithium5::sign(_skey, _msg0, _sig0, {});

  std::copy(_sig0.begin(), _sig0.end(), _sig1.begin());
  std::copy(_pkey0.begin(), _pkey0.end(), _pkey1.begin());
  std::copy(_msg0.begin(), _msg0.end(), _msg1.begin());

  random_bit_flip(_sig1);
  random_bit_flip(_pkey1);
  random_bit_flip(_msg1);

  flg0 = dilithium5::verify(_pkey0, _msg0, _sig0);
  flg1 = dilithium5::verify(_pkey0, _msg0, _sig1);
  flg2 = dilithium5::verify(_pkey1, _msg0, _sig0);
  flg3 = dilithium5::verify(_pkey0, _msg1, _sig0);

  EXPECT_TRUE(flg0 & !flg1 & !flg2 & !flg3);
}

TEST(Dilithium, Dilithium5KeygenSignVerifyFlow)
{
  for (size_t mlen = 1; mlen < 33; mlen++) {
    test_dilithium5_signing(mlen);
  }
}
