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
                 ff::ff_t* const __restrict poly_lo)
  requires(check_d(d))
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
// using decompose routine ), while not mutating source polynomial
template<const uint32_t alpha>
inline static void
poly_lowbits(const ff::ff_t* const __restrict src,
             ff::ff_t* const __restrict dst)
{
  for (size_t i = 0; i < ntt::N; i++) {
    dst[i] = lowbits<alpha>(src[i]);
  }
}

// Computes infinity norm of a degree-255 polynomial
//
// See point `Sizes of elements` in section 2.1 of Dilithium specification
// https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf
inline static ff::ff_t
poly_infinity_norm(const ff::ff_t* const __restrict poly)
{
  constexpr ff::ff_t qby2{ ff::Q >> 1 };
  ff::ff_t res{ 0u };

  for (size_t i = 0; i < ntt::N; i++) {
    const bool flg = poly[i] > qby2;
    const ff::ff_t br[]{ poly[i], -poly[i] };

    res = std::max(res, br[flg]);
  }

  return res;
}

// Given two degree-255 polynomials, this routine computes hint bit for each
// coefficient, using `make_hint` routine.
template<const uint32_t alpha>
inline static void
poly_make_hint(const ff::ff_t* const __restrict polya,
               const ff::ff_t* const __restrict polyb,
               ff::ff_t* const __restrict polyc)
{
  for (size_t i = 0; i < ntt::N; i++) {
    polyc[i] = make_hint<alpha>(polya[i], polyb[i]);
  }
}

// Given a hint bit polynomial ( of degree-255 ) and another degree-255
// polynomial r with arbitrary coefficients ∈ Z_q, this routine recovers high
// order bits of r + z s.t. hint bit was computed using `make_hint` routine and
// z is another degree-255 polynomial with small coefficients.
template<const uint32_t alpha>
inline static void
poly_use_hint(const ff::ff_t* const __restrict polyh,
              const ff::ff_t* const __restrict polyr,
              ff::ff_t* const __restrict polyrz)
{
  for (size_t i = 0; i < ntt::N; i++) {
    polyrz[i] = use_hint<alpha>(polyh[i], polyr[i]);
  }
}

// Given a degree-255 polynomial, this routine counts number of coefficients
// having value 1.
inline static size_t
count_1s(const ff::ff_t* const __restrict poly)
{
  constexpr ff::ff_t one{ 1u };
  size_t cnt = 0;

  for (size_t i = 0; i < ntt::N; i++) {
    cnt += 1 * (poly[i] == one);
  }

  return cnt;
}

// Given a degree-255 polynomial, this routine shifts each coefficient
// leftwards, by d bits
template<const size_t d>
inline static void
poly_shl(ff::ff_t* const __restrict poly)
{
  for (size_t i = 0; i < ntt::N; i++) {
    poly[i] = poly[i] << d;
  }
}

}
