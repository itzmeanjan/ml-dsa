#pragma once
#include "ntt.hpp"
#include "reduction.hpp"
#include "sampling.hpp"

// Utility functions for Dilithium Post-Quantum Digital Signature Algorithm
namespace dilithium_utils {

// Given a degree-255 polynomial over Z_q | q = 2^23 - 2^13 + 1, this routine
// attempts to extract out high and low order bits from each of 256 coefficients
template<const size_t d>
inline static void
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
inline static void
polymul(const ff::ff_t* const __restrict polya,
        const ff::ff_t* const __restrict polyb,
        ff::ff_t* const __restrict polyc)
{
  for (size_t i = 0; i < ntt::N; i++) {
    polyc[i] = polya[i] * polyb[i];
  }
}

// Given a degree-255 polynomial, which has all of its coefficients in [-η, η],
// this routine subtracts each coefficient from η, so that they stay in [0, 2η]
// range
template<const uint32_t eta>
inline static void
poly_sub_from_eta(ff::ff_t* const poly) requires(check_eta(eta))
{
  constexpr ff::ff_t eta_{ eta };

  for (size_t i = 0; i < ntt::N; i++) {
    poly[i] = eta_ - poly[i];
  }
}

}
