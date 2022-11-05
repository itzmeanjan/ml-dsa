#include "dilithium.hpp"
#include <cassert>
#include <iostream>

// Compile it with
//
// g++ -std=c++20 -O3 -I ./include -I ./sha3/include example/dilithium2.cpp
int
main()
{
  // Dilithium2 Parameters
  //
  // See table 2 of Dilithium specification
  // https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf
  // for all parameters
  constexpr size_t k = 4;
  constexpr size_t l = 4;
  constexpr uint32_t η = 2;
  constexpr size_t d = 13;
  constexpr uint32_t γ1 = 1u << 17;
  constexpr uint32_t γ2 = (ff::Q - 1) / 88;
  constexpr uint32_t τ = 39;
  constexpr uint32_t β = τ * η;
  constexpr size_t ω = 80;

  constexpr size_t slen = 32; // seed length
  constexpr size_t mlen = 32; // message length ( can be anything >= 1 )

  // compile-time compute public key, secret key and signature byte length
  constexpr size_t pklen = dilithium_utils::pubkey_length<k, d>();
  constexpr size_t sklen = dilithium_utils::seckey_length<k, l, η, d>();
  constexpr size_t siglen = dilithium_utils::signature_length<k, l, γ1, ω>();

  // allocate memory resources
  uint8_t* seed = static_cast<uint8_t*>(std::malloc(slen));
  uint8_t* msg = static_cast<uint8_t*>(std::malloc(mlen));
  uint8_t* pubkey = static_cast<uint8_t*>(std::malloc(pklen));
  uint8_t* seckey = static_cast<uint8_t*>(std::malloc(sklen));
  uint8_t* sig = static_cast<uint8_t*>(std::malloc(siglen));

  // generate random 32 -bytes seed & N -bytes message ( to be signed )
  dilithium_utils::random_data<uint8_t>(seed, slen);
  dilithium_utils::random_data<uint8_t>(msg, mlen);

  bool flg = false;

  // Key generation -> signing -> verification
  dilithium::keygen<k, l, d, η>(seed, pubkey, seckey);
  dilithium::sign<k, l, d, η, γ1, γ2, τ, β, ω>(seckey, msg, mlen, sig);

  // Default option is deterministic signing, for randomized signing, use
  // dilithium::sign<k, l, d, η, γ1, γ2, τ, β, ω, true>(seckey, msg, mlen, sig);

  flg = dilithium::verify<k, l, d, γ1, γ2, τ, β, ω>(pubkey, msg, mlen, sig);

  {
    using namespace dilithium_utils;

    std::cout << "Dilithium @ NIST security level 2\n";
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
