#pragma once
#include "bit_packing.hpp"
#include "params.hpp"
#include "poly.hpp"

// Utility functions applied on vector of degree-255 polynomials
namespace polyvec {

// Applies NTT on a vector ( of dimension k x 1 ) of degree-255 polynomials
template<const size_t k>
static inline void
ntt(field::zq_t* const __restrict vec)
{
  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ntt::N;
    ntt::ntt(vec + off);
  }
}

// Applies iNTT on a vector ( of dimension k x 1 ) of degree-255 polynomials
template<const size_t k>
static inline void
intt(field::zq_t* const __restrict vec)
{
  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ntt::N;
    ntt::intt(vec + off);
  }
}

// Compresses vector ( of dimension k x 1 ) of degree-255 polynomials by
// extracting out high and low order bits
template<const size_t k, const size_t d>
static inline void
power2round(const field::zq_t* const __restrict poly,
            field::zq_t* const __restrict poly_hi,
            field::zq_t* const __restrict poly_lo)
  requires(dilithium_params::check_d(d))
{
  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ntt::N;
    poly::power2round<d>(poly + off, poly_hi + off, poly_lo + off);
  }
}

// Given two matrices ( in NTT domain ) of compatible dimension, where each
// matrix element is a degree-255 polynomial over Z_q | q = 2^23 -2^13 + 1, this
// routine attempts to multiply and compute resulting matrix
template<const size_t a_rows,
         const size_t a_cols,
         const size_t b_rows,
         const size_t b_cols>
static void
matrix_multiply(const field::zq_t* const __restrict a,
                const field::zq_t* const __restrict b,
                field::zq_t* const __restrict c)
  requires(dilithium_params::check_matrix_dim(a_cols, b_rows))
{
  field::zq_t tmp[ntt::N]{};

  for (size_t i = 0; i < a_rows; i++) {
    for (size_t j = 0; j < b_cols; j++) {
      const size_t coff = (i * b_cols + j) * ntt::N;

      for (size_t k = 0; k < a_cols; k++) {
        const size_t aoff = (i * a_cols + k) * ntt::N;
        const size_t boff = (k * b_cols + j) * ntt::N;

        poly::mul(a + aoff, b + boff, tmp);

        for (size_t l = 0; l < ntt::N; l++) {
          c[coff + l] = c[coff + l] + tmp[l];
        }
      }
    }
  }
}

// Given a vector ( of dimension k x 1 ) of degree-255 polynomials, this
// routine adds it to another polynomial vector of same dimension s.t.
// destination vector is mutated.
template<const size_t k>
static inline void
add_to(const field::zq_t* const __restrict src,
       field::zq_t* const __restrict dst)
{
  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ntt::N;

    for (size_t l = 0; l < ntt::N; l++) {
      dst[off + l] = dst[off + l] + src[off + l];
    }
  }
}

// Given a vector ( of dimension k x 1 ) of degree-255 polynomials, this
// routine negates each coefficient.
template<const size_t k>
static inline void
neg(field::zq_t* const vec)
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
template<const size_t k, const uint32_t x>
static inline void
sub_from_x(field::zq_t* const vec)
{
  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ntt::N;
    poly::sub_from_x<x>(vec + off);
  }
}

// Given a vector ( of dimension k x 1 ) of degree-255 polynomials, this routine
// encodes each of those polynomials into 32 x sbw -bytes, writing to a
// (k x 32 x sbw) -bytes destination array
template<const size_t k, const size_t sbw>
static inline void
encode(const field::zq_t* const __restrict src, uint8_t* const __restrict dst)
{
  for (size_t i = 0; i < k; i++) {
    const size_t off0 = i * ntt::N;
    const size_t off1 = i * sbw * 32;

    bit_packing::encode<sbw>(src + off0, dst + off1);
  }
}

