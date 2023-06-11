#pragma once
#include "prng.hpp"
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
  constexpr field::zq_t t0{ 1u << d };

  prng::prng_t prng;

  for (size_t i = 0; i < rounds; i++) {
    const field::zq_t r = field::zq_t::random(prng);

    const auto t1 = reduction::power2round<d>(r);
    const field::zq_t t2 = t1.first * t0 + t1.second;

    assert(r == t2);
  }
}

// Given any arbitrary element r ∈ Z_q and a small element z ∈ Z_q | q = 2^23 -
// 2^13 + 1, this routine tries to test correctness of decompose routine, which
// is used for computing a hint bit ( say h ) s.t. one doesn't need to store r,
// z both, but they can still recover high order bits of r + z
//
// Read section 2.4 of Dilithium specification
// https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf
template<const uint32_t alpha, const uint32_t z, const size_t rounds = 65536ul>
static void
test_decompose()
{
  prng::prng_t prng;

  for (size_t i = 0; i < rounds; i++) {
    const field::zq_t r = field::zq_t::random(prng);
    constexpr field::zq_t z_{ z };

    const field::zq_t h = reduction::make_hint<alpha>(z_, r);
    const field::zq_t rz = reduction::use_hint<alpha>(h, r);

    const field::zq_t rz_ = reduction::highbits<alpha>(r + z_);

    assert(rz == rz_);
  }
}

}
