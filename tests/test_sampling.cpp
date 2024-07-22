#include "ml_dsa/internals/poly/sampling.hpp"
#include <gtest/gtest.h>
#include <numeric>

// Check whether hashing to a ball routine works as expected or not.
template<uint32_t τ>
static void
test_sample_in_ball()
{
  std::array<uint8_t, 32> seed{};
  std::array<ml_dsa_field::zq_t, ml_dsa_ntt::N> poly{};

  ml_dsa_prng::prng_t<256> prng;
  prng.read(seed);

  ml_dsa_sampling::sample_in_ball<τ>(seed, poly);

  auto sqrd_norm = std::accumulate(poly.begin(), poly.end(), ml_dsa_field::zq_t::zero(), [](auto acc, auto cur) -> auto { return acc + (cur * cur); });
  EXPECT_EQ(sqrd_norm, ml_dsa_field::zq_t(τ));
}

TEST(ML_DSA, HashingToABall)
{
  test_sample_in_ball<39u>();
  test_sample_in_ball<49u>();
  test_sample_in_ball<60u>();
}
