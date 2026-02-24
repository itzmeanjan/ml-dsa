#pragma once
#include "ml_dsa/internals/math/field.hpp"
#include "ml_dsa/internals/utility/force_inline.hpp"
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>

// Utility functions for ML-DSA
namespace ml_dsa_utils {

// Internal byte lengths for ML-DSA key/hash components, see FIPS 204 Table 2.
static constexpr size_t RHO_BYTE_LEN = 32;
static constexpr size_t RHO_PRIME_BYTE_LEN = 64;
static constexpr size_t KEY_BYTE_LEN = 32;
static constexpr size_t TR_BYTE_LEN = 64;

// Securely zeroizes a std::array, preventing the compiler from optimizing away the operation.
// At compile-time: a plain fill suffices -- there is no real memory to protect.
// At runtime: the fill is followed by a compiler barrier (empty asm with "memory" clobber)
// that forces the compiler to treat the array's memory as externally observable, preventing
// dead store elimination. The barrier generates zero instructions.
template<typename T, size_t N>
forceinline constexpr void
secure_zeroize(std::array<T, N>& arr)
{
  arr.fill(T{});
  if (!std::is_constant_evaluated()) {
    asm volatile("" : : "r"(arr.data()) : "memory"); // NOLINT(hicpp-no-assembler)
  }
}

// Compile-time compute how many bytes to reserve for storing serialized ML-DSA public key, for given parameter set.
//
// See table 1, 2 of ML-DSA standard https://doi.org/10.6028/NIST.FIPS.204.
static constexpr size_t
pub_key_len(const size_t k, const size_t d)
{
  const size_t t1_bw = ml_dsa_field::Q_BIT_WIDTH - d;
  const size_t pklen = RHO_BYTE_LEN + (k * 32 * t1_bw);
  return pklen;
}

// Compile-time compute how many bytes to reserve for storing serialized ML-DSA secret key, for given parameter set.
//
// See table 1, 2 of ML-DSA standard https://doi.org/10.6028/NIST.FIPS.204.
static constexpr size_t
sec_key_len(const size_t k, const size_t l, const uint32_t eta, const size_t d)
{
  const size_t eta_bw = static_cast<size_t>(std::bit_width(2 * eta));
  const size_t sklen = RHO_BYTE_LEN + KEY_BYTE_LEN + TR_BYTE_LEN + (32 * ((eta_bw * (k + l)) + (k * d)));
  return sklen;
}

// Compile-time compute how many bytes to reserve for storing serialized ML-DSA signature, for specified parameter set.
//
// See table 1, 2 of ML-DSA standard https://doi.org/10.6028/NIST.FIPS.204.
static constexpr size_t
sig_len(const size_t k, const size_t l, const uint32_t gamma1, const size_t omega, const size_t lambda)
{
  const size_t gamma1_bw = static_cast<size_t>(std::bit_width(gamma1));
  const size_t siglen = ((2 * lambda) / std::numeric_limits<uint8_t>::digits) + (32 * l * gamma1_bw) + (omega + k);
  return siglen;
}

}
