#pragma once
#include "ml_dsa/internals/ml_dsa.hpp"

namespace ml_dsa_87 {

// See table 1 of ML-DSA standard @ https://doi.org/10.6028/NIST.FIPS.204
static constexpr size_t d = 13;
static constexpr uint32_t tau = 60;
static constexpr uint32_t gamma1 = 1u << 19;
static constexpr uint32_t gamma2 = (ml_dsa_field::Q - 1) / 32;
static constexpr size_t k = 8;
static constexpr size_t l = 7;
static constexpr uint32_t eta = 2;
static constexpr uint32_t beta = tau * eta;
static constexpr size_t omega = 75;
static constexpr size_t lambda = 256;

// Byte length ( = 32 ) of ML-DSA-87 key generation seed.
static constexpr size_t KeygenSeedByteLen = ml_dsa::KEYGEN_SEED_BYTE_LEN;

// Byte length ( = 2592 ) of ML-DSA-87 public key.
static constexpr size_t PubKeyByteLen = ml_dsa_utils::pub_key_len(k, d);

// Byte length ( = 4896 ) of ML-DSA-87 secret key.
static constexpr size_t SecKeyByteLen = ml_dsa_utils::sec_key_len(k, l, eta, d);

// Byte length ( = 32 ) of ML-DSA-87 signing seed.
static constexpr size_t SigningSeedByteLen = ml_dsa::RND_BYTE_LEN;

// Byte length ( = 4627 ) of ML-DSA-87 signature.
static constexpr size_t SigByteLen = ml_dsa_utils::sig_len(k, l, gamma1, omega, lambda);

// Given a 32 -bytes seed, this routine can be used for generating a fresh ML-DSA-87 keypair, in deterministic fashion.
constexpr void
keygen(std::span<const uint8_t, KeygenSeedByteLen> ξ, std::span<uint8_t, PubKeyByteLen> pubkey, std::span<uint8_t, SecKeyByteLen> seckey)
{
  ml_dsa::keygen<k, l, d, eta>(ξ, pubkey, seckey);
}

// Given a 32 -bytes seed `rnd` and ML-DSA-87 secret key, this routine can be used for signing any arbitrary (>=0)
// length message M, while also including an optional context (whose length must be capped at 255 -bytes),
// producing a ML-DSA-87 signature S.
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
  return ml_dsa::sign<k, l, d, eta, gamma1, gamma2, tau, beta, omega, lambda>(rnd, seckey, msg, ctx, sig);
}

// Given a ML-DSA-87 public key, a message M, an optional context C (of length at max 255 -bytes) and a signature S,
// this routine can be used for verifying if the signature is valid for the provided message or not, returning truth
// value only in case of successful signature verification, otherwise false is returned.
constexpr bool
verify(std::span<const uint8_t, PubKeyByteLen> pubkey, std::span<const uint8_t> msg, std::span<const uint8_t> ctx, std::span<const uint8_t, SigByteLen> sig)
{
  return ml_dsa::verify<k, l, d, gamma1, gamma2, tau, beta, omega, lambda>(pubkey, msg, ctx, sig);
}

}
