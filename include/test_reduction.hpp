#pragma once
#include "reduction.hpp"
#include <cassert>

// Test functional correctness of Dilithium PQC suite implementation
namespace test_dilithium {

// Given a random element ∈ Z_q | q = 2^23 - 2^13 + 1, this routine tests
// whether extracting high and low order bits & then reconstructing original
// element using components of smaller bit width, work as expected or not.
template<const size_t rounds = 65536ul>
static void
test_power2round()
{
  constexpr size_t d = 13;
  constexpr ff::ff_t t0{ 1u << d };

  for (size_t i = 0; i < rounds; i++) {
    const ff::ff_t r = ff::ff_t::random();

    const auto t1 = dilithium_utils::power2round<d>(r);
    const ff::ff_t t2 = t1.first * t0 + t1.second;

    assert(r == t2);
  }
}

// Given any arbitrary element r ∈ Z_q and a small element z ∈ Z_q | q = 2^23 -
// 2^13 + 1, this routine tries to test correctness of decompose routine, which
// is used for computing a hint bit ( say h ) s.t. one doesn't need to store r,
// z both, but they can still recover high order bits of r + z
//
// Read section 2.4 of Dilithium specification
// https://csrc.nist.gov/CSRC/media/Projects/post-quantum-cryptography/documents/round-3/submissions/Dilithium-Round3.zip
template<const uint32_t alpha, const uint32_t z, const size_t rounds = 65536ul>
static void
test_decompose()
{
  for (size_t i = 0; i < rounds; i++) {
    const ff::ff_t r = ff::ff_t::random();
    constexpr ff::ff_t z_{ z };

    const ff::ff_t h = dilithium_utils::make_hint<alpha>(z_, r);
    const ff::ff_t rz = dilithium_utils::use_hint<alpha>(h, r);

    const ff::ff_t rz_ = dilithium_utils::highbits<alpha>(r + z_);

    assert(rz == rz_);
  }
}

}