// Given a byte array of length (k x 32 x sbw) -bytes, this routine decodes them
// into k degree-255 polynomials, writing them to a column vector of dimension
// k x 1
template<const size_t k, const size_t sbw>
static inline void
decode(const uint8_t* const __restrict src, field::zq_t* const __restrict dst)
{
  for (size_t i = 0; i < k; i++) {
    const size_t off0 = i * sbw * 32;
    const size_t off1 = i * ntt::N;

    bit_packing::decode<sbw>(src + off0, dst + off1);
  }
}

// Given a vector ( of dimension k x 1 ) of degree-255 polynomials, this routine
// extracts out high order bits from each coefficient
template<const size_t k, const uint32_t alpha>
static inline void
highbits(const field::zq_t* const __restrict src,
         field::zq_t* const __restrict dst)
{
  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ntt::N;
    poly::highbits<alpha>(src + off, dst + off);
  }
}

// Given a vector ( of dimension k x 1 ) of degree-255 polynomials, this routine
// extracts out low order bits from each coefficient, while not mutating operand
template<const size_t k, const uint32_t alpha>
static inline void
lowbits(const field::zq_t* const __restrict src,
        field::zq_t* const __restrict dst)
{
  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ntt::N;
    poly::lowbits<alpha>(src + off, dst + off);
  }
}

// Given a vector ( of dimension k x 1 ) of degree-255 polynomials and one
// multiplier polynomial, this routine performs k pointwise polynomial
// multiplications when each of these polynomials are in their NTT
// representation, while not mutating operand polynomials.
template<const size_t k>
static inline void
mul_by_poly(const field::zq_t* const __restrict poly,
            const field::zq_t* const __restrict src_vec,
            field::zq_t* const __restrict dst_vec)
{
  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ntt::N;
    poly::mul(poly, src_vec + off, dst_vec + off);
  }
}

// Computes infinity norm of a vector ( of dimension k x 1 ) of degree-255
// polynomials
//
// See point `Sizes of elements` in section 2.1 of Dilithium specification
// https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf
template<const size_t k>
static inline field::zq_t
infinity_norm(const field::zq_t* const __restrict vec)
{
  field::zq_t res{ 0u };

  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ntt::N;
    res = std::max(res, poly::infinity_norm(vec + off));
  }

  return res;
}

// Given two vector ( of dimension k x 1 ) of degree-255 polynomials, this
// routine computes hint bit for each coefficient, using `make_hint` routine.
template<const size_t k, const uint32_t alpha>
static inline void
make_hint(const field::zq_t* const __restrict polya,
          const field::zq_t* const __restrict polyb,
          field::zq_t* const __restrict polyc)
{
  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ntt::N;
    poly::make_hint<alpha>(polya + off, polyb + off, polyc + off);
  }
}

// Recovers high order bits of a vector of degree-255 polynomials (  i.e. r + z
// ) s.t. hint bits ( say h ) and another polynomial vector ( say r ) are
// provided.
template<const size_t k, const uint32_t alpha>
static inline void
use_hint(const field::zq_t* const __restrict polyh,
         const field::zq_t* const __restrict polyr,
         field::zq_t* const __restrict polyrz)
{
  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ntt::N;
    poly::use_hint<alpha>(polyh + off, polyr + off, polyrz + off);
  }
}

// Given a vector ( of dimension k x 1 ) of degree-255 polynomials, this routine
// counts number of coefficients having value 1.
template<const size_t k>
static inline size_t
count_1s(const field::zq_t* const __restrict vec)
{
  size_t cnt = 0;

  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ntt::N;
    cnt += poly::count_1s(vec + off);
  }

  return cnt;
}

// Given a vector ( of dimension k x 1 ) of degree-255 polynomials, this routine
// shifts each coefficient leftwards by d bits
template<const size_t k, const size_t d>
static inline void
shl(field::zq_t* const __restrict vec)
{
  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ntt::N;
    poly::shl<d>(vec + off);
  }
}

}
