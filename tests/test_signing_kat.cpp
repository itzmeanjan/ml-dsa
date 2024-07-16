#include "ml_dsa/dilithium2.hpp"
#include "ml_dsa/dilithium3.hpp"
#include "ml_dsa/dilithium5.hpp"
#include <charconv>
#include <cstdint>
#include <fstream>
#include <gtest/gtest.h>
#include <string>
#include <string_view>
#include <vector>

using namespace std::literals;

// Given a hex encoded string of length 2*L, this routine can be used for parsing it as a byte array of length L.
static inline std::vector<uint8_t>
from_hex(std::string_view hex)
{
  const size_t hlen = hex.length();
  assert(hlen % 2 == 0);

  const size_t blen = hlen / 2;
  std::vector<uint8_t> res(blen, 0);

  for (size_t i = 0; i < blen; i++) {
    const size_t off = i * 2;

    uint8_t byte = 0;
    auto sstr = hex.substr(off, 2);
    std::from_chars(sstr.data(), sstr.data() + 2, byte, 16);

    res[i] = byte;
  }

  return res;
}

// Use Dilithium2 known answer tests to ensure functional correctness and
// compatibility of this Dilithium library implementation with the specification
// https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf.
TEST(Dilithium, Dilithium2KnownAnswerTests)
{
  const std::string kat_file = "./kats/ml_dsa_44.kat";
  std::fstream file(kat_file);

  while (true) {
    std::string seed0;

    if (!std::getline(file, seed0).eof()) {
      auto seed1 = std::string_view(seed0);
      auto seed2 = seed1.substr(seed1.find("="sv) + 2, seed1.size());
      auto seed = from_hex(seed2);
      auto _seed = std::span<uint8_t, 32>(seed); // 32 -bytes seed

      std::string pkey0;
      std::getline(file, pkey0);

      auto pkey1 = std::string_view(pkey0);
      auto pkey2 = pkey1.substr(pkey1.find("="sv) + 2, pkey1.size());
      auto pkey = from_hex(pkey2); // Expected public key

      std::string skey0;
      std::getline(file, skey0);

      auto skey1 = std::string_view(skey0);
      auto skey2 = skey1.substr(skey1.find("="sv) + 2, skey1.size());
      auto skey = from_hex(skey2); // Expected secret key

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
      auto msg = from_hex(msg2);
      auto _msg = std::span(msg); // Message to be signed

      std::string rnd0;
      std::getline(file, rnd0);

      auto rnd1 = std::string_view(rnd0);
      auto rnd2 = rnd1.substr(rnd1.find("="sv) + 2, rnd1.size());
      auto rnd = from_hex(rnd2);
      auto _rnd = std::span<uint8_t, 32>(rnd); // Randomness for "randomized" message signing

      std::string sig0;
      std::getline(file, sig0);

      auto sig1 = std::string_view(sig0);
      auto sig2 = sig1.substr(sig1.find("="sv) + 2, sig1.size());
      auto sig = from_hex(sig2); // Expected signature

      std::vector<uint8_t> _pkey(dilithium2::PubKeyLen, 0);
      std::vector<uint8_t> _skey(dilithium2::SecKeyLen, 0);
      std::vector<uint8_t> _sig(dilithium2::SigLen, 0);

      auto __pkey = std::span<uint8_t, dilithium2::PubKeyLen>(_pkey); // Computed public key
      auto __skey = std::span<uint8_t, dilithium2::SecKeyLen>(_skey); // Computed secret key
      auto __sig = std::span<uint8_t, dilithium2::SigLen>(_sig);      // Computed signature

      // Keygen -> Sign -> Verify
      dilithium2::keygen(_seed, __pkey, __skey);
      dilithium2::sign(_rnd, __skey, _msg, __sig);
      const auto f = dilithium2::verify(__pkey, _msg, __sig);

      // Check if computed public key, secret key and signature matches expected
      // ones, from KAT file.
      EXPECT_EQ(pkey, _pkey);
      EXPECT_EQ(skey, _skey);
      EXPECT_EQ(sig, _sig);
      EXPECT_TRUE(f);

      std::string empty_line;
      std::getline(file, empty_line);
    } else {
      break;
    }
  }

  file.close();
}

