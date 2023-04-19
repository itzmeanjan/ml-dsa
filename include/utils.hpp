#pragma once
#include "params.hpp"
#include "reduction.hpp"
#include <iomanip>
#include <sstream>

// Utility functions for Dilithium Post-Quantum Digital Signature Algorithm
namespace dilithium_utils {

// Compile-time compute how many bytes to reserve for storing serialized
// Dilithium public key, for given parameter set
//
// See table 2 and section 5.4 of Dilithium specification
// https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf
template<const size_t k, const size_t d>
inline static constexpr size_t
pubkey_length()
  requires(dilithium_params::check_d(d))
{
  constexpr size_t t1_bw = std::bit_width(field::Q) - d;
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
seckey_length()
  requires(dilithium_params::check_d(d))
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
sig_length()
{
  constexpr size_t gamma1_bw = std::bit_width(γ1);
  constexpr size_t siglen = 32 + (32 * l * gamma1_bw) + (ω + k);
  return siglen;
}

// Given a bytearray of length N, this function converts it to human readable
// hex string of length N << 1 | N >= 0
inline const std::string
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
