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

// Use ML-DSA-44 sign_internal known answer tests (KATs) from NIST ACVP Server to ensure functional correctness and compatibility
// of this ML-DSA library implementation with ML-DSA standard @ https://doi.org/10.6028/NIST.FIPS.204.
TEST(ML_DSA, ML_DSA_44_Sign_Internal_ACVP_KnownAnswerTests)
{
  const std::string kat_file = "./kats/ml_dsa_44_sign_internal.acvp.kat";
  std::fstream file(kat_file);

  while (true) {
    std::string mu_line;

    if (!std::getline(file, mu_line).eof()) {
      std::string pkey_line;
      std::string skey_line;
      std::string rnd_line;
      std::string sig_line;

      std::getline(file, pkey_line);
      std::getline(file, skey_line);
      std::getline(file, rnd_line);
      std::getline(file, sig_line);

      const auto mu = ml_dsa_test_helper::extract_and_parse_fixed_length_hex_string<ml_dsa_44::MessageRepresentativeByteLen>(mu_line);
      [[maybe_unused]] const auto _ = ml_dsa_test_helper::extract_and_parse_fixed_length_hex_string<ml_dsa_44::PubKeyByteLen>(pkey_line);
      const auto skey = ml_dsa_test_helper::extract_and_parse_fixed_length_hex_string<ml_dsa_44::SecKeyByteLen>(skey_line);
      const auto rnd = ml_dsa_test_helper::extract_and_parse_fixed_length_hex_string<ml_dsa_44::SigningSeedByteLen>(rnd_line);
      const auto sig = ml_dsa_test_helper::extract_and_parse_fixed_length_hex_string<ml_dsa_44::SigByteLen>(sig_line);

      std::array<uint8_t, ml_dsa_44::SigByteLen> computed_sig{};
      const auto sign_ok = ml_dsa_44::sign_internal(rnd, skey, mu, computed_sig);
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

// Use ML-DSA-44 verify_internal known answer tests (KATs) from NIST ACVP Server to ensure functional correctness and compatibility
// of this ML-DSA library implementation with ML-DSA standard @ https://doi.org/10.6028/NIST.FIPS.204.
TEST(ML_DSA, ML_DSA_44_Verify_Internal_ACVP_KnownAnswerTests)
{
  const std::string kat_file = "./kats/ml_dsa_44_verify_internal.acvp.kat";
  std::fstream file(kat_file);

  while (true) {
    std::string mu_line;

    if (!std::getline(file, mu_line).eof()) {
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

      const auto mu = ml_dsa_test_helper::extract_and_parse_fixed_length_hex_string<ml_dsa_44::MessageRepresentativeByteLen>(mu_line);
      const auto pkey = ml_dsa_test_helper::extract_and_parse_fixed_length_hex_string<ml_dsa_44::PubKeyByteLen>(pkey_line);
      [[maybe_unused]] const auto _ = ml_dsa_test_helper::extract_and_parse_fixed_length_hex_string<ml_dsa_44::SecKeyByteLen>(skey_line);
      const auto sig = ml_dsa_test_helper::extract_and_parse_fixed_length_hex_string<ml_dsa_44::SigByteLen>(sig_line);
      const auto test_passed = testPassed_line.substr(testPassed_line.find("="sv) + 2, testPassed_line.size()) == "True";

      const auto is_valid = ml_dsa_44::verify_internal(pkey, mu, sig);

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
