#include "ml_dsa/internals/poly/sampling.hpp"
#include "randomshake/randomshake.hpp"
#include <gtest/gtest.h>
#include <limits>
#include <numeric>

namespace {

// Check whether hashing to a ball routine works as expected or not.
template<uint32_t tau, size_t lambda>
void
test_sample_in_ball()
{
  std::array<uint8_t, (2 * lambda) / std::numeric_limits<uint8_t>::digits> seed{};
  std::array<ml_dsa_field::zq_t, ml_dsa_ntt::N> poly{};

  randomshake::randomshake_t csprng;
  csprng.generate(seed);

  ml_dsa_sampling::sample_in_ball<tau, lambda>(seed, poly);

  auto sqrd_norm = std::accumulate(poly.begin(), poly.end(), ml_dsa_field::zq_t::zero(), [](auto acc, auto cur) -> auto { return acc + (cur * cur); });
  EXPECT_EQ(sqrd_norm, ml_dsa_field::zq_t(tau));
}

}

TEST(ML_DSA, SampleInBallFor_ML_DSA_44)
{
  test_sample_in_ball<39U, 128U>();
}

TEST(ML_DSA, SampleInBallFor_ML_DSA_65)
{
  test_sample_in_ball<49U, 192U>();
}

TEST(ML_DSA, SampleInBallFor_ML_DSA_87)
{
  test_sample_in_ball<60U, 256U>();
}
