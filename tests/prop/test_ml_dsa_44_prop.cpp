#include "ml_dsa/ml_dsa_44.hpp"
#include "randomshake/randomshake.hpp"
#include "test_helper.hpp"
#include <cassert>
#include <gtest/gtest.h>
#include <limits>
#include <random>

constexpr size_t NUMBER_OF_TEST_ITERATIONS = 100;
constexpr size_t MAX_MSG_BYTE_LEN = 33;
constexpr size_t MAX_CTX_BYTE_LEN = std::numeric_limits<uint8_t>::max();

namespace {

void
random_mutate_span(std::span<const uint8_t> src, std::span<uint8_t> dst)
{
  assert(src.size() == dst.size());

  if (!src.empty()) {
    std::copy(src.begin(), src.end(), dst.begin());
    ml_dsa_test_helper::random_bit_flip(dst);
  }
}

// Test functional correctness of ML-DSA-44 signature scheme, by
//
// - Generating random key pair.
// - Signing random message ( of mlen -bytes ).
// - Finally attempting to verify using respective public key.
//
// In case when signature is not mutated ( the good case ), it should be able to verify successfully.
// While in the case when random bit flip is introduced in signature/ public key/ message ( the bad case ) verification algorithm must fail.
void
test_ml_dsa_44_signing(const size_t mlen, const size_t ctx_len, auto sign_fn, auto verify_fn)
{
  assert(ctx_len < 256);

  std::array<uint8_t, ml_dsa_44::KeygenSeedByteLen> seed{};
  std::array<uint8_t, ml_dsa_44::PubKeyByteLen> pkey{};
  std::array<uint8_t, ml_dsa_44::PubKeyByteLen> pkey_copy{};
  std::array<uint8_t, ml_dsa_44::SecKeyByteLen> skey{};
  std::array<uint8_t, ml_dsa_44::SigningSeedByteLen> rnd{};
  std::array<uint8_t, ml_dsa_44::SigByteLen> sig{};
  std::array<uint8_t, ml_dsa_44::SigByteLen> sig_copy{};

  std::vector<uint8_t> msg(mlen, 0);
  std::vector<uint8_t> msg_copy(mlen, 0);
  std::vector<uint8_t> ctx(ctx_len, 0);
  std::vector<uint8_t> ctx_copy(ctx_len, 0);

  auto msg_span = std::span(msg);
  auto msg_copy_span = std::span(msg_copy);
  auto ctx_span = std::span(ctx);
  auto ctx_copy_span = std::span(ctx_copy);

  randomshake::randomshake_t csprng;
  csprng.generate(seed);
  csprng.generate(msg_span);
  csprng.generate(rnd);
  csprng.generate(ctx_span);

  ml_dsa_44::keygen(seed, pkey, skey);
  EXPECT_TRUE(sign_fn(rnd, skey, msg_span, ctx_span, sig));

  std::copy(sig.begin(), sig.end(), sig_copy.begin());
  std::copy(pkey.begin(), pkey.end(), pkey_copy.begin());

  ml_dsa_test_helper::random_bit_flip(sig_copy);
  ml_dsa_test_helper::random_bit_flip(pkey_copy);
  random_mutate_span(msg_span, msg_copy_span);
  random_mutate_span(ctx_span, ctx_copy_span);

  EXPECT_TRUE(verify_fn(pkey, msg_span, ctx_span, sig));                       // pkey is good, msg is good, ctx is good, sig is good
  EXPECT_FALSE(verify_fn(pkey, msg_span, ctx_span, sig_copy));                 // pkey is good, msg is good, ctx is good, sig is bad
  EXPECT_FALSE(verify_fn(pkey_copy, msg_span, ctx_span, sig));                 // pkey is bad, msg is good, ctx is good, sig is good
  EXPECT_TRUE(mlen == 0 || !verify_fn(pkey, msg_copy_span, ctx_span, sig));    // pkey is good, msg is bad, ctx is good, sig is good
  EXPECT_TRUE(ctx_len == 0 || !verify_fn(pkey, msg_span, ctx_copy_span, sig)); // pkey is good, msg is good, ctx is bad, sig is good
}

} // namespace

TEST(ML_DSA, ML_DSA_44_KeygenSignVerifyFlow)
{
  using prng_t = randomshake::randomshake_t<size_t, randomshake::xof_kind_t::TURBOSHAKE256>;
  prng_t csprng;

  std::uniform_int_distribution<size_t> msg_len_dist(0, MAX_MSG_BYTE_LEN);
  std::uniform_int_distribution<size_t> ctx_len_dist(0, MAX_CTX_BYTE_LEN);

  for (size_t i = 0; i < NUMBER_OF_TEST_ITERATIONS; i++) {
    const size_t msg_len = msg_len_dist(csprng);
    const size_t ctx_len = ctx_len_dist(csprng);

    test_ml_dsa_44_signing(msg_len, ctx_len, ml_dsa_44::sign, ml_dsa_44::verify);
  }
}

// Test HashML-DSA sign/verify for ML-DSA-44 across all SHA3-family hash algorithms.
TEST(ML_DSA, ML_DSA_44_HashSignVerifyFlow)
{
  using ha = ml_dsa_44::hash_algorithm;
  using prng_t = randomshake::randomshake_t<size_t, randomshake::xof_kind_t::TURBOSHAKE256>;
  prng_t csprng;

  std::uniform_int_distribution<size_t> msg_len_dist(0, MAX_MSG_BYTE_LEN);
  std::uniform_int_distribution<size_t> ctx_len_dist(0, MAX_CTX_BYTE_LEN);

  auto test_with_hash = [&](auto sign_fn, auto verify_fn) {
    const size_t msg_len = msg_len_dist(csprng);
    const size_t ctx_len = ctx_len_dist(csprng);

    test_ml_dsa_44_signing(msg_len, ctx_len, sign_fn, verify_fn);
  };

  test_with_hash([](auto&... args) { return ml_dsa_44::hash_sign<ha::SHA3_224>(args...); }, [](auto&... args) { return ml_dsa_44::hash_verify<ha::SHA3_224>(args...); });
  test_with_hash([](auto&... args) { return ml_dsa_44::hash_sign<ha::SHA3_256>(args...); }, [](auto&... args) { return ml_dsa_44::hash_verify<ha::SHA3_256>(args...); });
  test_with_hash([](auto&... args) { return ml_dsa_44::hash_sign<ha::SHA3_384>(args...); }, [](auto&... args) { return ml_dsa_44::hash_verify<ha::SHA3_384>(args...); });
  test_with_hash([](auto&... args) { return ml_dsa_44::hash_sign<ha::SHA3_512>(args...); }, [](auto&... args) { return ml_dsa_44::hash_verify<ha::SHA3_512>(args...); });
  test_with_hash([](auto&... args) { return ml_dsa_44::hash_sign<ha::SHAKE_128>(args...); }, [](auto&... args) { return ml_dsa_44::hash_verify<ha::SHAKE_128>(args...); });
  test_with_hash([](auto&... args) { return ml_dsa_44::hash_sign<ha::SHAKE_256>(args...); }, [](auto&... args) { return ml_dsa_44::hash_verify<ha::SHAKE_256>(args...); });
}
