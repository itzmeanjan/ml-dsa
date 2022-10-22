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

}
