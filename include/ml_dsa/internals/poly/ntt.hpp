#pragma once
#include "ml_dsa/internals/math/field.hpp"
#include <array>
#include <span>

// Number Theoretic Transform for degree-255 polynomial
namespace ml_dsa_ntt {

static constexpr size_t LOG2N = 8;
static constexpr size_t N = 1 << LOG2N;

// First primitive 512 -th root of unity modulo q
static constexpr ml_dsa_field::zq_t ζ(1753);
static_assert((ζ ^ 512) == ml_dsa_field::zq_t::one(), "ζ must be 512th root of unity modulo Q");

// Multiplicative inverse of N over Z_q
static constexpr auto INV_N = ml_dsa_field::zq_t(N).inv();

// Given a 64 -bit unsigned integer, this routine extracts specified many contiguous bits from LSB ( least significant
// bits ) side & reverses their bit order, returning bit reversed `mbw` -bit wide number.
//
// See https://github.com/itzmeanjan/kyber/blob/3cd41a5/include/ntt.hpp#L74-L93 for source of inspiration.
template<size_t mbw>
static inline constexpr size_t
bit_rev(const size_t v)
  requires(mbw == LOG2N)
{
  size_t v_rev = 0ul;

  for (size_t i = 0; i < mbw; i++) {
    const size_t bit = (v >> i) & 0b1;
    v_rev ^= bit << (mbw - 1ul - i);
  }

  return v_rev;
}

// Precomputed table of powers of ζ, used during polynomial evaluation.
static constexpr auto ζ_EXP = []() {
  std::array<ml_dsa_field::zq_t, N> res;

  for (size_t i = 0; i < N; i++) {
    res[i] = ζ ^ bit_rev<LOG2N>(i);
  }

  return res;
}();

// Precomputed table of negated powers of ζ, used during polynomial interpolation.
static constexpr auto ζ_NEG_EXP = []() {
  std::array<ml_dsa_field::zq_t, N> res;

  for (size_t i = 0; i < N; i++) {
    res[i] = -ζ_EXP[i];
  }

  return res;
}();

// Given a polynomial f with 256 coefficients over Z_q, this routine computes number theoretic transform using
// Cooley-Tukey algorithm, producing polynomial f' s.t. its coefficients are placed in bit-reversed order.
//
// Note, this routine mutates input i.e. it's an in-place NTT implementation.
//
// Implementation inspired from https://github.com/itzmeanjan/kyber/blob/3cd41a5/include/ntt.hpp#L95-L129.
// See algorithm 35 of ML-DSA draft standard https://doi.org/10.6028/NIST.FIPS.204.ipd.
static inline constexpr void
ntt(std::span<ml_dsa_field::zq_t, N> poly)
{
#if (not defined __clang__) && (defined __GNUG__)
#pragma GCC unroll 8
#endif
  for (int64_t l = LOG2N - 1; l >= 0; l--) {
    const size_t len = 1ul << l;
    const size_t lenx2 = len << 1;
    const size_t k_beg = N >> (l + 1);

    for (size_t start = 0; start < poly.size(); start += lenx2) {
      const size_t k_now = k_beg + (start >> (l + 1));
      const ml_dsa_field::zq_t ζ_exp = ζ_EXP[k_now];

#if (not defined __clang__) && (defined __GNUG__)
#pragma GCC unroll 4
#pragma GCC ivdep
#endif
      for (size_t i = start; i < start + len; i++) {
        auto tmp = ζ_exp * poly[i + len];

        poly[i + len] = poly[i] - tmp;
        poly[i] += tmp;
      }
    }
  }
}

// Given a polynomial f with 256 coefficients over Z_q, s.t. its coefficients are placed in bit-reversed order, this
// routine computes inverse number theoretic transform using Gentleman-Sande algorithm, producing polynomial f' s.t. its
// coefficients are placed in standard order.
//
// Note, this routine mutates input i.e. it's an in-place iNTT implementation.
//
// Implementation inspired from https://github.com/itzmeanjan/kyber/blob/3cd41a5/include/ntt.hpp#L131-L172.
// See algorithm 36 of ML-DSA draft standard https://doi.org/10.6028/NIST.FIPS.204.ipd.
static inline constexpr void
intt(std::span<ml_dsa_field::zq_t, N> poly)
{
#if (not defined __clang__) && (defined __GNUG__)
#pragma GCC unroll 8
#endif
  for (size_t l = 0; l < LOG2N; l++) {
    const size_t len = 1ul << l;
    const size_t lenx2 = len << 1;
    const size_t k_beg = (N >> l) - 1;

    for (size_t start = 0; start < poly.size(); start += lenx2) {
      const size_t k_now = k_beg - (start >> (l + 1));
      const ml_dsa_field::zq_t neg_ζ_exp = ζ_NEG_EXP[k_now];

#if (not defined __clang__) && (defined __GNUG__)
#pragma GCC unroll 4
#pragma GCC ivdep
#endif
      for (size_t i = start; i < start + len; i++) {
        const auto tmp = poly[i];

        poly[i] += poly[i + len];
        poly[i + len] = tmp - poly[i + len];
        poly[i + len] *= neg_ζ_exp;
      }
    }
  }

  for (size_t i = 0; i < poly.size(); i++) {
    poly[i] *= INV_N;
  }
}

}
