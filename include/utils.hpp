#pragma once
#include "reduction.hpp"

// Utility functions for Dilithium Post-Quantum Digital Signature Algorithm
namespace dilithium_utils {

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

}
