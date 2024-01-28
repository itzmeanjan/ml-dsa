#pragma once
#include "params.hpp"
#include "reduction.hpp"
#include <cassert>
#include <charconv>
#include <iomanip>
#include <sstream>
#include <vector>

// Utility functions for Dilithium Post-Quantum Digital Signature Algorithm
namespace dilithium_utils {

// Compile-time compute how many bytes to reserve for storing serialized
// Dilithium public key, for given parameter set
//
// See table 2 and section 5.4 of Dilithium specification
// https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf
template<size_t k, size_t d>
static inline constexpr size_t
pub_key_len()
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
template<size_t k, size_t l, uint32_t η, size_t d>
static inline constexpr size_t
sec_key_len()
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
template<size_t k, size_t l, uint32_t γ1, size_t ω>
static inline constexpr size_t
sig_len()
{
  constexpr size_t gamma1_bw = std::bit_width(γ1);
  constexpr size_t siglen = 32 + (32 * l * gamma1_bw) + (ω + k);
  return siglen;
}

// Given a bytearray of length N, this function converts it to human readable
// hex string of length N << 1 | N >= 0
static inline const std::string
to_hex(std::span<const uint8_t> bytes)
{
  std::stringstream ss;
  ss << std::hex;

  for (size_t i = 0; i < bytes.size(); i++) {
    ss << std::setw(2) << std::setfill('0') << static_cast<uint32_t>(bytes[i]);
  }

  return ss.str();
}

// Given a hex encoded string of length 2*L, this routine can be used for
// parsing it as a byte array of length L.
static inline std::vector<uint8_t>
from_hex(std::string_view hex)
{
  const size_t hlen = hex.length();
  assert(hlen % 2 == 0);

  const size_t blen = hlen / 2;
  std::vector<uint8_t> res(blen, 0);

  for (size_t i = 0; i < blen; i++) {
    const size_t off = i * 2;

    uint8_t byte = 0;
    auto sstr = hex.substr(off, 2);
    std::from_chars(sstr.data(), sstr.data() + 2, byte, 16);

    res[i] = byte;
  }

  return res;
}

}