// Use Dilithium3 known answer tests to ensure functional correctness and
// compatibility of this Dilithium library implementation with the specification
// https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf.
TEST(Dilithium, Dilithium3KnownAnswerTests)
{
  const std::string kat_file = "./kats/ml_dsa_65.kat";
  std::fstream file(kat_file);

  while (true) {
    std::string seed0;

    if (!std::getline(file, seed0).eof()) {
      auto seed1 = std::string_view(seed0);
      auto seed2 = seed1.substr(seed1.find("="sv) + 2, seed1.size());
      auto seed = from_hex(seed2);
      auto _seed = std::span<uint8_t, 32>(seed); // 32 -bytes seed

      std::string pkey0;
      std::getline(file, pkey0);

      auto pkey1 = std::string_view(pkey0);
      auto pkey2 = pkey1.substr(pkey1.find("="sv) + 2, pkey1.size());
      auto pkey = from_hex(pkey2); // Expected public key

      std::string skey0;
      std::getline(file, skey0);

      auto skey1 = std::string_view(skey0);
      auto skey2 = skey1.substr(skey1.find("="sv) + 2, skey1.size());
      auto skey = from_hex(skey2); // Expected secret key

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
      auto msg = from_hex(msg2);
      auto _msg = std::span(msg); // Message to be signed

      std::string rnd0;
      std::getline(file, rnd0);

      auto rnd1 = std::string_view(rnd0);
      auto rnd2 = rnd1.substr(rnd1.find("="sv) + 2, rnd1.size());
      auto rnd = from_hex(rnd2);
      auto _rnd = std::span<uint8_t, 32>(rnd); // Randomness for "randomized" message signing

      std::string sig0;
      std::getline(file, sig0);

      auto sig1 = std::string_view(sig0);
      auto sig2 = sig1.substr(sig1.find("="sv) + 2, sig1.size());
      auto sig = from_hex(sig2); // Expected signature

      std::vector<uint8_t> _pkey(dilithium3::PubKeyLen, 0);
      std::vector<uint8_t> _skey(dilithium3::SecKeyLen, 0);
      std::vector<uint8_t> _sig(dilithium3::SigLen, 0);

      auto __pkey = std::span<uint8_t, dilithium3::PubKeyLen>(_pkey); // Computed public key
      auto __skey = std::span<uint8_t, dilithium3::SecKeyLen>(_skey); // Computed secret key
      auto __sig = std::span<uint8_t, dilithium3::SigLen>(_sig);      // Computed signature

      // Keygen -> Sign -> Verify
      dilithium3::keygen(_seed, __pkey, __skey);
      dilithium3::sign(_rnd, __skey, _msg, __sig);
      const auto f = dilithium3::verify(__pkey, _msg, __sig);

      // Check if computed public key, secret key and signature matches expected
      // ones, from KAT file.
      EXPECT_EQ(pkey, _pkey);
      EXPECT_EQ(skey, _skey);
      EXPECT_EQ(sig, _sig);
      EXPECT_TRUE(f);

      std::string empty_line;
      std::getline(file, empty_line);
    } else {
      break;
    }
  }

  file.close();
}

// Use Dilithium5 known answer tests to ensure functional correctness and
// compatibility of this Dilithium library implementation with the specification
// https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf.
TEST(Dilithium, Dilithium5KnownAnswerTests)
{
  const std::string kat_file = "./kats/ml_dsa_87.kat";
  std::fstream file(kat_file);

  while (true) {
    std::string seed0;

    if (!std::getline(file, seed0).eof()) {
      auto seed1 = std::string_view(seed0);
      auto seed2 = seed1.substr(seed1.find("="sv) + 2, seed1.size());
      auto seed = from_hex(seed2);
      auto _seed = std::span<uint8_t, 32>(seed); // 32 -bytes seed

      std::string pkey0;
      std::getline(file, pkey0);

      auto pkey1 = std::string_view(pkey0);
      auto pkey2 = pkey1.substr(pkey1.find("="sv) + 2, pkey1.size());
      auto pkey = from_hex(pkey2); // Expected public key

      std::string skey0;
      std::getline(file, skey0);

      auto skey1 = std::string_view(skey0);
      auto skey2 = skey1.substr(skey1.find("="sv) + 2, skey1.size());
      auto skey = from_hex(skey2); // Expected secret key

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
      auto msg = from_hex(msg2);
      auto _msg = std::span(msg); // Message to be signed

      std::string rnd0;
      std::getline(file, rnd0);

      auto rnd1 = std::string_view(rnd0);
      auto rnd2 = rnd1.substr(rnd1.find("="sv) + 2, rnd1.size());
      auto rnd = from_hex(rnd2);
      auto _rnd = std::span<uint8_t, 32>(rnd); // Randomness for "randomized" message signing

      std::string sig0;
      std::getline(file, sig0);

      auto sig1 = std::string_view(sig0);
      auto sig2 = sig1.substr(sig1.find("="sv) + 2, sig1.size());
      auto sig = from_hex(sig2); // Expected signature

      std::vector<uint8_t> _pkey(dilithium5::PubKeyLen, 0);
      std::vector<uint8_t> _skey(dilithium5::SecKeyLen, 0);
      std::vector<uint8_t> _sig(dilithium5::SigLen, 0);

      auto __pkey = std::span<uint8_t, dilithium5::PubKeyLen>(_pkey); // Computed public key
      auto __skey = std::span<uint8_t, dilithium5::SecKeyLen>(_skey); // Computed secret key
      auto __sig = std::span<uint8_t, dilithium5::SigLen>(_sig);      // Computed signature

      // Keygen -> Sign -> Verify
      dilithium5::keygen(_seed, __pkey, __skey);
      dilithium5::sign(_rnd, __skey, _msg, __sig);
      const auto f = dilithium5::verify(__pkey, _msg, __sig);

      // Check if computed public key, secret key and signature matches expected
      // ones, from KAT file.
      EXPECT_EQ(pkey, _pkey);
      EXPECT_EQ(skey, _skey);
      EXPECT_EQ(sig, _sig);
      EXPECT_TRUE(f);

      std::string empty_line;
      std::getline(file, empty_line);
    } else {
      break;
    }
  }

  file.close();
}
