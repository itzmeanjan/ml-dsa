#pragma once
#include "ff.hpp"
#include "utility"

// Utility functions for Dilithium Post-Quantum Digital Signature Algorithm
namespace dilithium_utils {

// Compile-time check to ensure that number of bits to be dropped from a
// polynomial coefficient is supplied correctly.
inline static constexpr bool
check_d(const size_t d)
{
  return d == 13;
}

// Given an element of Z_q | q = 2^23 - 2^13 + 1, this routine extracts out high
// and low order bits s.t.
//
// r mod^+ q = hi * 2^D + lo | -2^{D-1} < lo <= 2^{D-1}
//
// so that public key can be compressed.
//
// See definition of this routine in figure 3 of Dilithium specification, as
// submitted to NIST final round call
// https://csrc.nist.gov/CSRC/media/Projects/post-quantum-cryptography/documents/round-3/submissions/Dilithium-Round3.zip
//
// This implementation collects some ideas from
// https://github.com/pq-crystals/dilithium/blob/3e9b9f1/ref/rounding.c#L5-L23
template<const size_t d>
inline static std::pair<ff::ff_t, ff::ff_t>
power2round(const ff::ff_t r) requires(check_d(d))
{
  constexpr uint32_t max = 1u << (d - 1);

  const uint32_t t1 = r.v + max - 1u;
  const uint32_t t2 = t1 >> d;
  const uint32_t t3 = t2 << d;

  const ff::ff_t hi{ t2 };
  const ff::ff_t lo = r - ff::ff_t{ t3 };

  return std::make_pair(hi, lo);
}

// Given an element of Z_q | q = 2^23 - 2^13 + 1, this routine computes high and
// low order bits s.t.
//
// r mod^+ q = r1 * α + r0 | -α/2 < r0 <= α/2
//
// If r1 = (q - 1)/ α then r1 = 0; r0 = r0 - 1
//
// See definition of this routine in figure 3 of Dilithium specification, as
// submitted to NIST final round call
// https://csrc.nist.gov/CSRC/media/Projects/post-quantum-cryptography/documents/round-3/submissions/Dilithium-Round3.zip
template<const uint32_t alpha>
inline static std::pair<ff::ff_t, ff::ff_t>
decompose(const ff::ff_t r)
{
  constexpr uint32_t t0 = alpha >> 1;
  constexpr uint32_t t1 = ff::Q - 1u;

  const uint32_t t2 = r.v + t0 - 1u;
  const uint32_t t3 = t2 / alpha;
  const uint32_t t4 = t3 * alpha;

  const ff::ff_t r0 = r - ff::ff_t{ t4 };
  const ff::ff_t t5 = r - r0;

  const bool flg = !static_cast<bool>(t5.v ^ t1);
  const ff::ff_t br[]{ t5.v / alpha, 0u };

  const ff::ff_t r1 = br[flg];
  const ff::ff_t r0_ = r0 - ff::ff_t{ 1u * flg };

  return std::make_pair(r1, r0_);
}

// Given an element ∈ Z_q, this routine uses decompose routine ( defined above )
// to extract out high order bits of r.
//
// See definition of this routine in figure 3 of Dilithium specification, as
// submitted to NIST final round call
// https://csrc.nist.gov/CSRC/media/Projects/post-quantum-cryptography/documents/round-3/submissions/Dilithium-Round3.zip
template<const uint32_t alpha>
inline static ff::ff_t
highbits(const ff::ff_t r)
{
  const auto s = decompose<alpha>(r);
  return s.first;
}

// Given an element ∈ Z_q, this routine uses decompose routine ( defined above )
// to extract out low order bits of r.
//
// See definition of this routine in figure 3 of Dilithium specification, as
// submitted to NIST final round call
// https://csrc.nist.gov/CSRC/media/Projects/post-quantum-cryptography/documents/round-3/submissions/Dilithium-Round3.zip
template<const uint32_t alpha>
inline static ff::ff_t
lowbits(const ff::ff_t r)
{
  const auto s = decompose<alpha>(r);
  return s.second;
}

// This algorithm takes r, z ∈ Z_q, producing a 1 -bit hint h such that
// it allows one to compute the higher order bits of r + z just using r and h.
//
// This hint is essentially the “carry” caused by z in the addition.
// Note, z is small.
//
// See definition of this routine in figure 3 of Dilithium specification, as
// submitted to NIST final round call
// https://csrc.nist.gov/CSRC/media/Projects/post-quantum-cryptography/documents/round-3/submissions/Dilithium-Round3.zip
template<const uint32_t alpha>
inline static ff::ff_t
make_hint(const ff::ff_t z, const ff::ff_t r)
{
  const ff::ff_t r1 = highbits<alpha>(r);
  const ff::ff_t v1 = highbits<alpha>(r + z);

  return ff::ff_t{ static_cast<uint32_t>(r1 != v1) };
}

// 1 -bit hint ( read h ) is used to recover higher order bits of r + z s.t.
// hint bit was computed using make_hint routine ( defined above ).
//
// See definition of this routine in figure 3 of Dilithium specification, as
// submitted to NIST final round call
// https://csrc.nist.gov/CSRC/media/Projects/post-quantum-cryptography/documents/round-3/submissions/Dilithium-Round3.zip
template<const uint32_t alpha>
inline static ff::ff_t
use_hint(const ff::ff_t h, const ff::ff_t r)
{
  constexpr uint32_t m = (ff::Q - 1) / alpha;
  constexpr ff::ff_t t0{ alpha >> 1 };
  constexpr ff::ff_t t1 = ff::ff_t{ ff::Q } - t0;
  constexpr ff::ff_t t2{ 1u };

  const auto s = decompose<alpha>(r);

  if ((h == t2) && ((s.second.v > 0u) && (s.second.v < t1.v))) {
    const bool flg = s.first == ff::ff_t{ m - 1u };
    const ff::ff_t br[]{ s.first + t2, ff::ff_t{ 0u } };

    return br[flg];
  } else if ((h == t2) && (s.second.v >= t1.v)) {
    const bool flg = s.first == ff::ff_t{ 0u };
    const ff::ff_t br[]{ s.first - t2, ff::ff_t{ m - 1 } };

    return br[flg];
  } else {
    return s.first;
  }
}

}
