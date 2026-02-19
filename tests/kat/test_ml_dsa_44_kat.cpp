#include "ml_dsa/ml_dsa_44.hpp"
#include "test_helper.hpp"
#include <array>
#include <cstdint>
#include <fstream>
#include <gtest/gtest.h>

using namespace std::literals;

// Use ML-DSA-44 known answer tests to ensure functional correctness and compatibility of this ML-DSA library
// implementation with ML-DSA standard @ https://doi.org/10.6028/NIST.FIPS.204.
TEST(ML_DSA, ML_DSA_44_KnownAnswerTests)
{
  const std::string kat_file = "./kats/ml_dsa_44.kat";
  std::fstream file(kat_file);

  while (true) {
    std::string seed_line;

    if (!std::getline(file, seed_line).eof()) {
      std::string pkey_line;
      std::string skey_line;
      std::string mlen_line;
      std::string msg_line;
      std::string ctx_len_line;
      std::string ctx_line;
      std::string rnd_line;
      std::string sig_line;

      std::getline(file, pkey_line);
      std::getline(file, skey_line);
      std::getline(file, mlen_line);
      std::getline(file, msg_line);
      std::getline(file, ctx_len_line);
      std::getline(file, ctx_line);
      std::getline(file, rnd_line);
      std::getline(file, sig_line);

      const auto seed = ml_dsa_test_helper::extract_and_parse_fixed_length_hex_string<ml_dsa_44::KeygenSeedByteLen>(seed_line);
      const auto pkey = ml_dsa_test_helper::extract_and_parse_fixed_length_hex_string<ml_dsa_44::PubKeyByteLen>(pkey_line);
      const auto skey = ml_dsa_test_helper::extract_and_parse_fixed_length_hex_string<ml_dsa_44::SecKeyByteLen>(skey_line);
      const auto mlen = ml_dsa_test_helper::extract_and_parse_integer(mlen_line);
      const auto msg = ml_dsa_test_helper::extract_and_parse_variable_length_hex_string(msg_line);
      const auto ctx_len = ml_dsa_test_helper::extract_and_parse_integer(ctx_len_line);
      const auto ctx = ml_dsa_test_helper::extract_and_parse_variable_length_hex_string(ctx_line);
      const auto rnd = ml_dsa_test_helper::extract_and_parse_fixed_length_hex_string<ml_dsa_44::SigningSeedByteLen>(rnd_line);
      const auto sig = ml_dsa_test_helper::extract_and_parse_fixed_length_hex_string<ml_dsa_44::SigByteLen>(sig_line);

      EXPECT_EQ(mlen, msg.size());
      EXPECT_EQ(ctx_len, ctx.size());

      std::array<uint8_t, ml_dsa_44::PubKeyByteLen> computed_pkey{};
      std::array<uint8_t, ml_dsa_44::SecKeyByteLen> computed_skey{};
      std::array<uint8_t, ml_dsa_44::SigByteLen> computed_sig{};

      // Keygen -> Sign -> Verify
      ml_dsa_44::keygen(seed, computed_pkey, computed_skey);
      const auto has_signed = ml_dsa_44::sign(rnd, computed_skey, msg, ctx, computed_sig);
      const auto is_valid = ml_dsa_44::verify(computed_pkey, msg, ctx, computed_sig);

      // Check if computed public key, secret key and signature matches expected ones, from KAT file.
      EXPECT_EQ(pkey, computed_pkey);
      EXPECT_EQ(skey, computed_skey);
      EXPECT_EQ(sig, computed_sig);
      EXPECT_TRUE(has_signed);
      EXPECT_TRUE(is_valid);

      std::string empty_line;
      std::getline(file, empty_line);
    } else {
      break;
    }
  }

  file.close();
}

