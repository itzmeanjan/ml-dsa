#pragma once
#include "dilithium.hpp"

// Dilithium Post-Quantum Digital Signature Algorithm instantiated with NIST
// security level 5 parameters, as suggested in table 2 of
// https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf
namespace dilithium5 {

// Byte length of Dilithium5 public key = 2592B
constexpr size_t PubKeyLength = dilithium_utils::pubkey_length<8, 13>();

// Byte length of Dilithium5 secret key = 4864B
constexpr size_t SecKeyLength = dilithium_utils::seckey_length<8, 7, 2, 13>();

// Byte length of Dilithium5 signature = 4595B
constexpr size_t SigLength = dilithium_utils::sig_length<8, 7, 1u << 19, 75>();

// Given a 32 -bytes seed, this routine can be used for generating a fresh
// keypair.
inline void
keygen(const uint8_t* const __restrict seed, // 32 -bytes seed
       uint8_t* const __restrict pubkey,
       uint8_t* const __restrict seckey)
{
  constexpr size_t k = 8;
  constexpr size_t l = 7;
  constexpr uint32_t η = 2;
  constexpr size_t d = 13;

  dilithium::keygen<k, l, d, η>(seed, pubkey, seckey);
}

// Given a secret key and a message M, this routine can be used for signing the
// message, computing the signature either deterministically ( by default ) or
// non-deterministically - a compile-time decision using template parameters.
template<const bool random = false>
inline void
sign(const uint8_t* const __restrict seckey,
     const uint8_t* const __restrict msg,
     const size_t mlen,
     uint8_t* const __restrict sig)
{
  constexpr size_t k = 8;
  constexpr size_t l = 7;
  constexpr uint32_t η = 2;
  constexpr size_t d = 13;
  constexpr uint32_t γ1 = 1u << 19;
  constexpr uint32_t γ2 = (field::Q - 1) / 32;
  constexpr uint32_t τ = 60;
  constexpr uint32_t β = τ * η;
  constexpr size_t ω = 75;

  dilithium::sign<k, l, d, η, γ1, γ2, τ, β, ω, random>(seckey, msg, mlen, sig);
}

// Given a public key, a message M and a signature S, this routine can be used
// for verifying if the signature is valid for the provided message or not,
// returning truth value only in case of successful signature verification,
// otherwise false is returned.
inline bool
verify(const uint8_t* const __restrict pubkey,
       const uint8_t* const __restrict msg,
       const size_t mlen,
       const uint8_t* const __restrict sig)
{
  constexpr size_t k = 8;
  constexpr size_t l = 7;
  constexpr uint32_t η = 2;
  constexpr size_t d = 13;
  constexpr uint32_t γ1 = 1u << 19;
  constexpr uint32_t γ2 = (field::Q - 1) / 32;
  constexpr uint32_t τ = 60;
  constexpr uint32_t β = τ * η;
  constexpr size_t ω = 75;

  return dilithium::verify<k, l, d, γ1, γ2, τ, β, ω>(pubkey, msg, mlen, sig);
}

}
