#pragma once
#include "ff.hpp"

// (inverse) Number Theoretic Transform for degree-255 polynomial, over
// Dilithium Prime Field Z_q | q = 2^23 - 2^13 + 1
namespace ntt {

constexpr size_t LOG2N = 8;
constexpr size_t N = 1 << LOG2N;

// First primitive 512 -th root of unity modulo q | q = 2^23 - 2^13 + 1
//
// Meaning, 1753 ** 512 == 1 mod q
constexpr ff::ff_t ζ{ 1753 };

// Multiplicative inverse of N over Z_q | q = 2^23 - 2^13 + 1, N = 256
constexpr auto INV_N = ff::ff_t{ N }.inv();

// Given a 64 -bit unsigned integer, this routine extracts specified many
// contiguous bits from ( least significant bits ) LSB side & reverses their bit
// order, returning bit reversed `mbw` -bit wide number
//
// See
// https://github.com/itzmeanjan/kyber/blob/3cd41a5/include/ntt.hpp#L74-L93
// for source of inspiration
template<const size_t mbw>
inline static constexpr size_t
bit_rev(const size_t v)
{
  size_t v_rev = 0ul;

  for (size_t i = 0; i < mbw; i++) {
    const size_t bit = (v >> i) & 0b1;
    v_rev ^= bit << (mbw - 1ul - i);
  }

  return v_rev;
}

// Given a polynomial f with 256 coefficients over Z_q | q = 2^23 - 2^13 + 1,
// this routine computes number theoretic transform using Cooley-Tukey
// algorithm, producing polynomial f' s.t. its coefficients are placed in
// bit-reversed order
//
// Note, this routine mutates input i.e. it's an in-place NTT implementation.
//
// Implementation inspired from
// https://github.com/itzmeanjan/kyber/blob/3cd41a5/include/ntt.hpp#L95-L129
static void
ntt(ff::ff_t* const __restrict poly)
{
  for (int64_t l = LOG2N - 1; l >= 0; l--) {
    const size_t len = 1ul << l;
    const size_t lenx2 = len << 1;
    const size_t k_beg = N >> (l + 1);

    for (size_t start = 0; start < N; start += lenx2) {
      const size_t k_now = k_beg + (start >> (l + 1));
      const ff::ff_t ζ_exp = ζ ^ bit_rev<LOG2N>(k_now);

      for (size_t i = start; i < start + len; i++) {
        auto tmp = ζ_exp * poly[i + len];

        poly[i + len] = poly[i] - tmp;
        poly[i] = poly[i] + tmp;
      }
    }
  }
}

// Given a polynomial f with 256 coefficients over Z_q | q = 2^23 - 2^13 + 1,
// s.t. its coefficients are placed in bit-reversed order, this routine computes
// inverse number theoretic transform using Gentleman-Sande algorithm, producing
// polynomial f' s.t. its coefficients are placed in standard order
//
// Note, this routine mutates input i.e. it's an in-place iNTT implementation.
//
// Implementation inspired from
// https://github.com/itzmeanjan/kyber/blob/3cd41a5/include/ntt.hpp#L131-L172
static void
intt(ff::ff_t* const __restrict poly)
{
  for (size_t l = 0; l < LOG2N; l++) {
    const size_t len = 1ul << l;
    const size_t lenx2 = len << 1;
    const size_t k_beg = (N >> l) - 1;

    for (size_t start = 0; start < N; start += lenx2) {
      const size_t k_now = k_beg - (start >> (l + 1));
      const ff::ff_t neg_ζ_exp = -(ζ ^ bit_rev<LOG2N>(k_now));

      for (size_t i = start; i < start + len; i++) {
        const auto tmp = poly[i];

        poly[i] = poly[i] + poly[i + len];
        poly[i + len] = tmp - poly[i + len];
        poly[i + len] = poly[i + len] * neg_ζ_exp;
      }
    }
  }

  for (size_t i = 0; i < N; i++) {
    poly[i] = poly[i] * INV_N;
  }
}

}