// Use ML-DSA-44 key generation known answer tests (KATs) from NIST ACVP Server to ensure functional correctness and compatibility
// of this ML-DSA library implementation with ML-DSA standard @ https://doi.org/10.6028/NIST.FIPS.204.
TEST(ML_DSA, ML_DSA_44_Keygen_ACVP_KnownAnswerTests)
{
  const std::string kat_file = "./kats/ml_dsa_44_keygen.acvp.kat";
  std::fstream file(kat_file);

  while (true) {
    std::string seed_line;

    if (!std::getline(file, seed_line).eof()) {
      std::string pkey_line;
      std::string skey_line;

      std::getline(file, pkey_line);
      std::getline(file, skey_line);

      const auto seed = ml_dsa_test_helper::extract_and_parse_fixed_length_hex_string<ml_dsa_44::KeygenSeedByteLen>(seed_line);
      const auto pkey = ml_dsa_test_helper::extract_and_parse_fixed_length_hex_string<ml_dsa_44::PubKeyByteLen>(pkey_line);
      const auto skey = ml_dsa_test_helper::extract_and_parse_fixed_length_hex_string<ml_dsa_44::SecKeyByteLen>(skey_line);

      std::array<uint8_t, ml_dsa_44::PubKeyByteLen> computed_pkey{};
      std::array<uint8_t, ml_dsa_44::SecKeyByteLen> computed_skey{};

      ml_dsa_44::keygen(seed, computed_pkey, computed_skey);

      EXPECT_EQ(pkey, computed_pkey);
      EXPECT_EQ(skey, computed_skey);

      std::string empty_line;
      std::getline(file, empty_line);
    } else {
      break;
    }
  }

  file.close();
}

// Use ML-DSA-44 sign known answer tests (KATs) from NIST ACVP Server to ensure functional correctness and compatibility
// of this ML-DSA library implementation with ML-DSA standard @ https://doi.org/10.6028/NIST.FIPS.204.
TEST(ML_DSA, ML_DSA_44_Sign_ACVP_KnownAnswerTests)
{
  const std::string kat_file = "./kats/ml_dsa_44_sign.acvp.kat";
  std::fstream file(kat_file);

  while (true) {
    std::string msg_line;

    if (!std::getline(file, msg_line).eof()) {
      std::string pkey_line;
      std::string skey_line;
      std::string ctx_line;
      std::string rnd_line;
      std::string sig_line;

      std::getline(file, pkey_line);
      std::getline(file, skey_line);
      std::getline(file, ctx_line);
      std::getline(file, rnd_line);
      std::getline(file, sig_line);

      const auto msg = ml_dsa_test_helper::extract_and_parse_variable_length_hex_string(msg_line);
      [[maybe_unused]] const auto _ = ml_dsa_test_helper::extract_and_parse_fixed_length_hex_string<ml_dsa_44::PubKeyByteLen>(pkey_line);
      const auto skey = ml_dsa_test_helper::extract_and_parse_fixed_length_hex_string<ml_dsa_44::SecKeyByteLen>(skey_line);
      const auto ctx = ml_dsa_test_helper::extract_and_parse_variable_length_hex_string(ctx_line);
      const auto rnd = ml_dsa_test_helper::extract_and_parse_fixed_length_hex_string<ml_dsa_44::SigningSeedByteLen>(rnd_line);
      const auto sig = ml_dsa_test_helper::extract_and_parse_fixed_length_hex_string<ml_dsa_44::SigByteLen>(sig_line);

      std::array<uint8_t, ml_dsa_44::SigByteLen> computed_sig{};
      const auto sign_ok = ml_dsa_44::sign(rnd, skey, msg, ctx, computed_sig);
      ASSERT_TRUE(sign_ok);

      EXPECT_EQ(sig, computed_sig);

      std::string empty_line;
      std::getline(file, empty_line);
    } else {
      break;
    }
  }

  file.close();
}

