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
// and low order bits, using modulo of power of 2 ( = 2^d ), so that public key
// can be compressed.
//
// See definition of this routine in figure 3 of Dilithium specification, as
// submitted to NIST final round call
// https://csrc.nist.gov/CSRC/media/Projects/post-quantum-cryptography/documents/round-3/submissions/Dilithium-Round3.zip
template<const size_t d>
inline static std::pair<ff::ff_t, ff::ff_t>
power2round(const ff::ff_t r) requires(check_d(d))
{
  constexpr uint32_t mask = (1u << d) - 1u;

  const uint32_t t0 = r.v >> d;
  const uint32_t t1 = r.v & mask;

  return std::make_pair(ff::ff_t{ t0 }, ff::ff_t{ t1 });
}

}
