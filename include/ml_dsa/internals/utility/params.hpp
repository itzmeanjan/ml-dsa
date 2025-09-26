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

// Compile-time check to ensure that eta ∈ {2, 4}, so that sampled secret key range stays short i.e. [-eta, eta].
consteval bool
check_eta(const uint32_t eta)
{
  return (eta == 2u) || (eta == 4u);
}

// Compile-time check to ensure that starting nonce belongs to allowed set of values when uniform sampling polynomial
// coefficients in [-eta, eta].
consteval bool
check_nonce(const size_t nonce)
{
  return (nonce == 0) || (nonce == 4) || (nonce == 5) || (nonce == 7);
}

// Compile-time check to ensure that gamma1 has recommended value.
consteval bool
check_gamma1(const uint32_t gamma1)
{
  return (gamma1 == (1u << 17)) || (gamma1 == (1u << 19));
}

// Compile-time check to ensure that gamma2 has recommended value.
consteval bool
check_gamma2(const uint32_t gamma2)
{
  return (gamma2 == ((ml_dsa_field::Q - 1) / 88)) || (gamma2 == ((ml_dsa_field::Q - 1) / 32));
}

// Compile-time check to ensure that tau is set to parameter recommended in ML-DSA specification.
consteval bool
check_tau(const uint32_t tau)
{
  return (tau == 39) || (tau == 49) || (tau == 60);
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
// suggested in table 1 of ML-DSA standard https://doi.org/10.6028/NIST.FIPS.204.
consteval bool
check_keygen_params(const size_t k, const size_t l, const size_t d, const uint32_t eta)
{
  return ((k == 4) && (l == 4) && (d == 13) && (eta == 2)) || // ML-DSA-44
         ((k == 6) && (l == 5) && (d == 13) && (eta == 4)) || // ML-DSA-65
         ((k == 8) && (l == 7) && (d == 13) && (eta == 2));   // ML-DSA-87
}

// Compile-time executable constraints for ensuring that ML-DSA signing algorithm is only invoked with arguments
// suggested in table 1 of ML-DSA standard https://doi.org/10.6028/NIST.FIPS.204.
consteval bool
check_signing_params(const size_t k,
                     const size_t l,
                     const size_t d,
                     const uint32_t eta,
                     const uint32_t gamma1,
                     const uint32_t gamma2,
                     const uint32_t tau,
                     const uint32_t beta,
                     const size_t omega,
                     const size_t lambda)
{
  return ((k == 4) && (l == 4) && (d == 13) && (eta == 2) && (gamma1 == (1u << 17)) && (gamma2 == ((ml_dsa_field::Q - 1) / 88)) && (tau == 39) && (beta == tau * eta) &&
          (omega == 80) && (lambda == 128)) || // ML-DSA-44
         ((k == 6) && (l == 5) && (d == 13) && (eta == 4) && (gamma1 == (1u << 19)) && (gamma2 == ((ml_dsa_field::Q - 1) / 32)) && (tau == 49) && (beta == tau * eta) &&
          (omega == 55) && (lambda == 192)) || // ML-DSA-65
         ((k == 8) && (l == 7) && (d == 13) && (eta == 2) && (gamma1 == (1u << 19)) && (gamma2 == ((ml_dsa_field::Q - 1) / 32)) && (tau == 60) && (beta == tau * eta) &&
          (omega == 75) && (lambda == 256)); // ML-DSA-87
}

// Compile-time executable constraints for ensuring that ML-DSA verification algorithm is only invoked with arguments
// suggested in table 1 of ML-DSA standard https://doi.org/10.6028/NIST.FIPS.204.
consteval bool
check_verify_params(const size_t k,
                    const size_t l,
                    const size_t d,
                    const uint32_t gamma1,
                    const uint32_t gamma2,
                    const uint32_t tau,
                    const uint32_t beta,
                    const size_t omega,
                    const size_t lambda)
{
  return ((k == 4) && (l == 4) && (d == 13) && (gamma1 == (1u << 17)) && (gamma2 == ((ml_dsa_field::Q - 1) / 88)) && (tau == 39) && (beta == tau * 2) && (omega == 80) &&
          (lambda == 128)) || // ML-DSA-44
         ((k == 6) && (l == 5) && (d == 13) && (gamma1 == (1u << 19)) && (gamma2 == ((ml_dsa_field::Q - 1) / 32)) && (tau == 49) && (beta == tau * 4) && (omega == 55) &&
          (lambda == 192)) || // ML-DSA-65
         ((k == 8) && (l == 7) && (d == 13) && (gamma1 == (1u << 19)) && (gamma2 == ((ml_dsa_field::Q - 1) / 32)) && (tau == 60) && (beta == tau * 2) && (omega == 75) &&
          (lambda == 256)); // ML-DSA-87
}

}
