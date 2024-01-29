#pragma once
#include "bit_packing.hpp"
#include "field.hpp"
#include "params.hpp"
#include "poly.hpp"
#include <array>
#include <cstdint>
#include <span>

// Utility functions applied on vector of degree-255 polynomials
namespace polyvec {

using const_poly_t = std::span<const field::zq_t, ntt::N>;
using poly_t = std::span<field::zq_t, ntt::N>;

// Applies NTT on a vector ( of dimension k x 1 ) of degree-255 polynomials
template<size_t k>
static inline constexpr void
ntt(std::span<field::zq_t, k * ntt::N> vec)
{
  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ntt::N;
    ntt::ntt(poly_t(vec.subspan(off, ntt::N)));
  }
}

// Applies iNTT on a vector ( of dimension k x 1 ) of degree-255 polynomials
template<size_t k>
static inline constexpr void
intt(std::span<field::zq_t, k * ntt::N> vec)
{
  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ntt::N;
    ntt::intt(poly_t(vec.subspan(off, ntt::N)));
  }
}

// Compresses vector ( of dimension k x 1 ) of degree-255 polynomials by
// extracting out high and low order bits
template<size_t k, size_t d>
static inline constexpr void
power2round(std::span<const field::zq_t, k * ntt::N> poly,
            std::span<field::zq_t, k * ntt::N> poly_hi,
            std::span<field::zq_t, k * ntt::N> poly_lo)
  requires(dilithium_params::check_d(d))
{
  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ntt::N;
    poly::power2round<d>(const_poly_t(poly.subspan(off, ntt::N)),
                         poly_t(poly_hi.subspan(off, ntt::N)),
                         poly_t(poly_lo.subspan(off, ntt::N)));
  }
}

// Given two matrices ( in NTT domain ) of compatible dimension, where each
// matrix element is a degree-255 polynomial over Z_q | q = 2^23 -2^13 + 1, this
// routine attempts to multiply and compute resulting matrix
template<size_t a_rows, size_t a_cols, size_t b_rows, size_t b_cols>
static inline constexpr void
matrix_multiply(std::span<const field::zq_t, a_rows * a_cols * ntt::N> a,
                std::span<const field::zq_t, b_rows * b_cols * ntt::N> b,
                std::span<field::zq_t, a_rows * b_cols * ntt::N> c)
  requires(dilithium_params::check_matrix_dim(a_cols, b_rows))
{
  std::array<field::zq_t, ntt::N> tmp{};
  auto _tmp = std::span(tmp);

  for (size_t i = 0; i < a_rows; i++) {
    for (size_t j = 0; j < b_cols; j++) {
      const size_t coff = (i * b_cols + j) * ntt::N;

      for (size_t k = 0; k < a_cols; k++) {
        const size_t aoff = (i * a_cols + k) * ntt::N;
        const size_t boff = (k * b_cols + j) * ntt::N;

        poly::mul(const_poly_t(a.subspan(aoff, ntt::N)), const_poly_t(b.subspan(boff, ntt::N)), _tmp);

        for (size_t l = 0; l < _tmp.size(); l++) {
          c[coff + l] += _tmp[l];
        }
      }
    }
  }
}

// Given a vector ( of dimension k x 1 ) of degree-255 polynomials, this
// routine adds it to another polynomial vector of same dimension s.t.
// destination vector is mutated.
template<size_t k>
static inline constexpr void
add_to(std::span<const field::zq_t, k * ntt::N> src, std::span<field::zq_t, k * ntt::N> dst)
{
  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ntt::N;

    for (size_t l = 0; l < ntt::N; l++) {
      dst[off + l] += src[off + l];
    }
  }
}

// Given a vector ( of dimension k x 1 ) of degree-255 polynomials, this
// routine negates each coefficient.
template<size_t k>
static inline constexpr void
neg(std::span<field::zq_t, k * ntt::N> vec)
{
  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ntt::N;

    for (size_t l = 0; l < ntt::N; l++) {
      vec[off + l] = -vec[off + l];
    }
  }
}

// Given a vector ( of dimension k x 1 ) of degree-255 polynomials s.t. each
// coefficient ∈ [-x, x], this routine subtracts each coefficient from x so that
// coefficients now stay in [0, 2x].
template<size_t k, uint32_t x>
static inline constexpr void
sub_from_x(std::span<field::zq_t, k * ntt::N> vec)
{
  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ntt::N;
    poly::sub_from_x<x>(poly_t(vec.subspan(off, ntt::N)));
  }
}

// Given a vector ( of dimension k x 1 ) of degree-255 polynomials, this routine
// encodes each of those polynomials into 32 x sbw -bytes, writing to a
// (k x 32 x sbw) -bytes destination array.
template<size_t k, size_t sbw>
static inline constexpr void
encode(std::span<const field::zq_t, k * ntt::N> src, std::span<uint8_t, k * sbw * ntt::N / 8> dst)
{
  // Byte length of degree-255 polynomial after serialization
  constexpr size_t poly_blen = sbw * ntt::N / 8;

  for (size_t i = 0; i < k; i++) {
    const size_t off0 = i * ntt::N;
    const size_t off1 = i * poly_blen;

    bit_packing::encode<sbw>(const_poly_t(src.subspan(off0, ntt::N)),
                             std::span<uint8_t, poly_blen>(dst.subspan(off1, poly_blen)));
  }
}