// Use ML-DSA-44 verify known answer tests (KATs) from NIST ACVP Server to ensure functional correctness and compatibility
// of this ML-DSA library implementation with ML-DSA standard @ https://doi.org/10.6028/NIST.FIPS.204.
TEST(ML_DSA, ML_DSA_44_Verify_ACVP_KnownAnswerTests)
{
  const std::string kat_file = "./kats/ml_dsa_44_verify.acvp.kat";
  std::fstream file(kat_file);

  while (true) {
    std::string msg_line;

    if (!std::getline(file, msg_line).eof()) {
      std::string pkey_line;
      std::string skey_line;
      std::string ctx_line;
      std::string sig_line;
      std::string testPassed_line;
      std::string reason_line;

      std::getline(file, pkey_line);
      std::getline(file, skey_line);
      std::getline(file, ctx_line);
      std::getline(file, sig_line);
      std::getline(file, testPassed_line);
      std::getline(file, reason_line);

      const auto msg = ml_dsa_test_helper::extract_and_parse_variable_length_hex_string(msg_line);
      const auto pkey = ml_dsa_test_helper::extract_and_parse_fixed_length_hex_string<ml_dsa_44::PubKeyByteLen>(pkey_line);
      [[maybe_unused]] const auto _ = ml_dsa_test_helper::extract_and_parse_fixed_length_hex_string<ml_dsa_44::SecKeyByteLen>(skey_line);
      const auto ctx = ml_dsa_test_helper::extract_and_parse_variable_length_hex_string(ctx_line);
      const auto sig = ml_dsa_test_helper::extract_and_parse_fixed_length_hex_string<ml_dsa_44::SigByteLen>(sig_line);
      const auto test_passed = testPassed_line.substr(testPassed_line.find("="sv) + 2, testPassed_line.size()) == "True";

      const auto is_valid = ml_dsa_44::verify(pkey, msg, ctx, sig);

      if (test_passed) {
        EXPECT_TRUE(is_valid);
      } else {
        EXPECT_FALSE(is_valid);
      }

      std::string empty_line;
      std::getline(file, empty_line);
    } else {
      break;
    }
  }

  file.close();
}

// Use ML-DSA-44 sign_internal (M'-taking, externalMu=False) known answer tests from NIST ACVP Server.
TEST(ML_DSA, ML_DSA_44_Sign_Internal_MuFalse_ACVP_KnownAnswerTests)
{
  const std::string kat_file = "./kats/ml_dsa_44_sign_internal_mu_false.acvp.kat";
  std::fstream file(kat_file);

  while (true) {
    std::string msg_line;

    if (!std::getline(file, msg_line).eof()) {
      std::string pkey_line;
      std::string skey_line;
      std::string rnd_line;
      std::string sig_line;

      std::getline(file, pkey_line);
      std::getline(file, skey_line);
      std::getline(file, rnd_line);
      std::getline(file, sig_line);

      const auto msg = ml_dsa_test_helper::extract_and_parse_variable_length_hex_string(msg_line);
      [[maybe_unused]] const auto _ = ml_dsa_test_helper::extract_and_parse_fixed_length_hex_string<ml_dsa_44::PubKeyByteLen>(pkey_line);
      const auto skey = ml_dsa_test_helper::extract_and_parse_fixed_length_hex_string<ml_dsa_44::SecKeyByteLen>(skey_line);
      const auto rnd = ml_dsa_test_helper::extract_and_parse_fixed_length_hex_string<ml_dsa_44::SigningSeedByteLen>(rnd_line);
      const auto sig = ml_dsa_test_helper::extract_and_parse_fixed_length_hex_string<ml_dsa_44::SigByteLen>(sig_line);

      std::array<uint8_t, ml_dsa_44::SigByteLen> computed_sig{};
      const auto sign_ok = ml_dsa_44::sign_internal(rnd, skey, msg, computed_sig);
      ASSERT_TRUE(sign_ok);

      EXPECT_EQ(sig, computed_sig);

      std::string empty_line;
      std::getline(file, empty_line);
    } else {
      break;
    }
  }

  file.close();
}

