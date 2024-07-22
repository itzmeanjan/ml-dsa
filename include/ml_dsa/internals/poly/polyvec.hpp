#pragma once
#include "bit_packing.hpp"
#include "ml_dsa/internals/math/field.hpp"
#include "ml_dsa/internals/utility/params.hpp"
#include "poly.hpp"
#include <limits>

// Utility functions applied on vector of degree-255 polynomials
namespace ml_dsa_polyvec {

using const_poly_t = std::span<const ml_dsa_field::zq_t, ml_dsa_ntt::N>;
using poly_t = std::span<ml_dsa_field::zq_t, ml_dsa_ntt::N>;

// Applies NTT on a vector ( of dimension k x 1 ) of degree-255 polynomials.
template<size_t k>
static inline constexpr void
ntt(std::span<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> vec)
{
  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ml_dsa_ntt::N;
    ml_dsa_ntt::ntt(poly_t(vec.subspan(off, ml_dsa_ntt::N)));
  }
}

// Applies iNTT on a vector ( of dimension k x 1 ) of degree-255 polynomials.
template<size_t k>
static inline constexpr void
intt(std::span<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> vec)
{
  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ml_dsa_ntt::N;
    ml_dsa_ntt::intt(poly_t(vec.subspan(off, ml_dsa_ntt::N)));
  }
}

// Compresses vector ( of dimension k x 1 ) of degree-255 polynomials by extracting out high and low order bits.
template<size_t k, size_t d>
static inline constexpr void
power2round(std::span<const ml_dsa_field::zq_t, k * ml_dsa_ntt::N> poly,
            std::span<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> poly_hi,
            std::span<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> poly_lo)
  requires(ml_dsa_params::check_d(d))
{
  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ml_dsa_ntt::N;
    ml_dsa_poly::power2round<d>(const_poly_t(poly.subspan(off, ml_dsa_ntt::N)), poly_t(poly_hi.subspan(off, ml_dsa_ntt::N)), poly_t(poly_lo.subspan(off, ml_dsa_ntt::N)));
  }
}

// Given two matrices ( in NTT domain ) of compatible dimension, where each matrix element is a degree-255 polynomial
// over Z_q, this routine multiplies them, computing resulting matrix.
template<size_t a_rows, size_t a_cols, size_t b_rows, size_t b_cols>
static inline constexpr void
matrix_multiply(std::span<const ml_dsa_field::zq_t, a_rows * a_cols * ml_dsa_ntt::N> a,
                std::span<const ml_dsa_field::zq_t, b_rows * b_cols * ml_dsa_ntt::N> b,
                std::span<ml_dsa_field::zq_t, a_rows * b_cols * ml_dsa_ntt::N> c)
  requires(ml_dsa_params::check_matrix_dim(a_cols, b_rows))
{
  std::array<ml_dsa_field::zq_t, ml_dsa_ntt::N> tmp{};
  auto tmp_span = poly_t(tmp);

  for (size_t i = 0; i < a_rows; i++) {
    for (size_t j = 0; j < b_cols; j++) {
      const size_t coff = (i * b_cols + j) * ml_dsa_ntt::N;

      for (size_t k = 0; k < a_cols; k++) {
        const size_t aoff = (i * a_cols + k) * ml_dsa_ntt::N;
        const size_t boff = (k * b_cols + j) * ml_dsa_ntt::N;

        ml_dsa_poly::mul(const_poly_t(a.subspan(aoff, ml_dsa_ntt::N)), const_poly_t(b.subspan(boff, ml_dsa_ntt::N)), tmp_span);

        for (size_t l = 0; l < tmp_span.size(); l++) {
          c[coff + l] += tmp_span[l];
        }
      }
    }
  }
}

// Given a vector ( of dimension k x 1 ) of degree-255 polynomials, this routine adds it to another polynomial vector of
// same dimension s.t. destination vector is mutated.
template<size_t k>
static inline constexpr void
add_to(std::span<const ml_dsa_field::zq_t, k * ml_dsa_ntt::N> src, std::span<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> dst)
{
  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ml_dsa_ntt::N;

    for (size_t l = 0; l < ml_dsa_ntt::N; l++) {
      dst[off + l] += src[off + l];
    }
  }
}

