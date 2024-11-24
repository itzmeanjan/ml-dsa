#pragma once
#include "ml_dsa/internals/ml_dsa.hpp"

namespace ml_dsa_44 {

// See table 1 of ML-DSA draft standard @ https://doi.org/10.6028/NIST.FIPS.204.ipd
static constexpr size_t d = 13;
static constexpr uint32_t τ = 39;
static constexpr uint32_t γ1 = 1u << 17;
static constexpr uint32_t γ2 = (ml_dsa_field::Q - 1) / 88;
static constexpr size_t k = 4;
static constexpr size_t l = 4;
static constexpr uint32_t η = 2;
static constexpr uint32_t β = τ * η;
static constexpr size_t ω = 80;
static constexpr size_t λ = 128;

// Byte length ( = 32 ) of ML-DSA-44 key generation seed.
static constexpr size_t KeygenSeedByteLen = ml_dsa::KEYGEN_SEED_BYTE_LEN;

// Byte length ( = 1312 ) of ML-DSA-44 public key.
static constexpr size_t PubKeyByteLen = ml_dsa_utils::pub_key_len(k, d);

// Byte length ( = 2560 ) of ML-DSA-44 secret key.
static constexpr size_t SecKeyByteLen = ml_dsa_utils::sec_key_len(k, l, η, d);

// Byte length ( = 32 ) of ML-DSA-44 signing seed.
static constexpr size_t SigningSeedByteLen = ml_dsa::RND_BYTE_LEN;

// Byte length ( = 2420 ) of ML-DSA-44 signature.
static constexpr size_t SigByteLen = ml_dsa_utils::sig_len(k, l, γ1, ω, λ);

// Given a 32 -bytes seed, this routine can be used for generating a fresh ML-DSA-44 keypair.
constexpr void
keygen(std::span<const uint8_t, KeygenSeedByteLen> ξ, std::span<uint8_t, PubKeyByteLen> pubkey, std::span<uint8_t, SecKeyByteLen> seckey)
{
  ml_dsa::keygen<k, l, d, η>(ξ, pubkey, seckey);
}

// Given a 32 -bytes seed `rnd` and ML-DSA-44 secret key, this routine can be used for signing any arbitrary (>=0)
// length message M, producing a ML-DSA-44 signature S.
//
// Default (and recommended) signing mode is "hedged" i.e. using 32B input randomness for signing, results into
// randomized signature. For "deterministic" signing mode, simply fill `rnd` with zero bytes.
constexpr bool
sign(std::span<const uint8_t, SigningSeedByteLen> rnd,
     std::span<const uint8_t, SecKeyByteLen> seckey,
     std::span<const uint8_t> msg,
     std::span<const uint8_t> ctx,
     std::span<uint8_t, SigByteLen> sig)
{
  return ml_dsa::sign<k, l, d, η, γ1, γ2, τ, β, ω, λ>(rnd, seckey, msg, ctx, sig);
}

// Given a ML-DSA-44 public key, a message M and a signature S, this routine can be used for verifying if the signature
// is valid for the provided message or not, returning truth value only in case of successful signature verification,
// otherwise false is returned.
constexpr bool
verify(std::span<const uint8_t, PubKeyByteLen> pubkey, std::span<const uint8_t> msg, std::span<const uint8_t> ctx, std::span<const uint8_t, SigByteLen> sig)
{
  return ml_dsa::verify<k, l, d, γ1, γ2, τ, β, ω, λ>(pubkey, msg, ctx, sig);
}

}
