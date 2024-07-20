#pragma once
#include "ml_dsa/internals/ml_dsa.hpp"

// Dilithium Post-Quantum Digital Signature Algorithm instantiated with NIST
// security level 2 parameters, as suggested in table 2 of
// https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf
namespace dilithium2 {

// See NIST security level 2 parameter set, in table 2 of the specification.
constexpr size_t d = 13;
constexpr uint32_t τ = 39;
constexpr uint32_t γ1 = 1u << 17;
constexpr uint32_t γ2 = (ml_dsa_field::Q - 1) / 88;
constexpr size_t k = 4;
constexpr size_t l = 4;
constexpr uint32_t η = 2;
constexpr uint32_t β = τ * η;
constexpr size_t ω = 80;
constexpr size_t λ = 128;

// = 1312 -bytes Dilithium2 public key
constexpr size_t PubKeyLen = ml_dsa_utils::pub_key_len(k, d);

// = 2560 -bytes Dilithium2 secret key
constexpr size_t SecKeyLen = ml_dsa_utils::sec_key_len(k, l, η, d);

// = 2420 -bytes Dilithium2 signature
constexpr size_t SigLen = ml_dsa_utils::sig_len(k, l, γ1, ω, λ);

// Given a 32 -bytes seed, this routine can be used for generating a fresh
// Dilithium2 keypair.
inline void
keygen(std::span<const uint8_t, 32> seed, std::span<uint8_t, PubKeyLen> pubkey, std::span<uint8_t, SecKeyLen> seckey)
{
  ml_dsa::keygen<k, l, d, η>(seed, pubkey, seckey);
}

// Given a Dilithium2 secret key and a non-empty message M, this routine can be
// used for signing the message, computing the signature either
// deterministically ( by default ) or non-deterministically - a compile-time
// decision using template parameter. In case you opt for using randomized
// signing, ensure that you pass 64 -bytes uniform random sampled seed, using
// last parameter of the function. If you choose (default) deterministic
// signing, you can safely pass `nullptr` for last parameter of this function -
// as that seed will never be accessed during signing.
inline void
sign(std::span<const uint8_t, 32> rnd,
     std::span<const uint8_t, SecKeyLen> seckey,
     std::span<const uint8_t> msg,
     std::span<uint8_t, SigLen> sig)
{
  ml_dsa::sign<k, l, d, η, γ1, γ2, τ, β, ω, λ>(rnd, seckey, msg, sig);
}

// Given a Dilithium2 public key, a message M and a signature S, this routine
// can be used for verifying if the signature is valid for the provided message
// or not, returning truth value only in case of successful signature
// verification, otherwise false is returned.
inline bool
verify(std::span<const uint8_t, PubKeyLen> pubkey, std::span<const uint8_t> msg, std::span<const uint8_t, SigLen> sig)
{
  return ml_dsa::verify<k, l, d, γ1, γ2, τ, β, ω, λ>(pubkey, msg, sig);
}

}
