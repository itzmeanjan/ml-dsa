#pragma once
#include "ntt.hpp"
#include "reduction.hpp"
#include <algorithm>

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

// Given a degree-255 polynomial, which has all of its coefficients in [-x, x],
// this routine subtracts each coefficient from x, so that they stay in [0, 2x]
// range
template<const uint32_t x>
inline static void
poly_sub_from_x(ff::ff_t* const poly)
{
  constexpr ff::ff_t x_cap{ x };

  for (size_t i = 0; i < ntt::N; i++) {
    poly[i] = x_cap - poly[i];
  }
}

// Given a degree-255 polynomial, this routine extracts out high order bits (
// using decompose routine ), while not mutating source polynomial
template<const uint32_t alpha>
inline static void
poly_highbits(const ff::ff_t* const __restrict polya,
              ff::ff_t* const __restrict polyb)
{
  for (size_t i = 0; i < ntt::N; i++) {
    polyb[i] = highbits<alpha>(polya[i]);
  }
}

// Given a degree-255 polynomial, this routine extracts out low order bits (
// using decompose routine ), while mutating source polynomial
template<const uint32_t alpha>
inline static void
poly_mut_lowbits(ff::ff_t* const __restrict poly)
{
  for (size_t i = 0; i < ntt::N; i++) {
    poly[i] = lowbits<alpha>(poly[i]);
  }
}

// Computes infinity norm of a degree-255 polynomial
//
// See point `Sizes of elements` in section 2.1 of Dilithium specification
// https://csrc.nist.gov/CSRC/media/Projects/post-quantum-cryptography/documents/round-3/submissions/Dilithium-Round3.zip
inline static ff::ff_t
poly_infinity_norm(const ff::ff_t* const __restrict poly)
{
  ff::ff_t res{ 0u };

  for (size_t i = 0; i < ntt::N; i++) {
    res = std::max(res, poly[i]);
  }

  return res;
}

}