// Given a vector ( of dimension k x 1 ) of degree-255 polynomials, this routine negates each coefficient.
template<size_t k>
static inline constexpr void
neg(std::span<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> vec)
{
  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ml_dsa_ntt::N;

    for (size_t l = 0; l < ml_dsa_ntt::N; l++) {
      vec[off + l] = -vec[off + l];
    }
  }
}

// Given a vector ( of dimension k x 1 ) of degree-255 polynomials s.t. each coefficient ∈ [-x, x], this routine
// subtracts each coefficient from x so that coefficients now stay in [0, 2x].
template<size_t k, uint32_t x>
static inline constexpr void
sub_from_x(std::span<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> vec)
{
  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ml_dsa_ntt::N;
    ml_dsa_poly::sub_from_x<x>(poly_t(vec.subspan(off, ml_dsa_ntt::N)));
  }
}

// Given a vector ( of dimension k x 1 ) of degree-255 polynomials, this routine encodes each of those polynomials into
// 32 x sbw -bytes, writing to a (k x 32 x sbw) -bytes destination array.
template<size_t k, size_t sbw>
static inline constexpr void
encode(std::span<const ml_dsa_field::zq_t, k * ml_dsa_ntt::N> src, std::span<uint8_t, (k * sbw * ml_dsa_ntt::N) / std::numeric_limits<uint8_t>::digits> dst)
{
  // Byte length of degree-255 polynomial after serialization
  constexpr size_t poly_blen = dst.size() / k;

  for (size_t i = 0; i < k; i++) {
    const size_t off0 = i * ml_dsa_ntt::N;
    const size_t off1 = i * poly_blen;

    ml_dsa_bit_packing::encode<sbw>(const_poly_t(src.subspan(off0, ml_dsa_ntt::N)), std::span<uint8_t, poly_blen>(dst.subspan(off1, poly_blen)));
  }
}

// Given a byte array of length (k x 32 x sbw) -bytes, this routine decodes them into k degree-255 polynomials, writing
// them to a column vector of dimension k x 1.
template<size_t k, size_t sbw>
static inline constexpr void
decode(std::span<const uint8_t, (k * sbw * ml_dsa_ntt::N) / std::numeric_limits<uint8_t>::digits> src, std::span<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> dst)
{
  // Byte length of degree-255 polynomial after serialization
  constexpr size_t poly_blen = src.size() / k;

  for (size_t i = 0; i < k; i++) {
    const size_t off0 = i * poly_blen;
    const size_t off1 = i * ml_dsa_ntt::N;

    ml_dsa_bit_packing::decode<sbw>(std::span<const uint8_t, poly_blen>(src.subspan(off0, poly_blen)), poly_t(dst.subspan(off1, ml_dsa_ntt::N)));
  }
}

// Given a vector (of dimension k x 1) of degree-255 polynomials, it extracts out high order bits from each coefficient.
template<size_t k, uint32_t alpha>
static inline constexpr void
highbits(std::span<const ml_dsa_field::zq_t, k * ml_dsa_ntt::N> src, std::span<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> dst)
{
  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ml_dsa_ntt::N;
    ml_dsa_poly::highbits<alpha>(const_poly_t(src.subspan(off, ml_dsa_ntt::N)), poly_t(dst.subspan(off, ml_dsa_ntt::N)));
  }
}

// Given a vector (of dimension k x 1) of degree-255 polynomials, it extracts out low order bits from each coefficient.
template<size_t k, uint32_t alpha>
static inline constexpr void
lowbits(std::span<const ml_dsa_field::zq_t, k * ml_dsa_ntt::N> src, std::span<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> dst)
{
  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ml_dsa_ntt::N;
    ml_dsa_poly::lowbits<alpha>(const_poly_t(src.subspan(off, ml_dsa_ntt::N)), poly_t(dst.subspan(off, ml_dsa_ntt::N)));
  }
}

