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

}
