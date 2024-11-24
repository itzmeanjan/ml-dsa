#include "ml_dsa/ml_dsa_65.hpp"
#include "randomshake/randomshake.hpp"
#include "test_helper.hpp"
#include <cassert>
#include <gtest/gtest.h>

// Test functional correctness of ML-DSA-65 signature scheme, by
//
// - Generating random key pair.
// - Signing random message ( of mlen -bytes ).
// - Finally attempting to verify using respective public key.
//
// In case when signature is not mutated ( the good case ), it should be able to verify successfully.
// While in the case when random bit flip is introduced in signature/ public key/ message ( the bad case ) verification algorithm must fail.
static void
test_ml_dsa_65_signing(const size_t mlen, const size_t ctx_len)
{
  assert(ctx_len < 256);

  std::array<uint8_t, ml_dsa_65::KeygenSeedByteLen> seed{};
  std::array<uint8_t, ml_dsa_65::PubKeyByteLen> pkey{};
  std::array<uint8_t, ml_dsa_65::PubKeyByteLen> pkey_copy{};
  std::array<uint8_t, ml_dsa_65::SecKeyByteLen> skey{};
  std::array<uint8_t, ml_dsa_65::SigningSeedByteLen> rnd{};
  std::array<uint8_t, ml_dsa_65::SigByteLen> sig{};
  std::array<uint8_t, ml_dsa_65::SigByteLen> sig_copy{};

  std::vector<uint8_t> msg(mlen, 0);
  std::vector<uint8_t> msg_copy(mlen, 0);
  std::vector<uint8_t> ctx(ctx_len, 0);
  std::vector<uint8_t> ctx_copy(ctx_len, 0);

  auto msg_span = std::span(msg);
  auto msg_copy_span = std::span(msg_copy);
  auto ctx_span = std::span(ctx);
  auto ctx_copy_span = std::span(ctx_copy);

  randomshake::randomshake_t<192> csprng;
  csprng.generate(seed);
  csprng.generate(msg_span);
  csprng.generate(rnd);
  csprng.generate(ctx_span);

  ml_dsa_65::keygen(seed, pkey, skey);                 // Generate a valid ML-DSA-65 keypair
  ml_dsa_65::sign(rnd, skey, msg_span, ctx_span, sig); // Sign a random message with ML-DSA-65 secret ket

  std::copy(sig.begin(), sig.end(), sig_copy.begin());
  std::copy(pkey.begin(), pkey.end(), pkey_copy.begin());

  ml_dsa_test_helper::random_bit_flip(sig_copy);
  ml_dsa_test_helper::random_bit_flip(pkey_copy);

  EXPECT_TRUE(ml_dsa_65::verify(pkey, msg_span, ctx_span, sig));       // pkey is good, msg is good, ctx is good, sig is good
  EXPECT_FALSE(ml_dsa_65::verify(pkey, msg_span, ctx_span, sig_copy)); // pkey is good, msg is good, ctx is good, sig is bad
  EXPECT_FALSE(ml_dsa_65::verify(pkey_copy, msg_span, ctx_span, sig)); // pkey is bad, msg is good, ctx is good, sig is good

  if (mlen > 0) {
    std::copy(msg_span.begin(), msg_span.end(), msg_copy_span.begin());
    ml_dsa_test_helper::random_bit_flip(msg_copy_span);

    EXPECT_FALSE(ml_dsa_65::verify(pkey, msg_copy_span, ctx_span, sig)); // pkey is good, msg is bad, ctx is good, sig is good
  }
  if (ctx_len > 0) {
    std::copy(ctx_span.begin(), ctx_span.end(), ctx_copy_span.begin());
    ml_dsa_test_helper::random_bit_flip(ctx_copy_span);

    EXPECT_FALSE(ml_dsa_65::verify(pkey, msg_span, ctx_copy_span, sig)); // pkey is good, msg is good, ctx is bad, sig is good
  }
}

TEST(ML_DSA, ML_DSA_65_KeygenSignVerifyFlow)
{
  for (size_t mlen = 0; mlen < 33; mlen++) {
    for (size_t ctx_len = 0; ctx_len < 256; ctx_len++) {
      test_ml_dsa_65_signing(mlen, ctx_len);
    }
  }
}
