#pragma once
#include "ml_dsa/internals/math/field.hpp"

// Utility functions for ML-DSA
namespace ml_dsa_utils {

// Compile-time compute how many bytes to reserve for storing serialized ML-DSA public key, for given parameter set.
//
// See table 1, 2 of ML-DSA standard https://doi.org/10.6028/NIST.FIPS.204.
static inline constexpr size_t
pub_key_len(const size_t k, const size_t d)
{
  const size_t t1_bw = ml_dsa_field::Q_BIT_WIDTH - d;
  const size_t pklen = 32 + k * 32 * t1_bw;
  return pklen;
}

// Compile-time compute how many bytes to reserve for storing serialized ML-DSA secret key, for given parameter set.
//
// See table 1, 2 of ML-DSA standard https://doi.org/10.6028/NIST.FIPS.204.
static inline constexpr size_t
sec_key_len(const size_t k, const size_t l, const uint32_t η, const size_t d)
{
  const size_t eta_bw = std::bit_width(2 * η);
  const size_t sklen = 32 + 32 + 64 + 32 * (eta_bw * (k + l) + k * d);
  return sklen;
}

// Compile-time compute how many bytes to reserve for storing serialized ML-DSA signature, for specified parameter set.
//
// See table 1, 2 of ML-DSA standard https://doi.org/10.6028/NIST.FIPS.204.
static inline constexpr size_t
sig_len(const size_t k, const size_t l, const uint32_t γ1, const size_t ω, const size_t λ)
{
  const size_t gamma1_bw = std::bit_width(γ1);
  const size_t siglen = ((2 * λ) / std::numeric_limits<uint8_t>::digits) + (32 * l * gamma1_bw) + (ω + k);
  return siglen;
}

}
