#include "dilithium2.hpp"
#include "prng.hpp"
#include <cassert>
#include <iostream>
#include <vector>

// Compile it with
//
// g++ -std=c++20 -O3 -march=native -I ./include -I ./sha3/include examples/dilithium2.cpp
int
main()
{
  constexpr size_t slen = 32; // seed length
  constexpr size_t mlen = 32; // message length ( can be anything >= 1 )

  std::vector<uint8_t> seed(slen);
  std::vector<uint8_t> msg(mlen);
  std::vector<uint8_t> pubkey(dilithium2::PubKeyLen);
  std::vector<uint8_t> seckey(dilithium2::SecKeyLen);
  std::vector<uint8_t> sig(dilithium2::SigLen);

  auto _seed = std::span<uint8_t, slen>(seed);
  auto _msg = std::span(msg);
  auto _pubkey = std::span<uint8_t, dilithium2::PubKeyLen>(pubkey);
  auto _seckey = std::span<uint8_t, dilithium2::SecKeyLen>(seckey);
  auto _sig = std::span<uint8_t, dilithium2::SigLen>(sig);

  // generate random 32 -bytes seed & N -bytes message ( to be signed )
  prng::prng_t prng;

  prng.read(_seed);
  prng.read(_msg);

  bool flg = false;

  // Key generation -> signing -> verification
  dilithium2::keygen(_seed, _pubkey, _seckey);
  dilithium2::sign(_seckey, _msg, _sig, {});

  // Default option is deterministic signing, for randomized signing, use
  // dilithium2::sign<true>(_seckey, _msg, _sig, std::array<uint8_t, 64>{});

  flg = dilithium2::verify(_pubkey, _msg, _sig);

  std::cout << "Dilithium @ NIST security level 2\n";
  std::cout << "seed      : " << dilithium_utils::to_hex(_seed) << "\n";
  std::cout << "pubkey    : " << dilithium_utils::to_hex(_pubkey) << "\n";
  std::cout << "seckey    : " << dilithium_utils::to_hex(_seckey) << "\n";
  std::cout << "message   : " << dilithium_utils::to_hex(_msg) << "\n";
  std::cout << "signature : " << dilithium_utils::to_hex(_sig) << "\n";
  std::cout << "verified   : " << std::boolalpha << flg << "\n";

  // check that signature verification passed !
  assert(flg);

  return EXIT_SUCCESS;
}
