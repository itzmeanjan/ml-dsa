#pragma once
#include "ntt.hpp"
#include "reduction.hpp"

// Utility functions for Dilithium Post-Quantum Digital Signature Algorithm
namespace dilithium_utils {

// Given a degree-255 polynomial over Z_q | q = 2^23 - 2^13 + 1, this routine
// attempts to extract out high and low order bits from each of 256 coefficients
template<const size_t d>
static void
poly_power2round(const ff::ff_t* const __restrict poly,
                 ff::ff_t* const __restrict poly_hi,
                 ff::ff_t* const __restrict poly_lo) requires(check_d(d))
{
  for (size_t i = 0; i < ntt::N; i++) {
    const auto ext = power2round<d>(poly[i]);

    poly_hi[i] = ext.first;
    poly_lo[i] = ext.second;
  }
}

// Given two degree-255 polynomials in NTT representation, this routine performs
// element-wise multiplication over Z_q | q = 2^23 - 2^13 + 1
static void
polymul(const ff::ff_t* const __restrict polya,
        const ff::ff_t* const __restrict polyb,
        ff::ff_t* const __restrict polyc)
{
  for (size_t i = 0; i < ntt::N; i++) {
    polyc[i] = polya[i] * polyb[i];
  }
}

}
