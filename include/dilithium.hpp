#pragma once
#include "params.hpp"
#include "polyvec.hpp"
#include "sampling.hpp"
#include "utils.hpp"

// Dilithium Post-Quantum Digital Signature Algorithm
namespace dilithium {

// Given a 32 -bytes seed, this routine generates a public key and secret key
// pair, using deterministic key generation algorithm, as described in figure 4
// of Dilithium specification
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
static inline void
keygen(const uint8_t* const __restrict seed,
       uint8_t* const __restrict pubkey,
       uint8_t* const __restrict seckey)
  requires(dilithium_params::check_keygen_params(k, l, d, η))
{
  uint8_t seed_hash[32 + 64 + 32]{};

  shake256::shake256 hasher0{};
  hasher0.hash(seed, 32);
  hasher0.read(seed_hash, sizeof(seed_hash));

  const uint8_t* rho = seed_hash + 0;
  const uint8_t* rho_prime = rho + 32;
  const uint8_t* key = rho_prime + 64;

  field::zq_t A[k * l * ntt::N]{};

  sampling::expand_a<k, l>(rho, A);

  field::zq_t s1[l * ntt::N]{};
  field::zq_t s2[k * ntt::N]{};

  sampling::expand_s<η, l, 0>(rho_prime, s1);
  sampling::expand_s<η, k, l>(rho_prime, s2);

  field::zq_t s1_prime[l * ntt::N]{};

  std::memcpy(s1_prime, s1, sizeof(s1));
  polyvec::ntt<l>(s1_prime);

  field::zq_t t[k * ntt::N]{};

  polyvec::matrix_multiply<k, l, l, 1>(A, s1_prime, t);
  polyvec::intt<k>(t);
  polyvec::add_to<k>(s2, t);

  field::zq_t t1[k * ntt::N]{};
  field::zq_t t0[k * ntt::N]{};

  polyvec::power2round<k, d>(t, t1, t0);

  constexpr size_t t1_bw = std::bit_width(field::Q) - d;
  uint8_t crh_in[32 + k * 32 * t1_bw]{};
  uint8_t tr[32]{};

  std::memcpy(crh_in, rho, 32);
  polyvec::encode<k, t1_bw>(t1, crh_in + 32);

  shake256::shake256 hasher1{};
  hasher1.hash(crh_in, sizeof(crh_in));
  hasher1.read(tr, sizeof(tr));

  std::memcpy(pubkey, crh_in, sizeof(crh_in));
  std::memcpy(seckey, rho, 32);
  std::memcpy(seckey + 32, key, 32);
  std::memcpy(seckey + 64, tr, 32);

  polyvec::sub_from_x<l, η>(s1);
  polyvec::sub_from_x<k, η>(s2);

  constexpr size_t eta_bw = std::bit_width(2 * η);
  constexpr size_t s1_len = l * eta_bw * 32;
  constexpr size_t s2_len = k * eta_bw * 32;

  polyvec::encode<l, eta_bw>(s1, seckey + 96);
  polyvec::encode<k, eta_bw>(s2, seckey + 96 + s1_len);

  constexpr uint32_t t0_rng = 1u << (d - 1);

  polyvec::sub_from_x<k, t0_rng>(t0);
  polyvec::encode<k, d>(t0, seckey + 96 + s1_len + s2_len);
}

// Given a Dilithium secret key and non-empty message, this routine uses
// Dilithium signing algorithm for computing deterministic ( default choice ) or
// randomized signature for input messsage M, using provided parameters.
//
// If you're interested in generating randomized signature, you should pass
// truth value for last template parameter ( find `randomized` ). By default,
// this implementation generates deterministic signature i.e. for same message
// M, it'll generate same signature everytime. Note, when randomized signing is
// enabled ( compile-time choice ), uniform random 64 -bytes seed must be passed
// using last function parameter ( see `seed` ), which can be left empty ( say
// set to nullptr ) in case you're not adopting to use randomized signing.
//
// Signing algorithm is described in figure 4 of Dilithium specification
// https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf
//
// For Dilithium parameters, see table 2 of specification.
//
// Generated signature is of (32 + (32 * l * gamma1_bw) + (ω + k)) -bytes
//
// s.t. gamma1_bw = floor(log2(γ1)) + 1
//
// See section 5.4 of specification for understanding how signature is byte
// serialized.
template<const size_t k,
         const size_t l,
         const size_t d,
         const uint32_t η,
         const uint32_t γ1,
         const uint32_t γ2,
         const uint32_t τ,
         const uint32_t β,
         const size_t ω,
         const bool randomized = false>
static inline void
sign(
  const uint8_t* const __restrict seckey,
  const uint8_t* const __restrict msg,
  const size_t mlen,
  uint8_t* const __restrict sig,
  const uint8_t* const __restrict seed // 64 -bytes seed, for randomized signing
  )
  requires(dilithium_params::check_signing_params(k, l, d, η, γ1, γ2, τ, β, ω))
{
  constexpr uint32_t t0_rng = 1u << (d - 1);

  constexpr size_t eta_bw = std::bit_width(2 * η);
  constexpr size_t s1_len = l * eta_bw * 32;
  constexpr size_t s2_len = k * eta_bw * 32;

  constexpr size_t secoff0 = 0;
  constexpr size_t secoff1 = secoff0 + 32;
  constexpr size_t secoff2 = secoff1 + 32;
  constexpr size_t secoff3 = secoff2 + 32;
  constexpr size_t secoff4 = secoff3 + s1_len;
  constexpr size_t secoff5 = secoff4 + s2_len;

  const uint8_t* const rho = seckey + secoff0;
  const uint8_t* const key = seckey + secoff1;
  const uint8_t* const tr = seckey + secoff2;

  field::zq_t A[k * l * ntt::N]{};

  sampling::expand_a<k, l>(rho, A);

  uint8_t mu[64]{};

  shake256::shake256<true> hasher0{};
  hasher0.absorb(tr, 32);
  hasher0.absorb(msg, mlen);
  hasher0.finalize();
  hasher0.read(mu, sizeof(mu));

  uint8_t rho_prime[64]{};

  if constexpr (randomized) {
    std::memcpy(rho_prime, seed, sizeof(rho_prime));
  } else {
    uint8_t crh_in[32 + 64]{};

    std::memcpy(crh_in + 0, key, 32);
    std::memcpy(crh_in + 32, mu, sizeof(mu));

    shake256::shake256 hasher1{};
    hasher1.hash(crh_in, sizeof(crh_in));
    hasher1.read(rho_prime, sizeof(rho_prime));
  }

  field::zq_t s1[l * ntt::N]{};
  field::zq_t s2[k * ntt::N]{};
  field::zq_t t0[k * ntt::N]{};

  polyvec::decode<l, eta_bw>(seckey + secoff3, s1);
  polyvec::decode<k, eta_bw>(seckey + secoff4, s2);
  polyvec::decode<k, d>(seckey + secoff5, t0);

  polyvec::sub_from_x<l, η>(s1);
  polyvec::sub_from_x<k, η>(s2);
  polyvec::sub_from_x<k, t0_rng>(t0);

  polyvec::ntt<l>(s1);
  polyvec::ntt<k>(s2);
  polyvec::ntt<k>(t0);

  bool has_signed = false;
  uint16_t kappa = 0;

  field::zq_t z[l * ntt::N]{};
  field::zq_t h[k * ntt::N]{};
  uint8_t hash_out[32]{};

  while (!has_signed) {
    field::zq_t y[l * ntt::N]{};
    field::zq_t y_prime[l * ntt::N]{};
    field::zq_t w[k * ntt::N]{};

    sampling::expand_mask<γ1, l>(rho_prime, kappa, y);

    std::memcpy(y_prime, y, sizeof(y));

    polyvec::ntt<l>(y_prime);
    polyvec::matrix_multiply<k, l, l, 1>(A, y_prime, w);
    polyvec::intt<k>(w);

    constexpr uint32_t α = γ2 << 1;
    constexpr uint32_t m = (field::Q - 1u) / α;
    constexpr size_t w1bw = std::bit_width(m - 1u);

    field::zq_t w1[k * ntt::N]{};
    uint8_t hash_in[64 + (k * w1bw * 32)]{};
    field::zq_t c[ntt::N]{};

    polyvec::highbits<k, α>(w, w1);

    std::memcpy(hash_in, mu, 64);
    polyvec::encode<k, w1bw>(w1, hash_in + 64);

    shake256::shake256 hasher2{};
    hasher2.hash(hash_in, sizeof(hash_in));
    hasher2.read(hash_out, sizeof(hash_out));

    sampling::sample_in_ball<τ>(hash_out, c);
    ntt::ntt(c);

    polyvec::mul_by_poly<l>(c, s1, z);
    polyvec::intt<l>(z);
    polyvec::add_to<l>(y, z);

    field::zq_t r0[k * ntt::N]{};
    field::zq_t r1[k * ntt::N]{};

    polyvec::mul_by_poly<k>(c, s2, r1);
    polyvec::intt<k>(r1);
    polyvec::neg<k>(r1);
    polyvec::add_to<k>(w, r1);
    polyvec::lowbits<k, α>(r1, r0);

    const field::zq_t z_norm = polyvec::infinity_norm<l>(z);
    const field::zq_t r0_norm = polyvec::infinity_norm<k>(r0);

    constexpr field::zq_t bound0{ γ1 - β };
    constexpr field::zq_t bound1{ γ2 - β };

    const bool flg0 = z_norm >= bound0;
    const bool flg1 = r0_norm >= bound1;
    const bool flg2 = flg0 | flg1;

    has_signed = !flg2;

    field::zq_t h0[k * ntt::N]{};
    field::zq_t h1[k * ntt::N]{};

    polyvec::mul_by_poly<k>(c, t0, h0);
    polyvec::intt<k>(h0);
    std::memcpy(h1, h0, sizeof(h0));
    polyvec::neg<k>(h0);
    polyvec::add_to<k>(h1, r1);
    polyvec::make_hint<k, α>(h0, r1, h);

    const field::zq_t ct0_norm = polyvec::infinity_norm<k>(h1);
    const size_t count_1 = polyvec::count_1s<k>(h);

    constexpr field::zq_t bound2{ γ2 };

    const bool flg3 = ct0_norm >= bound2;
    const bool flg4 = count_1 > ω;
    const bool flg5 = flg3 | flg4;

    has_signed = has_signed & !flg5;
    kappa += static_cast<uint16_t>(l);
  }

  constexpr size_t gamma1_bw = std::bit_width(γ1);
  constexpr size_t sigoff0 = 0;
  constexpr size_t sigoff1 = sigoff0 + 32;
  constexpr size_t sigoff2 = sigoff1 + (32 * l * gamma1_bw);

  std::memcpy(sig + sigoff0, hash_out, sizeof(hash_out));
  polyvec::sub_from_x<l, γ1>(z);
  polyvec::encode<l, gamma1_bw>(z, sig + sigoff1);
  bit_packing::encode_hint_bits<k, ω>(h, sig + sigoff2);
}

// Given a Dilithium public key, message bytes and serialized signature, this
// routine verifies the correctness of signature, returning boolean result,
// denoting status of signature verification. For example, say it returns true,
// it means signature has successfully been verified.
//
// Verification algorithm is described in figure 4 of Dilithium specification
// https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf
template<const size_t k,
         const size_t l,
         const size_t d,
         const uint32_t γ1,
         const uint32_t γ2,
         const uint32_t τ,
         const uint32_t β,
         const size_t ω>
static inline bool
verify(const uint8_t* const __restrict pubkey,
       const uint8_t* const __restrict msg,
       const size_t mlen,
       const uint8_t* const __restrict sig)
  requires(dilithium_params::check_verify_params(k, l, d, γ1, γ2, τ, β, ω))
{
  constexpr size_t t1_bw = std::bit_width(field::Q) - d;
  constexpr size_t pklen = dilithium_utils::pub_key_len<k, d>();

  constexpr size_t puboff0 = 0;
  constexpr size_t puboff1 = puboff0 + 32;

  constexpr size_t gamma1_bw = std::bit_width(γ1);
  constexpr size_t sigoff0 = 0;
  constexpr size_t sigoff1 = sigoff0 + 32;
  constexpr size_t sigoff2 = sigoff1 + (32 * l * gamma1_bw);

  field::zq_t A[k * l * ntt::N]{};
  field::zq_t t1[k * ntt::N]{};

  sampling::expand_a<k, l>(pubkey + puboff0, A);
  polyvec::decode<k, t1_bw>(pubkey + puboff1, t1);

  uint8_t crh_in[32]{};
  uint8_t mu[64]{};

  shake256::shake256 hasher0{};
  hasher0.hash(pubkey, pklen);
  hasher0.read(crh_in, sizeof(crh_in));

  shake256::shake256<true> hasher1{};
  hasher1.absorb(crh_in, sizeof(crh_in));
  hasher1.absorb(msg, mlen);
  hasher1.finalize();
  hasher1.read(mu, sizeof(mu));

  field::zq_t c[ntt::N]{};

  sampling::sample_in_ball<τ>(sig + sigoff0, c);
  ntt::ntt(c);

  field::zq_t z[l * ntt::N]{};
  field::zq_t h[k * ntt::N]{};

  polyvec::decode<l, gamma1_bw>(sig + sigoff1, z);
  polyvec::sub_from_x<l, γ1>(z);
  const bool failed = bit_packing::decode_hint_bits<k, ω>(sig + sigoff2, h);

  field::zq_t w0[k * ntt::N]{};
  field::zq_t w1[k * ntt::N]{};
  field::zq_t w2[k * ntt::N]{};

  const field::zq_t z_norm = polyvec::infinity_norm<l>(z);
  const size_t count_1 = polyvec::count_1s<k>(h);

  polyvec::ntt<l>(z);
  polyvec::matrix_multiply<k, l, l, 1>(A, z, w0);

  polyvec::shl<k, d>(t1);
  polyvec::ntt<k>(t1);
  polyvec::mul_by_poly<k>(c, t1, w2);
  polyvec::neg<k>(w2);

  polyvec::add_to<k>(w0, w2);
  polyvec::intt<k>(w2);

  constexpr uint32_t α = γ2 << 1;
  constexpr uint32_t m = (field::Q - 1u) / α;
  constexpr size_t w1bw = std::bit_width(m - 1u);

  polyvec::use_hint<k, α>(h, w2, w1);

  uint8_t hash_in[64 + (k * w1bw * 32)]{};
  uint8_t hash_out[32]{};

  std::memcpy(hash_in, mu, sizeof(mu));
  polyvec::encode<k, w1bw>(w1, hash_in + 64);

  shake256::shake256 hasher2{};
  hasher2.hash(hash_in, sizeof(hash_in));
  hasher2.read(hash_out, sizeof(hash_out));

  constexpr field::zq_t bound0{ γ1 - β };

  const bool flg0 = z_norm < bound0;
  bool flg1 = false;
  const bool flg2 = count_1 <= ω;

  for (size_t i = 0; i < 32; i++) {
    flg1 |= static_cast<bool>(sig[sigoff0 + i] ^ hash_out[i]);
  }

  const bool flg3 = flg0 & !flg1 & flg2;
  const bool flg4 = !failed & flg3;
  return flg4;
}

}
