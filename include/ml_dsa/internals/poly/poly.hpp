#pragma once
#include "ml_dsa/internals/math/field.hpp"
#include "ml_dsa/internals/math/reduction.hpp"
#include "ml_dsa/internals/utility/params.hpp"
#include "ntt.hpp"
#include <algorithm>

// Degree-255 polynomial arithmetic
namespace ml_dsa_poly {

// Given a degree-255 polynomial, this routine extracts out high and low order bits from each coefficient.
template<size_t d>
static inline constexpr void
power2round(std::span<const ml_dsa_field::zq_t, ml_dsa_ntt::N> poly,
            std::span<ml_dsa_field::zq_t, ml_dsa_ntt::N> poly_hi,
            std::span<ml_dsa_field::zq_t, ml_dsa_ntt::N> poly_lo)
  requires(ml_dsa_params::check_d(d))
{
  for (size_t i = 0; i < poly.size(); i++) {
    const auto ext = ml_dsa_reduction::power2round<d>(poly[i]);

    poly_hi[i] = ext.first;
    poly_lo[i] = ext.second;
  }
}

// Given two degree-255 polynomials in NTT representation, this routine performs element-wise multiplication over Z_q.
static inline constexpr void
mul(std::span<const ml_dsa_field::zq_t, ml_dsa_ntt::N> polya, std::span<const ml_dsa_field::zq_t, ml_dsa_ntt::N> polyb, std::span<ml_dsa_field::zq_t, ml_dsa_ntt::N> polyc)
{
#if (not defined __clang__) && (defined __GNUG__)
#pragma GCC unroll 16
#pragma GCC ivdep
#endif
  for (size_t i = 0; i < polya.size(); i++) {
    polyc[i] = polya[i] * polyb[i];
  }
}

// Given a degree-255 polynomial, which has all of its coefficients in [-x, x], this routine subtracts each coefficient
// from x, so that they stay in [0, 2x].
template<uint32_t x>
static inline constexpr void
sub_from_x(std::span<ml_dsa_field::zq_t, ml_dsa_ntt::N> poly)
{
  constexpr ml_dsa_field::zq_t x_cap(x);

#if defined __clang__
#pragma clang loop unroll(enable) vectorize(enable) interleave(enable)
#endif
  for (size_t i = 0; i < poly.size(); i++) {
    poly[i] = x_cap - poly[i];
  }
}

// Given a degree-255 polynomial, this routine extracts out high order bits.
template<uint32_t alpha>
static inline constexpr void
highbits(std::span<const ml_dsa_field::zq_t, ml_dsa_ntt::N> src, std::span<ml_dsa_field::zq_t, ml_dsa_ntt::N> dst)
{
  for (size_t i = 0; i < src.size(); i++) {
    dst[i] = ml_dsa_reduction::highbits<alpha>(src[i]);
  }
}

// Given a degree-255 polynomial, this routine extracts out low order bits.
template<uint32_t alpha>
static inline constexpr void
lowbits(std::span<const ml_dsa_field::zq_t, ml_dsa_ntt::N> src, std::span<ml_dsa_field::zq_t, ml_dsa_ntt::N> dst)
{
  for (size_t i = 0; i < src.size(); i++) {
    dst[i] = ml_dsa_reduction::lowbits<alpha>(src[i]);
  }
}

// Computes infinity norm of a degree-255 polynomial.
//
// See line 462 of ML-DSA draft standard https://doi.org/10.6028/NIST.FIPS.204.ipd.
static inline constexpr ml_dsa_field::zq_t
infinity_norm(std::span<const ml_dsa_field::zq_t, ml_dsa_ntt::N> poly)
{
  constexpr ml_dsa_field::zq_t qby2(ml_dsa_field::Q / 2);
  auto res = ml_dsa_field::zq_t::zero();

  for (size_t i = 0; i < poly.size(); i++) {
#ifdef __clang__
    if (poly[i] > qby2) {
      res = std::max(res, -poly[i]);
    } else {
      res = std::max(res, poly[i]);
    }
#else
    const bool flg = poly[i] > qby2;
    const ml_dsa_field::zq_t br[]{ poly[i], -poly[i] };

    res = std::max(res, br[flg]);
#endif
  }

  return res;
}

// Given two degree-255 polynomials, this routine computes hint bit for each coefficient.
template<uint32_t alpha>
static inline constexpr void
make_hint(std::span<const ml_dsa_field::zq_t, ml_dsa_ntt::N> polya,
          std::span<const ml_dsa_field::zq_t, ml_dsa_ntt::N> polyb,
          std::span<ml_dsa_field::zq_t, ml_dsa_ntt::N> polyc)
{
  for (size_t i = 0; i < polya.size(); i++) {
    polyc[i] = ml_dsa_reduction::make_hint<alpha>(polya[i], polyb[i]);
  }
}

// Given a hint bit polynomial (of degree-255) and another degree-255 polynomial r with arbitrary coefficients ∈
// Z_q, this routine recovers high order bits of r + z s.t. hint bit was computed using `make_hint` routine and z is
// another degree-255 polynomial with small coefficients.
template<uint32_t alpha>
static inline constexpr void
use_hint(std::span<const ml_dsa_field::zq_t, ml_dsa_ntt::N> polyh,
         std::span<const ml_dsa_field::zq_t, ml_dsa_ntt::N> polyr,
         std::span<ml_dsa_field::zq_t, ml_dsa_ntt::N> polyrz)
{
  for (size_t i = 0; i < polyh.size(); i++) {
    polyrz[i] = ml_dsa_reduction::use_hint<alpha>(polyh[i], polyr[i]);
  }
}

// Given a degree-255 polynomial, this routine counts number of coefficients having value 1.
static inline constexpr size_t
count_1s(std::span<const ml_dsa_field::zq_t, ml_dsa_ntt::N> poly)
{
  constexpr auto one = ml_dsa_field::zq_t::one();
  size_t cnt = 0;

  for (size_t i = 0; i < poly.size(); i++) {
    cnt += 1 * (poly[i] == one);
  }

  return cnt;
}

// Given a degree-255 polynomial, this routine shifts each coefficient leftwards, by d bits.
template<size_t d>
static inline constexpr void
shl(std::span<ml_dsa_field::zq_t, ml_dsa_ntt::N> poly)
{
  for (size_t i = 0; i < poly.size(); i++) {
    poly[i] = poly[i] << d;
  }
}

}
