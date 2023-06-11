#pragma once
#include "field.hpp"
#include "prng.hpp"
#include <cassert>
#include <cstdint>

// Test functional correctness of Dilithium PQC suite implementation
namespace test_dilithium {

// Test functional correctness of Dilithium prime field operations, by running
// through multiple rounds ( see template parameter ) of execution of field
// operations on randomly generated field element
template<const size_t rounds = 1024ul>
static void
test_field_ops()
{
  prng::prng_t prng;

  for (size_t i = 0; i < rounds; i++) {
    const auto a = field::zq_t::random(prng);
    const auto b = field::zq_t::random(prng);

    // addition, subtraction, negation
    const auto c = a - b;
    const auto d = -b;
    const auto e = a + d;

    assert(c == e);

    // multiplication, division, inversion
    const auto f = a * b;
    const auto g = f / b;

    if (b == field::zq_t::zero()) {
      assert(g == field::zq_t::zero());
    } else {
      assert(g == a);
    }

    const auto h = a.inv();
    const auto k = h * a;

    if (a == field::zq_t::zero()) {
      assert(k == field::zq_t::zero());
    } else {
      assert(k == field::zq_t::one());
    }

    // exponentiation, multiplication
    size_t exp = 0;
    prng.read(reinterpret_cast<uint8_t*>(&exp), sizeof(exp));
    exp &= 0xfffful; // take only 16 -bit wide exponent, otherwise testing
                     // becomes too time consuming !

    const auto l = a ^ exp;

    auto res = field::zq_t::one();
    for (size_t j = 0; j < exp; j++) {
      res = res * a;
    }

    assert(res == l);
  }
}

}
