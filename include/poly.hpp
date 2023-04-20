#pragma once
#include "ntt.hpp"
#include "params.hpp"
#include "reduction.hpp"
#include <algorithm>

// Degree-255 polynomial utility functions for Dilithium Post-Quantum Digital
// Signature Algorithm
namespace poly {

// Given a degree-255 polynomial over Z_q | q = 2^23 - 2^13 + 1, this routine
// attempts to extract out high and low order bits from each of 256 coefficients
template<const size_t d>
static inline void
power2round(const field::zq_t* const __restrict poly,
            field::zq_t* const __restrict poly_hi,
            field::zq_t* const __restrict poly_lo)
  requires(dilithium_params::check_d(d))
{
  for (size_t i = 0; i < ntt::N; i++) {
    const auto ext = reduction::power2round<d>(poly[i]);

    poly_hi[i] = ext.first;
    poly_lo[i] = ext.second;
  }
}

// Given two degree-255 polynomials in NTT representation, this routine performs
// element-wise multiplication over Z_q | q = 2^23 - 2^13 + 1
inline void
mul(const field::zq_t* const __restrict polya,
    const field::zq_t* const __restrict polyb,
    field::zq_t* const __restrict polyc)
{
  for (size_t i = 0; i < ntt::N; i++) {
    polyc[i] = polya[i] * polyb[i];
  }
}

// Given a degree-255 polynomial, which has all of its coefficients in [-x, x],
// this routine subtracts each coefficient from x, so that they stay in [0, 2x]
// range
template<const uint32_t x>
static inline void
sub_from_x(field::zq_t* const poly)
{
  constexpr field::zq_t x_cap{ x };

  for (size_t i = 0; i < ntt::N; i++) {
    poly[i] = x_cap - poly[i];
  }
}

// Given a degree-255 polynomial, this routine extracts out high order bits (
// using decompose routine ), while not mutating source polynomial
template<const uint32_t alpha>
static inline void
highbits(const field::zq_t* const __restrict polya,
         field::zq_t* const __restrict polyb)
{
  for (size_t i = 0; i < ntt::N; i++) {
    polyb[i] = reduction::highbits<alpha>(polya[i]);
  }
}

// Given a degree-255 polynomial, this routine extracts out low order bits (
// using decompose routine ), while not mutating source polynomial
template<const uint32_t alpha>
static inline void
lowbits(const field::zq_t* const __restrict src,
        field::zq_t* const __restrict dst)
{
  for (size_t i = 0; i < ntt::N; i++) {
    dst[i] = reduction::lowbits<alpha>(src[i]);
  }
}

// Computes infinity norm of a degree-255 polynomial
//
// See point `Sizes of elements` in section 2.1 of Dilithium specification
// https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf
inline field::zq_t
infinity_norm(const field::zq_t* const __restrict poly)
{
  constexpr field::zq_t qby2{ field::Q >> 1 };
  field::zq_t res{ 0u };

  for (size_t i = 0; i < ntt::N; i++) {
    const bool flg = poly[i] > qby2;
    const field::zq_t br[]{ poly[i], -poly[i] };

    res = std::max(res, br[flg]);
  }

  return res;
}

// Given two degree-255 polynomials, this routine computes hint bit for each
// coefficient, using `make_hint` routine.
template<const uint32_t alpha>
static inline void
make_hint(const field::zq_t* const __restrict polya,
          const field::zq_t* const __restrict polyb,
          field::zq_t* const __restrict polyc)
{
  for (size_t i = 0; i < ntt::N; i++) {
    polyc[i] = reduction::make_hint<alpha>(polya[i], polyb[i]);
  }
}

// Given a hint bit polynomial ( of degree-255 ) and another degree-255
// polynomial r with arbitrary coefficients ∈ Z_q, this routine recovers high
// order bits of r + z s.t. hint bit was computed using `make_hint` routine and
// z is another degree-255 polynomial with small coefficients.
template<const uint32_t alpha>
static inline void
use_hint(const field::zq_t* const __restrict polyh,
         const field::zq_t* const __restrict polyr,
         field::zq_t* const __restrict polyrz)
{
  for (size_t i = 0; i < ntt::N; i++) {
    polyrz[i] = reduction::use_hint<alpha>(polyh[i], polyr[i]);
  }
}

// Given a degree-255 polynomial, this routine counts number of coefficients
// having value 1.
inline size_t
count_1s(const field::zq_t* const __restrict poly)
{
  constexpr field::zq_t one{ 1u };
  size_t cnt = 0;

  for (size_t i = 0; i < ntt::N; i++) {
    cnt += 1 * (poly[i] == one);
  }

  return cnt;
}

// Given a degree-255 polynomial, this routine shifts each coefficient
// leftwards, by d bits
template<const size_t d>
static inline void
shl(field::zq_t* const __restrict poly)
{
  for (size_t i = 0; i < ntt::N; i++) {
    poly[i] = poly[i] << d;
  }
}

}
