#include "ml_dsa/internals/math/field.hpp"
#include "randomshake/randomshake.hpp"
#include <gtest/gtest.h>

// Test functional correctness of ML-DSA prime field operations, by running through fairly large number of rounds of
// execution of field operations on randomly generated field element.
TEST(ML_DSA, ArithmeticOverZq)
{
  constexpr size_t itr_cnt = 1UL << 20;
  constexpr auto zero = ml_dsa_field::zq_t::zero();

  randomshake::randomshake_t csprng;

  for (size_t i = 0; i < itr_cnt; i++) {
    const auto a = ml_dsa_field::zq_t::random(csprng);
    const auto b = ml_dsa_field::zq_t::random(csprng);

    // Addition, Subtraction and Negation
    const auto c = a + b;
    const auto d = c - b;
    const auto e = c - a;

    EXPECT_EQ(d, a);
    EXPECT_EQ(e, b);

    // Multiplication, Exponentiation, Inversion and Division
    const auto f = a * b;
    const auto g = f / b;
    const auto h = f / a;

    if (b != zero) {
      EXPECT_EQ(g, a);
    } else {
      EXPECT_EQ(g, zero);
    }

    if (a != zero) {
      EXPECT_EQ(h, b);
    } else {
      EXPECT_EQ(h, zero);
    }
  }
}
