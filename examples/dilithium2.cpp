#include "ml_dsa/dilithium2.hpp"
#include "ml_dsa/internals/rng/prng.hpp"
#include <cassert>
#include <iostream>
#include <vector>

// Given a bytearray of length N, this function converts it to human readable
// hex string of length N << 1 | N >= 0
static inline const std::string
to_hex(std::span<const uint8_t> bytes)
{
  std::stringstream ss;
  ss << std::hex;

  for (size_t i = 0; i < bytes.size(); i++) {
    ss << std::setw(2) << std::setfill('0') << static_cast<uint32_t>(bytes[i]);
  }

  return ss.str();
}

// Compile it with
//
// g++ -std=c++20 -O3 -march=native -I ./include -I ./sha3/include examples/dilithium2.cpp
int
main()
{
  constexpr size_t slen = 32; // seed length
  constexpr size_t mlen = 32; // message length ( can be anything >= 1 )

  std::vector<uint8_t> seed(slen);
  std::vector<uint8_t> rnd(slen);
  std::vector<uint8_t> msg(mlen);
  std::vector<uint8_t> pubkey(dilithium2::PubKeyLen);
  std::vector<uint8_t> seckey(dilithium2::SecKeyLen);
  std::vector<uint8_t> sig(dilithium2::SigLen);

  auto _seed = std::span<uint8_t, slen>(seed);
  auto _rnd = std::span<uint8_t, slen>(rnd);
  auto _msg = std::span(msg);
  auto _pubkey = std::span<uint8_t, dilithium2::PubKeyLen>(pubkey);
  auto _seckey = std::span<uint8_t, dilithium2::SecKeyLen>(seckey);
  auto _sig = std::span<uint8_t, dilithium2::SigLen>(sig);

  // generate random 32 -bytes seed & N -bytes message ( to be signed )
  prng::prng_t prng;

  prng.read(_seed);
  prng.read(_msg);

  // Key generation -> signing -> verification
  dilithium2::keygen(_seed, _pubkey, _seckey);
  dilithium2::sign(_rnd, _seckey, _msg, _sig);
  const bool flg = dilithium2::verify(_pubkey, _msg, _sig);

  std::cout << "Dilithium @ NIST security level 2\n";
  std::cout << "seed      : " << to_hex(_seed) << "\n";
  std::cout << "pubkey    : " << to_hex(_pubkey) << "\n";
  std::cout << "seckey    : " << to_hex(_seckey) << "\n";
  std::cout << "message   : " << to_hex(_msg) << "\n";
  std::cout << "signature : " << to_hex(_sig) << "\n";
  std::cout << "verified   : " << std::boolalpha << flg << "\n";

  // check that signature verification passed !
  assert(flg);

  return EXIT_SUCCESS;
}