// Given a byte array of length (k x 32 x sbw) -bytes, this routine decodes them
// into k degree-255 polynomials, writing them to a column vector of dimension
// k x 1.
template<size_t k, size_t sbw>
static inline constexpr void
decode(std::span<const uint8_t, k * sbw * ntt::N / 8> src, std::span<field::zq_t, k * ntt::N> dst)
{
  // Byte length of degree-255 polynomial after serialization
  constexpr size_t poly_blen = sbw * ntt::N / 8;

  for (size_t i = 0; i < k; i++) {
    const size_t off0 = i * poly_blen;
    const size_t off1 = i * ntt::N;

    bit_packing::decode<sbw>(std::span<const uint8_t, poly_blen>(src.subspan(off0, poly_blen)),
                             poly_t(dst.subspan(off1, ntt::N)));
  }
}

// Given a vector ( of dimension k x 1 ) of degree-255 polynomials, this routine
// extracts out high order bits from each coefficient
template<size_t k, uint32_t alpha>
static inline constexpr void
highbits(std::span<const field::zq_t, k * ntt::N> src, std::span<field::zq_t, k * ntt::N> dst)
{
  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ntt::N;
    poly::highbits<alpha>(const_poly_t(src.subspan(off, ntt::N)), poly_t(dst.subspan(off, ntt::N)));
  }
}

// Given a vector ( of dimension k x 1 ) of degree-255 polynomials, this routine
// extracts out low order bits from each coefficient, while not mutating operand
template<size_t k, uint32_t alpha>
static inline constexpr void
lowbits(std::span<const field::zq_t, k * ntt::N> src, std::span<field::zq_t, k * ntt::N> dst)
{
  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ntt::N;
    poly::lowbits<alpha>(const_poly_t(src.subspan(off, ntt::N)), poly_t(dst.subspan(off, ntt::N)));
  }
}

// Given a vector ( of dimension k x 1 ) of degree-255 polynomials and one
// multiplier polynomial, this routine performs k pointwise polynomial
// multiplications when each of these polynomials are in their NTT
// representation, while not mutating operand polynomials.
template<size_t k>
static inline constexpr void
mul_by_poly(std::span<const field::zq_t, ntt::N> poly,
            std::span<const field::zq_t, k * ntt::N> src_vec,
            std::span<field::zq_t, k * ntt::N> dst_vec)
{
  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ntt::N;
    poly::mul(poly, const_poly_t(src_vec.subspan(off, ntt::N)), poly_t(dst_vec.subspan(off, ntt::N)));
  }
}

// Computes infinity norm of a vector ( of dimension k x 1 ) of degree-255
// polynomials.
//
// See point `Sizes of elements` in section 2.1 of Dilithium specification
// https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf
template<size_t k>
static inline constexpr field::zq_t
infinity_norm(std::span<const field::zq_t, k * ntt::N> vec)
{
  auto res = field::zq_t::zero();

  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ntt::N;
    res = std::max(res, poly::infinity_norm(const_poly_t(vec.subspan(off, ntt::N))));
  }

  return res;
}

// Given two vector ( of dimension k x 1 ) of degree-255 polynomials, this
// routine computes hint bit for each coefficient, using `make_hint` routine.
template<size_t k, uint32_t alpha>
static inline constexpr void
make_hint(std::span<const field::zq_t, k * ntt::N> polya,
          std::span<const field::zq_t, k * ntt::N> polyb,
          std::span<field::zq_t, k * ntt::N> polyc)
{
  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ntt::N;
    poly::make_hint<alpha>(const_poly_t(polya.subspan(off, ntt::N)),
                           const_poly_t(polyb.subspan(off, ntt::N)),
                           poly_t(polyc.subspan(off, ntt::N)));
  }
}

// Recovers high order bits of a vector of degree-255 polynomials (  i.e. r + z
// ) s.t. hint bits ( say h ) and another polynomial vector ( say r ) are
// provided.
template<size_t k, uint32_t alpha>
static inline constexpr void
use_hint(std::span<const field::zq_t, k * ntt::N> polyh,
         std::span<const field::zq_t, k * ntt::N> polyr,
         std::span<field::zq_t, k * ntt::N> polyrz)
{
  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ntt::N;
    poly::use_hint<alpha>(const_poly_t(polyh.subspan(off, ntt::N)),
                          const_poly_t(polyr.subspan(off, ntt::N)),
                          poly_t(polyrz.subspan(off, ntt::N)));
  }
}

// Given a vector ( of dimension k x 1 ) of degree-255 polynomials, this routine
// counts number of coefficients having value 1.
template<size_t k>
static inline constexpr size_t
count_1s(std::span<const field::zq_t, k * ntt::N> vec)
{
  size_t cnt = 0;

  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ntt::N;
    cnt += poly::count_1s(const_poly_t(vec.subspan(off, ntt::N)));
  }

  return cnt;
}

// Given a vector ( of dimension k x 1 ) of degree-255 polynomials, this routine
// shifts each coefficient leftwards by d bits
template<size_t k, size_t d>
static inline constexpr void
shl(std::span<field::zq_t, k * ntt::N> vec)
{
  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ntt::N;
    poly::shl<d>(poly_t(vec.subspan(off, ntt::N)));
  }
}

}
