#pragma once
#include "reduction.hpp"
#include <iomanip>
#include <sstream>

// Utility functions for Dilithium Post-Quantum Digital Signature Algorithm
namespace dilithium_utils {

// Generates N -many random values of type T | N >= 0
template<typename T>
static inline void
random_data(T* const data, const size_t len) requires(std::is_unsigned_v<T>)
{
  std::random_device rd;
  std::mt19937_64 gen(rd());
  std::uniform_int_distribution<T> dis;

  for (size_t i = 0; i < len; i++) {
    data[i] = dis(gen);
  }
}

// Compile-time compute how many bytes to reserve for storing serialized
// Dilithium public key, for given parameter set
//
// See table 2 and section 5.4 of Dilithium specification
// https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf
template<const size_t k, const size_t d>
inline static constexpr size_t
pubkey_length() requires(check_d(d))
{
  constexpr size_t t1_bw = std::bit_width(ff::Q) - d;
  constexpr size_t pklen = 32 + k * 32 * t1_bw;
  return pklen;
}

// Compile-time compute how many bytes to reserve for storing serialized
// Dilithium secret key, for given parameter set
//
// See table 2 and section 5.4 of Dilithium specification
// https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf
template<const size_t k, const size_t l, const uint32_t η, const size_t d>
inline static constexpr size_t
seckey_length() requires(check_d(d))
{
  constexpr size_t eta_bw = std::bit_width(2 * η);
  constexpr size_t sklen = 32 + 32 + 32 + 32 * (eta_bw * (k + l) + k * d);
  return sklen;
}

// Compile-time compute how many bytes to reserve for storing serialized
// Dilithium signature, for specified parameter set
//
// See table 2 and section 5.4 of Dilithium specification
// https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf
template<const size_t k, const size_t l, const uint32_t γ1, const size_t ω>
inline static constexpr size_t
signature_length()
{
  constexpr size_t gamma1_bw = std::bit_width(γ1);
  constexpr size_t siglen = 32 + (32 * l * gamma1_bw) + (ω + k);
  return siglen;
}

// Given a byte array, this routine randomly selects a bit and flips it. This
// routine is used for generating faulty data during testing.
//
// Collects inspiration from
// https://github.com/itzmeanjan/gift-cofb/blob/0bd9baa/wrapper/python/test_gift_cofb.py#L79-L101
static void
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

// Given a bytearray of length N, this function converts it to human readable
// hex string of length N << 1 | N >= 0
static inline const std::string
to_hex(const uint8_t* const bytes, const size_t len)
{
  std::stringstream ss;
  ss << std::hex;

  for (size_t i = 0; i < len; i++) {
    ss << std::setw(2) << std::setfill('0') << static_cast<uint32_t>(bytes[i]);
  }

  return ss.str();
}

}
