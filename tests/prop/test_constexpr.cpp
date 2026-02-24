#include "ml_dsa/ml_dsa_44.hpp"
#include "ml_dsa/ml_dsa_65.hpp"
#include "ml_dsa/ml_dsa_87.hpp"
#include "test_helper.hpp"
#include <gtest/gtest.h>

// If this file compiles, the static_assert has already proven correctness at compile-time.
namespace {

// KAT vector no 1.
constexpr auto SEED = ml_dsa_test_helper::from_hex<32>("7c9935a0b07694aa0c6d10e4db6b1add2fd81a25ccb148032dcd739936737f2d");
constexpr auto MSG = ml_dsa_test_helper::from_hex<33>("d81c4d8d734fcbfbeade3d3f8a039faa2a2c9957e835ad55b22e75bf57bb556ac8");
constexpr auto CTX = ml_dsa_test_helper::from_hex<33>("8626ed79d451140800e03b59b956f8210e556067407d13dc90fa9e8b872bfb8fab");
constexpr auto RND = ml_dsa_test_helper::from_hex<32>("6255563ba961772146ca0867678d56787cad77ab4fc8fcfe9e02df839c99424d");

// Compile-time evaluation of ML-DSA-44 keygen, signing, and verification.
constexpr bool
eval_ml_dsa_44()
{
  std::array<uint8_t, ml_dsa_44::PubKeyByteLen> pubkey{};
  std::array<uint8_t, ml_dsa_44::SecKeyByteLen> seckey{};
  std::array<uint8_t, ml_dsa_44::SigByteLen> sig{};

  ml_dsa_44::keygen(SEED, pubkey, seckey);
  const auto signed_ok = ml_dsa_44::sign(RND, seckey, MSG, CTX, sig);
  const auto verified = ml_dsa_44::verify(pubkey, MSG, CTX, sig);

  return signed_ok && verified;
}

// Compile-time evaluation of ML-DSA-65 keygen, signing, and verification.
constexpr bool
eval_ml_dsa_65()
{
  std::array<uint8_t, ml_dsa_65::PubKeyByteLen> pubkey{};
  std::array<uint8_t, ml_dsa_65::SecKeyByteLen> seckey{};
  std::array<uint8_t, ml_dsa_65::SigByteLen> sig{};

  ml_dsa_65::keygen(SEED, pubkey, seckey);
  const auto signed_ok = ml_dsa_65::sign(RND, seckey, MSG, CTX, sig);
  const auto verified = ml_dsa_65::verify(pubkey, MSG, CTX, sig);

  return signed_ok && verified;
}

// Compile-time evaluation of ML-DSA-87 keygen, signing, and verification.
constexpr bool
eval_ml_dsa_87()
{
  std::array<uint8_t, ml_dsa_87::PubKeyByteLen> pubkey{};
  std::array<uint8_t, ml_dsa_87::SecKeyByteLen> seckey{};
  std::array<uint8_t, ml_dsa_87::SigByteLen> sig{};

  ml_dsa_87::keygen(SEED, pubkey, seckey);
  const auto signed_ok = ml_dsa_87::sign(RND, seckey, MSG, CTX, sig);
  const auto verified = ml_dsa_87::verify(pubkey, MSG, CTX, sig);

  return signed_ok && verified;
}

} // anonymous namespace

// The static_asserts below are the real tests. They fire at compile-time.
// If this translation unit compiles, all three ML-DSA variants are proven correct at program compile-time itself.
// The GTest bodies exist only so that ctest reports them as passed.

TEST(ML_DSA, CompileTimeEval_ML_DSA_44)
{
  static_assert(eval_ml_dsa_44(), "ML-DSA-44 keygen/sign/verify must be correct at compile-time");
  SUCCEED();
}

TEST(ML_DSA, CompileTimeEval_ML_DSA_65)
{
  static_assert(eval_ml_dsa_65(), "ML-DSA-65 keygen/sign/verify must be correct at compile-time");
  SUCCEED();
}

TEST(ML_DSA, CompileTimeEval_ML_DSA_87)
{
  static_assert(eval_ml_dsa_87(), "ML-DSA-87 keygen/sign/verify must be correct at compile-time");
  SUCCEED();
}
