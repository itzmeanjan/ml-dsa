#pragma once
#include "polyvec.hpp"
#include "sampling.hpp"

// Dilithium Post-Quantum Digital Signature Algorithm
namespace dilithium {

// Given 32 -bytes seed, this routine generates Dilithium public key and secret
// key pair, using deterministic key generation algorithm, as described in
// figure 4 of Dilithium specification
// https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf
//
// See table 2 of specification for allowed parameters.
//
// Generated public key is of (32 + k * 320) -bytes.
// Generated secret key is of (96 + 32 * (k * ebw + l * ebw + k * d)) -bytes
//
// Note, ebw = ceil(log2(2 * η + 1))
//
// See section 5.4 of specification for public key and secret key byte length.
template<const size_t k, const size_t l, const size_t d, const uint32_t η>
static void
keygen(
  const uint8_t* const __restrict seed, // 32 -bytes seed
  uint8_t* const __restrict pubkey,     // (32 + k * 320) -bytes
  uint8_t* const __restrict seckey // (96 + 32 * (ebw*(k + l) + k*d)) -bytes
  )
  requires(dilithium_utils::check_η(η) && dilithium_utils::check_d(d))
{
  uint8_t seed_hash[32 + 64 + 32]{};

  shake256::shake256 hasher0{};
  hasher0.hash(seed, 32);
  hasher0.read(seed_hash, sizeof(seed_hash));

  const uint8_t* rho = seed_hash + 0;
  const uint8_t* rho_prime = rho + 32;
  const uint8_t* key = rho_prime + 64;

  field::zq_t A[k * l * ntt::N]{};

  dilithium_utils::expand_a<k, l>(rho, A);

  field::zq_t s1[l * ntt::N]{};
  field::zq_t s2[k * ntt::N]{};

  dilithium_utils::expand_s<η, l, 0>(rho_prime, s1);
  dilithium_utils::expand_s<η, k, l>(rho_prime, s2);

  field::zq_t s1_prime[l * ntt::N]{};

  std::memcpy(s1_prime, s1, sizeof(s1));
  dilithium_utils::polyvec_ntt<l>(s1_prime);

  field::zq_t t[k * ntt::N]{};

  dilithium_utils::matrix_multiply<k, l, l, 1>(A, s1_prime, t);
  dilithium_utils::polyvec_intt<k>(t);
  dilithium_utils::polyvec_add_to<k>(s2, t);

  field::zq_t t1[k * ntt::N]{};
  field::zq_t t0[k * ntt::N]{};

  dilithium_utils::polyvec_power2round<k, d>(t, t1, t0);

  constexpr size_t t1_bw = std::bit_width(field::Q) - d;
  uint8_t crh_in[32 + k * 32 * t1_bw]{};
  uint8_t tr[32]{};

  std::memcpy(crh_in, rho, 32);
  dilithium_utils::polyvec_encode<k, t1_bw>(t1, crh_in + 32);

  shake256::shake256 hasher1{};
  hasher1.hash(crh_in, sizeof(crh_in));
  hasher1.read(tr, sizeof(tr));

  std::memcpy(pubkey, crh_in, sizeof(crh_in));
  std::memcpy(seckey, rho, 32);
  std::memcpy(seckey + 32, key, 32);
  std::memcpy(seckey + 64, tr, 32);

  dilithium_utils::polyvec_sub_from_x<l, η>(s1);
  dilithium_utils::polyvec_sub_from_x<k, η>(s2);

  constexpr size_t eta_bw = std::bit_width(2 * η);
  constexpr size_t s1_len = l * eta_bw * 32;
  constexpr size_t s2_len = k * eta_bw * 32;

  dilithium_utils::polyvec_encode<l, eta_bw>(s1, seckey + 96);
  dilithium_utils::polyvec_encode<k, eta_bw>(s2, seckey + 96 + s1_len);

  constexpr uint32_t t0_rng = 1u << (d - 1);

  dilithium_utils::polyvec_sub_from_x<k, t0_rng>(t0);
  dilithium_utils::polyvec_encode<k, d>(t0, seckey + 96 + s1_len + s2_len);
}

}
