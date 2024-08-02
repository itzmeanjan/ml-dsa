#include "ml_dsa/ml_dsa_44.hpp"
#include "test_helper.hpp"
#include <array>
#include <cstdint>
#include <fstream>
#include <gtest/gtest.h>

using namespace std::literals;

// Use ML-DSA-44 known answer tests to ensure functional correctness and compatibility of this ML-DSA library
// implementation with ML-DSA draft standard @ https://doi.org/10.6028/NIST.FIPS.204.ipd.
TEST(ML_DSA, ML_DSA_44_KnownAnswerTests)
{
  const std::string kat_file = "./kats/ml_dsa_44.kat";
  std::fstream file(kat_file);

  while (true) {
    std::string seed0;

    if (!std::getline(file, seed0).eof()) {
      auto seed1 = std::string_view(seed0);
      auto seed2 = seed1.substr(seed1.find("="sv) + 2, seed1.size());
      const auto seed = ml_dsa_test_helper::from_hex<ml_dsa_44::KeygenSeedByteLen>(seed2);

      std::string pkey0;
      std::getline(file, pkey0);

      auto pkey1 = std::string_view(pkey0);
      auto pkey2 = pkey1.substr(pkey1.find("="sv) + 2, pkey1.size());
      const auto pkey = ml_dsa_test_helper::from_hex<ml_dsa_44::PubKeyByteLen>(pkey2); // Expected public key

      std::string skey0;
      std::getline(file, skey0);

      auto skey1 = std::string_view(skey0);
      auto skey2 = skey1.substr(skey1.find("="sv) + 2, skey1.size());
      const auto skey = ml_dsa_test_helper::from_hex<ml_dsa_44::SecKeyByteLen>(skey2); // Expected secret key

      std::string mlen0;
      std::getline(file, mlen0);
      size_t mlen = 0; // Message byte length

      auto mlen1 = std::string_view(mlen0);
      auto mlen2 = mlen1.substr(mlen1.find("="sv) + 2, mlen1.size());
      std::from_chars(mlen2.data(), mlen2.data() + mlen2.size(), mlen);

      std::string msg0;
      std::getline(file, msg0);

      auto msg1 = std::string_view(msg0);
      auto msg2 = msg1.substr(msg1.find("="sv) + 2, msg1.size());
      const auto msg = ml_dsa_test_helper::from_hex(msg2);
      const auto msg_span = std::span(msg); // Message to be signed

      std::string rnd0;
      std::getline(file, rnd0);

      auto rnd1 = std::string_view(rnd0);
      auto rnd2 = rnd1.substr(rnd1.find("="sv) + 2, rnd1.size());
      const auto rnd = ml_dsa_test_helper::from_hex<ml_dsa_44::SigningSeedByteLen>(rnd2); // Randomness for "randomized" message signing

      std::string sig0;
      std::getline(file, sig0);

      auto sig1 = std::string_view(sig0);
      auto sig2 = sig1.substr(sig1.find("="sv) + 2, sig1.size());
      const auto sig = ml_dsa_test_helper::from_hex<ml_dsa_44::SigByteLen>(sig2); // Expected signature

      std::array<uint8_t, ml_dsa_44::PubKeyByteLen> computed_pkey{};
      std::array<uint8_t, ml_dsa_44::SecKeyByteLen> computed_skey{};
      std::array<uint8_t, ml_dsa_44::SigByteLen> computed_sig{};

      // Keygen -> Sign -> Verify
      ml_dsa_44::keygen(seed, computed_pkey, computed_skey);
      ml_dsa_44::sign(rnd, computed_skey, msg_span, computed_sig);
      const auto is_valid = ml_dsa_44::verify(computed_pkey, msg_span, computed_sig);

      // Check if computed public key, secret key and signature matches expected ones, from KAT file.
      EXPECT_EQ(pkey, computed_pkey);
      EXPECT_EQ(skey, computed_skey);
      EXPECT_EQ(sig, computed_sig);
      EXPECT_TRUE(is_valid);

      std::string empty_line;
      std::getline(file, empty_line);
    } else {
      break;
    }
  }

  file.close();
}
