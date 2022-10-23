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
// See table 2 of Dilithium specification
// https://csrc.nist.gov/CSRC/media/Projects/post-quantum-cryptography/documents/round-3/submissions/Dilithium-Round3.zip
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
// See table 2 of Dilithium specification
// https://csrc.nist.gov/CSRC/media/Projects/post-quantum-cryptography/documents/round-3/submissions/Dilithium-Round3.zip
template<const size_t k, const size_t l, const uint32_t eta, const size_t d>
inline static constexpr size_t
seckey_length() requires(check_d(d))
{
  constexpr size_t eta_bw = std::bit_width(2 * eta);
  constexpr size_t sklen = 32 + 32 + 48 + 32 * (eta_bw * (k + l) + k * d);
  return sklen;
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