// Given a vector ( of dimension k x 1 ) of degree-255 polynomials and one multiplier polynomial, this routine performs
// k pointwise polynomial multiplications when each of these polynomials are in their NTT representation.
template<size_t k>
static inline constexpr void
mul_by_poly(std::span<const ml_dsa_field::zq_t, ml_dsa_ntt::N> poly,
            std::span<const ml_dsa_field::zq_t, k * ml_dsa_ntt::N> src_vec,
            std::span<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> dst_vec)
{
  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ml_dsa_ntt::N;
    ml_dsa_poly::mul(poly, const_poly_t(src_vec.subspan(off, ml_dsa_ntt::N)), poly_t(dst_vec.subspan(off, ml_dsa_ntt::N)));
  }
}

// Computes infinity norm of a vector ( of dimension k x 1 ) of degree-255 polynomials.
template<size_t k>
static inline constexpr ml_dsa_field::zq_t
infinity_norm(std::span<const ml_dsa_field::zq_t, k * ml_dsa_ntt::N> vec)
{
  auto res = ml_dsa_field::zq_t::zero();

  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ml_dsa_ntt::N;
    res = std::max(res, ml_dsa_poly::infinity_norm(const_poly_t(vec.subspan(off, ml_dsa_ntt::N))));
  }

  return res;
}

// Given two vectors (of dimension k x 1) of degree-255 polynomials, this routine computes hint bit for each
// coefficient, using `make_hint` routine.
template<size_t k, uint32_t alpha>
static inline constexpr void
make_hint(std::span<const ml_dsa_field::zq_t, k * ml_dsa_ntt::N> polya,
          std::span<const ml_dsa_field::zq_t, k * ml_dsa_ntt::N> polyb,
          std::span<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> polyc)
{
  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ml_dsa_ntt::N;
    ml_dsa_poly::make_hint<alpha>(
      const_poly_t(polya.subspan(off, ml_dsa_ntt::N)), const_poly_t(polyb.subspan(off, ml_dsa_ntt::N)), poly_t(polyc.subspan(off, ml_dsa_ntt::N)));
  }
}

// Recovers high order bits of a vector of degree-255 polynomials (i.e. r + z) s.t. hint bits (say h) and another
// polynomial vector (say r) are provided.
template<size_t k, uint32_t alpha>
static inline constexpr void
use_hint(std::span<const ml_dsa_field::zq_t, k * ml_dsa_ntt::N> polyh,
         std::span<const ml_dsa_field::zq_t, k * ml_dsa_ntt::N> polyr,
         std::span<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> polyrz)
{
  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ml_dsa_ntt::N;
    ml_dsa_poly::use_hint<alpha>(
      const_poly_t(polyh.subspan(off, ml_dsa_ntt::N)), const_poly_t(polyr.subspan(off, ml_dsa_ntt::N)), poly_t(polyrz.subspan(off, ml_dsa_ntt::N)));
  }
}

// Given a vector (of dimension k x 1) of degree-255 polynomials, it counts number of coefficients having value 1.
template<size_t k>
static inline constexpr size_t
count_1s(std::span<const ml_dsa_field::zq_t, k * ml_dsa_ntt::N> vec)
{
  size_t cnt = 0;

  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ml_dsa_ntt::N;
    cnt += ml_dsa_poly::count_1s(const_poly_t(vec.subspan(off, ml_dsa_ntt::N)));
  }

  return cnt;
}

// Given a vector (of dimension k x 1) of degree-255 polynomials, it shifts each coefficient leftwards by d bits.
template<size_t k, size_t d>
static inline constexpr void
shl(std::span<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> vec)
{
  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ml_dsa_ntt::N;
    ml_dsa_poly::shl<d>(poly_t(vec.subspan(off, ml_dsa_ntt::N)));
  }
}

}
