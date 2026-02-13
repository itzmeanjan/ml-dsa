#pragma once
#include "field.hpp"
#include <cstddef>
#include <cstdint>
#include <utility>

// Auxiliary functions used for extracting out high/ low order bits and making/ using hint bits.
namespace ml_dsa_reduction {

// Given an element of Z_q, this routine extracts out high and low order bits s.t.
//
// `r = hi * 2^D + lo (mod q)`
//
// This routine is used for compressing public key.
//
// See algorithm 35 of ML-DSA specification https://doi.org/10.6028/NIST.FIPS.204.
// This implementation collects some ideas from https://github.com/pq-crystals/dilithium/blob/3e9b9f1/ref/rounding.c#L5-L23.
template<size_t d>
static constexpr std::pair<ml_dsa_field::zq_t, ml_dsa_field::zq_t>
power2round(const ml_dsa_field::zq_t r)
{
  constexpr uint32_t max = 1U << (d - 1);

  const uint32_t t1 = r.raw() + max - 1U;
  const uint32_t t2 = t1 >> d;
  const uint32_t t3 = t2 << d;

  const ml_dsa_field::zq_t hi{ t2 };
  const ml_dsa_field::zq_t lo = r - ml_dsa_field::zq_t{ t3 };

  return std::make_pair(hi, lo);
}

// Given an element of Z_q, this routine computes high and low order bits s.t.
//
// `r mod^+ q = r1 * alpha + r0 | -alpha/2 < r0 <= alpha/2`
//
// If r1 = (q - 1)/ alpha then r1 = 0; r0 = r0 - 1
//
// See algorithm 36 of ML-DSA specification https://doi.org/10.6028/NIST.FIPS.204.
template<uint32_t alpha>
static constexpr std::pair<ml_dsa_field::zq_t, ml_dsa_field::zq_t>
decompose(const ml_dsa_field::zq_t r)
{
  constexpr uint32_t t0 = alpha >> 1;
  constexpr uint32_t t1 = ml_dsa_field::Q - 1U;

  const uint32_t t2 = r.raw() + t0 - 1U;
  const uint32_t t3 = t2 / alpha;
  const uint32_t t4 = t3 * alpha;

  const ml_dsa_field::zq_t r0 = r - ml_dsa_field::zq_t{ t4 };
  const ml_dsa_field::zq_t t5 = r - r0;

  const bool flg = (t5.raw() == t1);

  const ml_dsa_field::zq_t r1 = flg ? ml_dsa_field::zq_t::zero() : ml_dsa_field::zq_t(t5.raw() / alpha);
  const ml_dsa_field::zq_t r0_ = r0 - ml_dsa_field::zq_t{ static_cast<uint32_t>(flg) };

  return std::make_pair(r1, r0_);
}

// Given an element ∈ Z_q, this routine extracts out high order bits of r.
// See algorithm 37 of ML-DSA specification https://doi.org/10.6028/NIST.FIPS.204.
template<uint32_t alpha>
static constexpr ml_dsa_field::zq_t
highbits(const ml_dsa_field::zq_t r)
{
  const auto s = decompose<alpha>(r);
  return s.first;
}

// Given an element ∈ Z_q, this routine extracts out low order bits of r.
// See algorithm 38 of ML-DSA specification https://doi.org/10.6028/NIST.FIPS.204.
template<uint32_t alpha>
static constexpr ml_dsa_field::zq_t
lowbits(const ml_dsa_field::zq_t r)
{
  const auto s = decompose<alpha>(r);
  return s.second;
}

// This algorithm takes `r`, `z` ∈ Z_q, producing a 1 -bit hint `h` such that it allows one to compute the higher order
// bits of `r + z` just using `r` and `h`.
//
// This hint is essentially the “carry” caused by `z` in the addition. Note, `z` is small.
// See algorithm 39 of ML-DSA specification https://doi.org/10.6028/NIST.FIPS.204.
template<uint32_t alpha>
static constexpr ml_dsa_field::zq_t
make_hint(const ml_dsa_field::zq_t z, const ml_dsa_field::zq_t r)
{
  const ml_dsa_field::zq_t r1 = highbits<alpha>(r);
  const ml_dsa_field::zq_t v1 = highbits<alpha>(r + z);

  return ml_dsa_field::zq_t{ static_cast<uint32_t>(r1 != v1) };
}

// 1 -bit hint ( read `h` ) is used to recover higher order bits of `r + z`.
// See algorithm 40 of ML-DSA algorithm https://doi.org/10.6028/NIST.FIPS.204.
template<uint32_t alpha>
static constexpr ml_dsa_field::zq_t
use_hint(const ml_dsa_field::zq_t h, const ml_dsa_field::zq_t r)
{
  constexpr uint32_t m = (ml_dsa_field::Q - 1) / alpha;
  constexpr ml_dsa_field::zq_t t0{ alpha >> 1 };
  constexpr ml_dsa_field::zq_t t1 = ml_dsa_field::zq_t{ ml_dsa_field::Q } - t0;
  constexpr auto one = ml_dsa_field::zq_t::one();

  const auto s = decompose<alpha>(r);

  if ((h == one) && ((s.second > ml_dsa_field::zq_t::zero()) && (s.second < t1))) {
    const bool flg = s.first == ml_dsa_field::zq_t{ m - 1U };
    const ml_dsa_field::zq_t br[]{ s.first + one, ml_dsa_field::zq_t{ 0U } };

    return br[static_cast<size_t>(flg)];
  }
  if ((h == one) && (s.second >= t1)) {
    const bool flg = s.first == ml_dsa_field::zq_t{ 0U };
    const ml_dsa_field::zq_t br[]{ s.first - one, ml_dsa_field::zq_t{ m - 1 } };

    return br[static_cast<size_t>(flg)];
  }
  return s.first;
}

}
