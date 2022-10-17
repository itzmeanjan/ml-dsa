#pragma once
#include "ff.hpp"
#include <cassert>

// Test functional correctness of Dilithium PQC suite implementation
namespace test_dilithium {

// Test functional correctness of Dilithium prime field operations, by running
// through multiple rounds ( see template parameter ) of execution of field
// operations on randomly generated field element
template<const size_t rounds = 1024ul>
static void
test_field_ops()
{
  std::random_device rd;
  std::mt19937_64 gen(rd());
  std::uniform_int_distribution<size_t> dis{ 0ul, 1ul << 24 };

  for (size_t i = 0; i < rounds; i++) {
    const auto a = ff::ff_t::random();
    const auto b = ff::ff_t::random();

    // addition, subtraction, negation
    const auto c = a - b;
    const auto d = -b;
    const auto e = a + d;

    assert(c == e);

    // multiplication, division, inversion
    const auto f = a * b;
    const auto g = f / b;

    if (b == ff::ff_t::zero()) {
      assert(g == ff::ff_t::zero());
    } else {
      assert(g == a);
    }

    // exponentiation, multiplication
    const size_t exp = dis(gen);
    const auto h = a ^ exp;

    auto res = ff::ff_t::one();
    for (size_t j = 0; j < exp; j++) {
      res = res * a;
    }

    assert(res == h);
  }
}

}
