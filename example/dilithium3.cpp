#include "dilithium3.hpp"
#include "prng.hpp"
#include <cassert>
#include <iostream>

// Compile it with
//
// g++ -std=c++20 -O3 -march=native -I ./include -I ./sha3/include
// example/dilithium3.cpp
int
main()
{
  constexpr size_t slen = 32; // seed length
  constexpr size_t mlen = 32; // message length ( can be anything >= 1 )

  constexpr size_t pklen = dilithium3::PubKeyLen;
  constexpr size_t sklen = dilithium3::SecKeyLen;
  constexpr size_t siglen = dilithium3::SigLen;

  // allocate memory resources
  uint8_t* seed = static_cast<uint8_t*>(std::malloc(slen));
  uint8_t* msg = static_cast<uint8_t*>(std::malloc(mlen));
  uint8_t* pubkey = static_cast<uint8_t*>(std::malloc(pklen));
  uint8_t* seckey = static_cast<uint8_t*>(std::malloc(sklen));
  uint8_t* sig = static_cast<uint8_t*>(std::malloc(siglen));

  // generate random 32 -bytes seed & N -bytes message ( to be signed )
  prng::prng_t prng;

  prng.read(seed, slen);
  prng.read(msg, mlen);

  bool flg = false;

  // Key generation -> signing -> verification
  dilithium3::keygen(seed, pubkey, seckey);
  dilithium3::sign(seckey, msg, mlen, sig, nullptr);

  // Default option is deterministic signing, for randomized signing, use
  // dilithium3::sign<true>(seckey, msg, mlen, sig, seed);

  flg = dilithium3::verify(pubkey, msg, mlen, sig);

  {
    using namespace dilithium_utils;

    std::cout << "Dilithium @ NIST security level 3\n";
    std::cout << "seed      : " << to_hex(seed, slen) << "\n";
    std::cout << "pubkey    : " << to_hex(pubkey, pklen) << "\n";
    std::cout << "seckey    : " << to_hex(seckey, sklen) << "\n";
    std::cout << "message   : " << to_hex(msg, mlen) << "\n";
    std::cout << "signature : " << to_hex(sig, siglen) << "\n";
    std::cout << "verified   : " << std::boolalpha << flg << "\n";
  }

  // deallocate memory resources
  std::free(seed);
  std::free(msg);
  std::free(pubkey);
  std::free(seckey);
  std::free(sig);

  // check that signature verification passed !
  assert(flg);

  return EXIT_SUCCESS;
}
