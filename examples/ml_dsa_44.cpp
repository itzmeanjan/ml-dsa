#include "ml_dsa/ml_dsa_44.hpp"
#include "randomshake/randomshake.hpp" // IWYU pragma: keep
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <span>
#include <sstream>
#include <string>
#include <vector>

namespace {

// Given a bytearray of length N, this function converts it to human readable hex formatted string of length 2*N | N >= 0.
inline std::string
to_hex(std::span<const uint8_t> bytes)
{
  std::stringstream ss;
  ss << std::hex;

  for (const unsigned char byte : bytes) {
    ss << std::setw(2) << std::setfill('0') << static_cast<uint32_t>(byte);
  }

  return ss.str();
}

}

// Build with CMake, from this directory:
//   cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build
// Or from the repository root:
//   cmake -B build -DML_DSA_BUILD_EXAMPLES=ON -DCMAKE_BUILD_TYPE=Release && cmake --build build -j
int
main()
{
  // Seed required for keypair generation
  std::vector<uint8_t> seed(ml_dsa_44::KeygenSeedByteLen, 0);
  auto seed_span = std::span<uint8_t, ml_dsa_44::KeygenSeedByteLen>(seed);

  // Public/ private keypair
  std::vector<uint8_t> pkey(ml_dsa_44::PubKeyByteLen, 0);
  std::vector<uint8_t> skey(ml_dsa_44::SecKeyByteLen, 0);

  auto pkey_span = std::span<uint8_t, ml_dsa_44::PubKeyByteLen>(pkey);
  auto skey_span = std::span<uint8_t, ml_dsa_44::SecKeyByteLen>(skey);

  // Message and optional context string
  std::vector<uint8_t> msg(32, 0);
  std::vector<uint8_t> ctx(8, 0);

  auto msg_span = std::span(msg);
  auto ctx_span = std::span(ctx);

  // Seed required for signing (deterministic vs hedged)
  std::vector<uint8_t> rnd(ml_dsa_44::SigningSeedByteLen, 0);
  auto rnd_span = std::span<uint8_t, ml_dsa_44::SigningSeedByteLen>(rnd);

  // Signature
  std::vector<uint8_t> sig(ml_dsa_44::SigByteLen, 0);
  auto sig_span = std::span<uint8_t, ml_dsa_44::SigByteLen>(sig);

  // Pseudo-randomness source
  randomshake::randomshake_t csprng{};

  // Fill up seeds and message using PRNG
  csprng.generate(seed_span);
  csprng.generate(rnd_span);
  csprng.generate(msg_span);
  csprng.generate(ctx_span);

  // Generate a keypair
  ml_dsa_44::keygen(seed_span, pkey_span, skey_span);

  // Sign message
  const bool has_signed = ml_dsa_44::sign(rnd_span, skey_span, msg_span, ctx_span, sig_span);
  // Verify signature
  const bool is_valid = ml_dsa_44::verify(pkey_span, msg_span, ctx_span, sig_span);

  // Check that signing was successful and signature is valid
  assert(has_signed);
  (void)has_signed;
  assert(is_valid);

  std::cout << "ML-DSA-44 @ NIST security level 2\n";
  std::cout << "Seed      : " << to_hex(seed_span) << "\n";
  std::cout << "Pubkey    : " << to_hex(pkey_span) << "\n";
  std::cout << "Seckey    : " << to_hex(skey_span) << "\n";
  std::cout << "Message   : " << to_hex(msg_span) << "\n";
  std::cout << "Context   : " << to_hex(ctx_span) << "\n";
  std::cout << "Signature : " << to_hex(sig_span) << "\n";
  std::cout << "Verified  : " << std::boolalpha << is_valid << "\n";

  return EXIT_SUCCESS;
}
