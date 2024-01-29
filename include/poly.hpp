#pragma once
#include "field.hpp"
#include "ntt.hpp"
#include "params.hpp"
#include "reduction.hpp"
#include <algorithm>

// Degree-255 polynomial utility functions for Dilithium Post-Quantum Digital
// Signature Algorithm
namespace poly {

// Given a degree-255 polynomial over Z_q | q = 2^23 - 2^13 + 1, this routine
// attempts to extract out high and low order bits from each of 256 coefficients
template<size_t d>
static inline constexpr void
power2round(std::span<const field::zq_t, ntt::N> poly,
            std::span<field::zq_t, ntt::N> poly_hi,
            std::span<field::zq_t, ntt::N> poly_lo)
  requires(dilithium_params::check_d(d))
{
  for (size_t i = 0; i < poly.size(); i++) {
    const auto ext = reduction::power2round<d>(poly[i]);

    poly_hi[i] = ext.first;
    poly_lo[i] = ext.second;
  }
}

// Given two degree-255 polynomials in NTT representation, this routine performs
// element-wise multiplication over Z_q | q = 2^23 - 2^13 + 1
static inline constexpr void
mul(std::span<const field::zq_t, ntt::N> polya,
    std::span<const field::zq_t, ntt::N> polyb,
    std::span<field::zq_t, ntt::N> polyc)
{
  for (size_t i = 0; i < polya.size(); i++) {
    polyc[i] = polya[i] * polyb[i];
  }
}

// Given a degree-255 polynomial, which has all of its coefficients in [-x, x],
// this routine subtracts each coefficient from x, so that they stay in [0, 2x].
template<uint32_t x>
static inline constexpr void
sub_from_x(std::span<field::zq_t, ntt::N> poly)
{
  constexpr field::zq_t x_cap(x);

  for (size_t i = 0; i < poly.size(); i++) {
    poly[i] = x_cap - poly[i];
  }
}

// Given a degree-255 polynomial, this routine extracts out high order bits (
// using decompose routine ), while not mutating source polynomial
template<uint32_t alpha>
static inline constexpr void
highbits(std::span<const field::zq_t, ntt::N> src, std::span<field::zq_t, ntt::N> dst)
{
  for (size_t i = 0; i < src.size(); i++) {
    dst[i] = reduction::highbits<alpha>(src[i]);
  }
}

// Given a degree-255 polynomial, this routine extracts out low order bits (
// using decompose routine ), while not mutating source polynomial
template<uint32_t alpha>
static inline constexpr void
lowbits(std::span<const field::zq_t, ntt::N> src, std::span<field::zq_t, ntt::N> dst)
{
  for (size_t i = 0; i < src.size(); i++) {
    dst[i] = reduction::lowbits<alpha>(src[i]);
  }
}

// Computes infinity norm of a degree-255 polynomial
//
// See point `Sizes of elements` in section 2.1 of Dilithium specification
// https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf
static inline constexpr field::zq_t
infinity_norm(std::span<const field::zq_t, ntt::N> poly)
{
  constexpr field::zq_t qby2(field::Q / 2);
  auto res = field::zq_t::zero();

  for (size_t i = 0; i < poly.size(); i++) {
    const bool flg = poly[i] > qby2;
    const field::zq_t br[]{ poly[i], -poly[i] };

    res = std::max(res, br[flg]);
  }

  return res;
}

// Given two degree-255 polynomials, this routine computes hint bit for each
// coefficient, using `make_hint` routine.
template<uint32_t alpha>
static inline constexpr void
make_hint(std::span<const field::zq_t, ntt::N> polya,
          std::span<const field::zq_t, ntt::N> polyb,
          std::span<field::zq_t, ntt::N> polyc)
{
  for (size_t i = 0; i < polya.size(); i++) {
    polyc[i] = reduction::make_hint<alpha>(polya[i], polyb[i]);
  }
}

// Given a hint bit polynomial ( of degree-255 ) and another degree-255
// polynomial r with arbitrary coefficients ∈ Z_q, this routine recovers high
// order bits of r + z s.t. hint bit was computed using `make_hint` routine and
// z is another degree-255 polynomial with small coefficients.
template<uint32_t alpha>
static inline constexpr void
use_hint(std::span<const field::zq_t, ntt::N> polyh,
         std::span<const field::zq_t, ntt::N> polyr,
         std::span<field::zq_t, ntt::N> polyrz)
{
  for (size_t i = 0; i < polyh.size(); i++) {
    polyrz[i] = reduction::use_hint<alpha>(polyh[i], polyr[i]);
  }
}

// Given a degree-255 polynomial, this routine counts number of coefficients
// having value 1.
static inline constexpr size_t
count_1s(std::span<const field::zq_t, ntt::N> poly)
{
  constexpr auto one = field::zq_t::one();
  size_t cnt = 0;

  for (size_t i = 0; i < poly.size(); i++) {
    cnt += 1 * (poly[i] == one);
  }

  return cnt;
}

// Given a degree-255 polynomial, this routine shifts each coefficient
// leftwards, by d bits
template<size_t d>
static inline constexpr void
shl(std::span<field::zq_t, ntt::N> poly)
{
  for (size_t i = 0; i < poly.size(); i++) {
    poly[i] = poly[i] << d;
  }
}

}