// Use ML-DSA-44 verify_internal (M'-taking, externalMu=False) known answer tests from NIST ACVP Server.
TEST(ML_DSA, ML_DSA_44_Verify_Internal_MuFalse_ACVP_KnownAnswerTests)
{
  const std::string kat_file = "./kats/ml_dsa_44_verify_internal_mu_false.acvp.kat";
  std::fstream file(kat_file);

  while (true) {
    std::string msg_line;

    if (!std::getline(file, msg_line).eof()) {
      std::string pkey_line;
      std::string skey_line;
      std::string sig_line;
      std::string testPassed_line;
      std::string reason_line;

      std::getline(file, pkey_line);
      std::getline(file, skey_line);
      std::getline(file, sig_line);
      std::getline(file, testPassed_line);
      std::getline(file, reason_line);

      const auto msg = ml_dsa_test_helper::extract_and_parse_variable_length_hex_string(msg_line);
      const auto pkey = ml_dsa_test_helper::extract_and_parse_fixed_length_hex_string<ml_dsa_44::PubKeyByteLen>(pkey_line);
      [[maybe_unused]] const auto _ = ml_dsa_test_helper::extract_and_parse_fixed_length_hex_string<ml_dsa_44::SecKeyByteLen>(skey_line);
      const auto sig = ml_dsa_test_helper::extract_and_parse_fixed_length_hex_string<ml_dsa_44::SigByteLen>(sig_line);
      const auto test_passed = testPassed_line.substr(testPassed_line.find("="sv) + 2, testPassed_line.size()) == "True";

      const auto is_valid = ml_dsa_44::verify_internal(pkey, msg, sig);

      if (test_passed) {
        EXPECT_TRUE(is_valid);
      } else {
        EXPECT_FALSE(is_valid);
      }

      std::string empty_line;
      std::getline(file, empty_line);
    } else {
      break;
    }
  }

  file.close();
}

// Use ML-DSA-44 HashML-DSA sign known answer tests from NIST ACVP Server (SHA3-family only).
TEST(ML_DSA, ML_DSA_44_HashSign_ACVP_KnownAnswerTests)
{
  using hash_algorithm = ml_dsa_44::hash_algorithm;

  const std::string kat_file = "./kats/ml_dsa_44_hash_sign.acvp.kat";
  std::fstream file(kat_file);

  while (true) {
    std::string msg_line;

    if (!std::getline(file, msg_line).eof()) {
      std::string pkey_line;
      std::string skey_line;
      std::string ctx_line;
      std::string hashAlg_line;
      std::string rnd_line;
      std::string sig_line;

      std::getline(file, pkey_line);
      std::getline(file, skey_line);
      std::getline(file, ctx_line);
      std::getline(file, hashAlg_line);
      std::getline(file, rnd_line);
      std::getline(file, sig_line);

      const auto msg = ml_dsa_test_helper::extract_and_parse_variable_length_hex_string(msg_line);
      [[maybe_unused]] const auto _ = ml_dsa_test_helper::extract_and_parse_fixed_length_hex_string<ml_dsa_44::PubKeyByteLen>(pkey_line);
      const auto skey = ml_dsa_test_helper::extract_and_parse_fixed_length_hex_string<ml_dsa_44::SecKeyByteLen>(skey_line);
      const auto ctx = ml_dsa_test_helper::extract_and_parse_variable_length_hex_string(ctx_line);
      const auto hash_alg = hashAlg_line.substr(hashAlg_line.find("="sv) + 2, hashAlg_line.size());
      const auto rnd = ml_dsa_test_helper::extract_and_parse_fixed_length_hex_string<ml_dsa_44::SigningSeedByteLen>(rnd_line);
      const auto sig = ml_dsa_test_helper::extract_and_parse_fixed_length_hex_string<ml_dsa_44::SigByteLen>(sig_line);

      std::array<uint8_t, ml_dsa_44::SigByteLen> computed_sig{};
      bool sign_ok = false;

      if (hash_alg == "SHA3-224") {
        sign_ok = ml_dsa_44::hash_sign<hash_algorithm::SHA3_224>(rnd, skey, msg, ctx, computed_sig);
      } else if (hash_alg == "SHA3-256") {
        sign_ok = ml_dsa_44::hash_sign<hash_algorithm::SHA3_256>(rnd, skey, msg, ctx, computed_sig);
      } else if (hash_alg == "SHA3-384") {
        sign_ok = ml_dsa_44::hash_sign<hash_algorithm::SHA3_384>(rnd, skey, msg, ctx, computed_sig);
      } else if (hash_alg == "SHA3-512") {
        sign_ok = ml_dsa_44::hash_sign<hash_algorithm::SHA3_512>(rnd, skey, msg, ctx, computed_sig);
      } else if (hash_alg == "SHAKE-128") {
        sign_ok = ml_dsa_44::hash_sign<hash_algorithm::SHAKE_128>(rnd, skey, msg, ctx, computed_sig);
      } else if (hash_alg == "SHAKE-256") {
        sign_ok = ml_dsa_44::hash_sign<hash_algorithm::SHAKE_256>(rnd, skey, msg, ctx, computed_sig);
      } else {
        FAIL() << "Unknown hash algorithm: " << hash_alg;
      }

      ASSERT_TRUE(sign_ok);
      EXPECT_EQ(sig, computed_sig);

      std::string empty_line;
      std::getline(file, empty_line);
    } else {
      break;
    }
  }

  file.close();
}

