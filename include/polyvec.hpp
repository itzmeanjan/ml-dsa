#pragma once
#include "poly.hpp"

// Utility functions for Dilithium Post-Quantum Digital Signature Algorithm
namespace dilithium_utils {

// Applies NTT on a vector ( of dimension k x 1 ) of degree-255 polynomials
template<const size_t k>
static void
polyvec_ntt(ff::ff_t* const __restrict vec)
{
  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ntt::N;
    ntt::ntt(vec + off);
  }
}

// Applies iNTT on a vector ( of dimension k x 1 ) of degree-255 polynomials
template<const size_t k>
static void
polyvec_intt(ff::ff_t* const __restrict vec)
{
  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ntt::N;
    ntt::intt(vec + off);
  }
}

// Compresses vector ( of dimension k x 1 ) of degree-255 polynomials by
// extracting out high and low order bits
template<const size_t k, const size_t d>
static void
polyvec_power2round(const ff::ff_t* const __restrict poly,
                    ff::ff_t* const __restrict poly_hi,
                    ff::ff_t* const __restrict poly_lo) requires(check_d(d))
{
  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ntt::N;
    poly_power2round<d>(poly + off, poly_hi + off, poly_lo + off);
  }
}

// Compile-time check to ensure that operand matrices are having compatible
// dimension for matrix multiplication
static inline constexpr bool
check_matrix_dim(const size_t a_cols, const size_t b_rows)
{
  return !static_cast<bool>(a_cols ^ b_rows);
}

// Given two matrices ( in NTT domain ) of compatible dimension, where each
// matrix element is a degree-255 polynomial over Z_q | q = 2^23 -2^13 + 1, this
// routine attempts to multiply and compute resulting matrix
template<const size_t a_rows,
         const size_t a_cols,
         const size_t b_rows,
         const size_t b_cols>
static void
matrix_multiply(const ff::ff_t* const __restrict a,
                const ff::ff_t* const __restrict b,
                ff::ff_t* const __restrict c) requires(check_matrix_dim(a_cols,
                                                                        b_rows))
{
  ff::ff_t tmp[ntt::N]{};

  for (size_t i = 0; i < a_rows; i++) {
    for (size_t j = 0; j < b_cols; j++) {
      const size_t coff = (i * b_cols + j) * ntt::N;

      for (size_t k = 0; k < a_cols; k++) {
        const size_t aoff = (i * a_cols + k) * ntt::N;
        const size_t boff = (k * b_cols + j) * ntt::N;

        polymul(a + coff, b + boff, tmp);

        for (size_t l = 0; l < ntt::N; l++) {
          c[coff + l] = c[coff + l] + tmp[l];
        }
      }
    }
  }
}

}
