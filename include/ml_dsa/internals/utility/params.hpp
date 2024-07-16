#pragma once
#include "ml_dsa/internals/math/field.hpp"
#include <cstddef>
#include <cstdint>

// Compile-time executable functions, ensuring that ML-DSA routines are always invoked with proper arguments.
namespace ml_dsa_params {

// Compile-time check to ensure that *s*ignificant *b*it *w*idth (sbw) of Z_q element doesn't cross maximum bit width of
// field prime q ( = 2^23 - 2^13 + 1 ).
consteval bool
check_sbw(const size_t sbw)
{
  return sbw <= ml_dsa_field::Q_BIT_WIDTH;
}

// Compile-time check to ensure that η ∈ {2, 4}, so that sampled secret key range stays short i.e. [-η, η].
consteval bool
check_η(const uint32_t η)
{
  return (η == 2u) || (η == 4u);
}

// Compile-time check to ensure that starting nonce belongs to allowed set of values when uniform sampling polynomial
// coefficients in [-η, η].
consteval bool
check_nonce(const size_t nonce)
{
  return (nonce == 0) || (nonce == 4) || (nonce == 5) || (nonce == 7);
}

// Compile-time check to ensure that γ1 has recommended value.
consteval bool
check_γ1(const uint32_t γ1)
{
  return (γ1 == (1u << 17)) || (γ1 == (1u << 19));
}

// Compile-time check to ensure that γ2 has recommended value.
consteval bool
check_γ2(const uint32_t γ2)
{
  return (γ2 == ((ml_dsa_field::Q - 1) / 88)) || (γ2 == ((ml_dsa_field::Q - 1) / 32));
}

// Compile-time check to ensure that τ is set to parameter recommended in ML-DSA specification.
consteval bool
check_τ(const uint32_t τ)
{
  return (τ == 39) || (τ == 49) || (τ == 60);
}

// Compile-time check to ensure that number of bits to be dropped from a polynomial coefficient is supplied correctly.
consteval bool
check_d(const size_t d)
{
  return d == 13;
}

// Compile-time check to ensure that operand matrices are having compatible dimension for matrix multiplication.
consteval bool
check_matrix_dim(const size_t a_cols, const size_t b_rows)
{
  return !static_cast<bool>(a_cols ^ b_rows);
}

// Compile-time executable constraints for ensuring that ML-DSA key generation algorithm is only invoked with arguments
// suggested in table 1 of ML-DSA draft standard https://doi.org/10.6028/NIST.FIPS.204.ipd.
consteval bool
check_keygen_params(const size_t k, const size_t l, const size_t d, const uint32_t η)
{
  return ((k == 4) && (l == 4) && (d == 13) && (η == 2)) || // ML-DSA-44
         ((k == 6) && (l == 5) && (d == 13) && (η == 4)) || // ML-DSA-65
         ((k == 8) && (l == 7) && (d == 13) && (η == 2));   // ML-DSA-87
}

// Compile-time executable constraints for ensuring that ML-DSA signing algorithm is only invoked with arguments
// suggested in table 1 of ML-DSA draft standard https://doi.org/10.6028/NIST.FIPS.204.ipd.
consteval bool
check_signing_params(const size_t k,
                     const size_t l,
                     const size_t d,
                     const uint32_t η,
                     const uint32_t γ1,
                     const uint32_t γ2,
                     const uint32_t τ,
                     const uint32_t β,
                     const size_t ω,
                     const size_t λ)
{
  return ((k == 4) && (l == 4) && (d == 13) && (η == 2) && (γ1 == (1u << 17)) && (γ2 == ((ml_dsa_field::Q - 1) / 88)) &&
          (τ == 39) && (β == τ * η) && (ω == 80) && (λ == 128)) || // ML-DSA-44
         ((k == 6) && (l == 5) && (d == 13) && (η == 4) && (γ1 == (1u << 19)) && (γ2 == ((ml_dsa_field::Q - 1) / 32)) &&
          (τ == 49) && (β == τ * η) && (ω == 55) && (λ == 192)) || // ML-DSA-65
         ((k == 8) && (l == 7) && (d == 13) && (η == 2) && (γ1 == (1u << 19)) && (γ2 == ((ml_dsa_field::Q - 1) / 32)) &&
          (τ == 60) && (β == τ * η) && (ω == 75) && (λ == 256)); // ML-DSA-87
}

// Compile-time executable constraints for ensuring that ML-DSA verification algorithm is only invoked with arguments
// suggested in table 1 of ML-DSA draft standard https://doi.org/10.6028/NIST.FIPS.204.ipd.
consteval bool
check_verify_params(const size_t k,
                    const size_t l,
                    const size_t d,
                    const uint32_t γ1,
                    const uint32_t γ2,
                    const uint32_t τ,
                    const uint32_t β,
                    const size_t ω,
                    const size_t λ)
{
  return ((k == 4) && (l == 4) && (d == 13) && (γ1 == (1u << 17)) && (γ2 == ((ml_dsa_field::Q - 1) / 88)) &&
          (τ == 39) && (β == τ * 2) && (ω == 80) && (λ == 128)) || // ML-DSA-44
         ((k == 6) && (l == 5) && (d == 13) && (γ1 == (1u << 19)) && (γ2 == ((ml_dsa_field::Q - 1) / 32)) &&
          (τ == 49) && (β == τ * 4) && (ω == 55) && (λ == 192)) || // ML-DSA-65
         ((k == 8) && (l == 7) && (d == 13) && (γ1 == (1u << 19)) && (γ2 == ((ml_dsa_field::Q - 1) / 32)) &&
          (τ == 60) && (β == τ * 2) && (ω == 75) && (λ == 256)); // ML-DSA-87
}

}