// Use ML-DSA-44 HashML-DSA verify known answer tests from NIST ACVP Server (SHA3-family only).
TEST(ML_DSA, ML_DSA_44_HashVerify_ACVP_KnownAnswerTests)
{
  using hash_algorithm = ml_dsa_44::hash_algorithm;

  const std::string kat_file = "./kats/ml_dsa_44_hash_verify.acvp.kat";
  std::fstream file(kat_file);

  while (true) {
    std::string msg_line;

    if (!std::getline(file, msg_line).eof()) {
      std::string pkey_line;
      std::string skey_line;
      std::string ctx_line;
      std::string hashAlg_line;
      std::string sig_line;
      std::string testPassed_line;
      std::string reason_line;

      std::getline(file, pkey_line);
      std::getline(file, skey_line);
      std::getline(file, ctx_line);
      std::getline(file, hashAlg_line);
      std::getline(file, sig_line);
      std::getline(file, testPassed_line);
      std::getline(file, reason_line);

      const auto msg = ml_dsa_test_helper::extract_and_parse_variable_length_hex_string(msg_line);
      const auto pkey = ml_dsa_test_helper::extract_and_parse_fixed_length_hex_string<ml_dsa_44::PubKeyByteLen>(pkey_line);
      [[maybe_unused]] const auto _ = ml_dsa_test_helper::extract_and_parse_fixed_length_hex_string<ml_dsa_44::SecKeyByteLen>(skey_line);
      const auto ctx = ml_dsa_test_helper::extract_and_parse_variable_length_hex_string(ctx_line);
      const auto hash_alg = hashAlg_line.substr(hashAlg_line.find("="sv) + 2, hashAlg_line.size());
      const auto sig = ml_dsa_test_helper::extract_and_parse_fixed_length_hex_string<ml_dsa_44::SigByteLen>(sig_line);
      const auto test_passed = testPassed_line.substr(testPassed_line.find("="sv) + 2, testPassed_line.size()) == "True";

      bool is_valid = false;

      if (hash_alg == "SHA3-224") {
        is_valid = ml_dsa_44::hash_verify<hash_algorithm::SHA3_224>(pkey, msg, ctx, sig);
      } else if (hash_alg == "SHA3-256") {
        is_valid = ml_dsa_44::hash_verify<hash_algorithm::SHA3_256>(pkey, msg, ctx, sig);
      } else if (hash_alg == "SHA3-384") {
        is_valid = ml_dsa_44::hash_verify<hash_algorithm::SHA3_384>(pkey, msg, ctx, sig);
      } else if (hash_alg == "SHA3-512") {
        is_valid = ml_dsa_44::hash_verify<hash_algorithm::SHA3_512>(pkey, msg, ctx, sig);
      } else if (hash_alg == "SHAKE-128") {
        is_valid = ml_dsa_44::hash_verify<hash_algorithm::SHAKE_128>(pkey, msg, ctx, sig);
      } else if (hash_alg == "SHAKE-256") {
        is_valid = ml_dsa_44::hash_verify<hash_algorithm::SHAKE_256>(pkey, msg, ctx, sig);
      } else {
        FAIL() << "Unknown hash algorithm: " << hash_alg;
      }

      if (test_passed) {
        EXPECT_TRUE(is_valid);
      } else {
        EXPECT_FALSE(is_valid);
      }

      std::string empty_line;
      std::getline(file, empty_line);
    } else {
      break;
    }
  }

  file.close();
}
