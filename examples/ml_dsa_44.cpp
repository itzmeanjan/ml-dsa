#include "ml_dsa/ml_dsa_44.hpp"
#include "randomshake/randomshake.hpp"
#include <cassert>
#include <iomanip>
#include <iostream>
#include <vector>

// Given a bytearray of length N, this function converts it to human readable hex string of length 2*N | N >= 0.
static inline std::string
to_hex(std::span<const uint8_t> bytes)
{
  std::stringstream ss;
  ss << std::hex;

  for (size_t i = 0; i < bytes.size(); i++) {
    ss << std::setw(2) << std::setfill('0') << static_cast<uint32_t>(bytes[i]);
  }

  return ss.str();
}

int
main()
{
  // Byte length of message, can be anything >= 0.
  constexpr size_t mlen = 32;
  constexpr size_t ctx_len = 8;

  std::vector<uint8_t> seed(ml_dsa_44::KeygenSeedByteLen);
  std::vector<uint8_t> pubkey(ml_dsa_44::PubKeyByteLen);
  std::vector<uint8_t> seckey(ml_dsa_44::SecKeyByteLen);
  std::vector<uint8_t> rnd(ml_dsa_44::SigningSeedByteLen);
  std::vector<uint8_t> msg(mlen);
  std::vector<uint8_t> ctx(ctx_len);
  std::vector<uint8_t> sig(ml_dsa_44::SigByteLen);

  auto seed_span = std::span<uint8_t, ml_dsa_44::KeygenSeedByteLen>(seed);
  auto rnd_span = std::span<uint8_t, ml_dsa_44::SigningSeedByteLen>(rnd);
  auto msg_span = std::span(msg);
  auto ctx_span = std::span(ctx);
  auto pubkey_span = std::span<uint8_t, ml_dsa_44::PubKeyByteLen>(pubkey);
  auto seckey_span = std::span<uint8_t, ml_dsa_44::SecKeyByteLen>(seckey);
  auto sig_span = std::span<uint8_t, ml_dsa_44::SigByteLen>(sig);

  randomshake::randomshake_t<128> csprng;

  csprng.generate(seed_span);
  csprng.generate(msg_span);
  csprng.generate(ctx_span);

  // Filling `rnd` with random bytes, invokes default and recommended "hedged" version of signing.
  // While filling it with 0, invokes "deterministic" signing mode.
  csprng.generate(rnd_span);

  ml_dsa_44::keygen(seed_span, pubkey_span, seckey_span);
  const bool has_signed = ml_dsa_44::sign(rnd_span, seckey_span, msg_span, ctx_span, sig_span);
  const bool is_valid = ml_dsa_44::verify(pubkey_span, msg_span, ctx_span, sig_span);

  std::cout << "ML-DSA-44 @ NIST security level 2\n";
  std::cout << "Seed      : " << to_hex(seed_span) << "\n";
  std::cout << "Pubkey    : " << to_hex(pubkey_span) << "\n";
  std::cout << "Seckey    : " << to_hex(seckey_span) << "\n";
  std::cout << "Message   : " << to_hex(msg_span) << "\n";
  std::cout << "Context   : " << to_hex(ctx_span) << "\n";
  std::cout << "Signature : " << to_hex(sig_span) << "\n";
  std::cout << "Verified   : " << std::boolalpha << is_valid << "\n";

  assert(has_signed);
  assert(is_valid);

  return EXIT_SUCCESS;
}
