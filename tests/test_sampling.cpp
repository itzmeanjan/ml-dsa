#include "sampling.hpp"
#include <array>
#include <gtest/gtest.h>
#include <vector>

// Check whether hashing to a ball routine works as expected or not.
template<uint32_t τ>
static void
test_sample_in_ball()
{
  std::array<uint8_t, 32> seed{};
  std::vector<field::zq_t> poly(ntt::N, 0);

  auto _seed = std::span(seed);
  auto _poly = std::span<field::zq_t, ntt::N>(poly);

  prng::prng_t prng;

  prng.read(_seed);
  sampling::sample_in_ball<τ>(_seed, _poly);

  auto sqrd_norm = field::zq_t::zero();

  for (size_t i = 0; i < _poly.size(); i++) {
    sqrd_norm += (_poly[i] * _poly[i]);
  }

  EXPECT_EQ(sqrd_norm, field::zq_t(τ));
}

TEST(Dilithium, HashingToABall)
{
  test_sample_in_ball<39u>();
  test_sample_in_ball<49u>();
  test_sample_in_ball<60u>();
}
