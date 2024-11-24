#pragma once
#include "ml_dsa/internals/math/field.hpp"
#include "ml_dsa/internals/poly/polyvec.hpp"
#include "ml_dsa/internals/poly/sampling.hpp"
#include "ml_dsa/internals/utility/params.hpp"
#include "ml_dsa/internals/utility/utils.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <limits>

// ML-DSA FIPS 204
namespace ml_dsa {

// Byte length of seed ξ, required for key generation.
static constexpr size_t KEYGEN_SEED_BYTE_LEN = 32;

// Byte length of randomness, required for hedged signing.
static constexpr size_t RND_BYTE_LEN = 32;

// Given seed ξ, this routine generates a public key and secret key pair, using deterministic key generation algorithm.
//
// See algorithm 1 of ML-DSA draft standard @ https://doi.org/10.6028/NIST.FIPS.204.ipd.
template<size_t k, size_t l, size_t d, uint32_t η>
static inline constexpr void
keygen(std::span<const uint8_t, KEYGEN_SEED_BYTE_LEN> ξ,
       std::span<uint8_t, ml_dsa_utils::pub_key_len(k, d)> pubkey,
       std::span<uint8_t, ml_dsa_utils::sec_key_len(k, l, η, d)> seckey)
  requires(ml_dsa_params::check_keygen_params(k, l, d, η))
{
  constexpr std::array<uint8_t, 2> domain_separator{ k, l };

  std::array<uint8_t, 32 + 64 + 32> seed_hash{};
  auto seed_hash_span = std::span(seed_hash);

  shake256::shake256_t hasher;
  hasher.absorb(ξ);
  hasher.absorb(domain_separator);
  hasher.finalize();
  hasher.squeeze(seed_hash_span);

  auto rho = seed_hash_span.template first<32>();
  auto rho_prime = seed_hash_span.template subspan<rho.size(), 64>();
  auto key = seed_hash_span.template last<32>();

  std::array<ml_dsa_field::zq_t, k * l * ml_dsa_ntt::N> A{};
  ml_dsa_sampling::expand_a<k, l>(rho, A);

  std::array<ml_dsa_field::zq_t, l * ml_dsa_ntt::N> s1{};
  std::array<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> s2{};

  ml_dsa_sampling::expand_s<η, l, 0>(rho_prime, s1);
  ml_dsa_sampling::expand_s<η, k, l>(rho_prime, s2);

  std::array<ml_dsa_field::zq_t, l * ml_dsa_ntt::N> s1_prime{};

  std::copy(s1.begin(), s1.end(), s1_prime.begin());
  ml_dsa_polyvec::ntt<l>(s1_prime);

  std::array<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> t{};

  ml_dsa_polyvec::matrix_multiply<k, l, l, 1>(A, s1_prime, t);
  ml_dsa_polyvec::intt<k>(t);
  ml_dsa_polyvec::add_to<k>(s2, t);

  std::array<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> t1{};
  std::array<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> t0{};

  ml_dsa_polyvec::power2round<k, d>(t, t1, t0);

  constexpr size_t t1_bw = std::bit_width(ml_dsa_field::Q) - d;
  std::array<uint8_t, 64> tr{};

  // Prepare public key
  constexpr size_t pkoff0 = 0;
  constexpr size_t pkoff1 = pkoff0 + rho.size();
  constexpr size_t pkoff2 = pubkey.size();

  std::copy(rho.begin(), rho.end(), pubkey.begin());
  ml_dsa_polyvec::encode<k, t1_bw>(t1, pubkey.template last<pkoff2 - pkoff1>());

  // Prepare secret key
  hasher.reset();
  hasher.absorb(pubkey);
  hasher.finalize();
  hasher.squeeze(tr);

  constexpr size_t eta_bw = std::bit_width(2 * η);
  constexpr size_t s1_len = l * eta_bw * 32;
  constexpr size_t s2_len = k * eta_bw * 32;

  constexpr size_t skoff0 = 0;
  constexpr size_t skoff1 = skoff0 + rho.size();
  constexpr size_t skoff2 = skoff1 + key.size();
  constexpr size_t skoff3 = skoff2 + tr.size();
  constexpr size_t skoff4 = skoff3 + s1_len;
  constexpr size_t skoff5 = skoff4 + s2_len;
  constexpr size_t skoff6 = seckey.size();

  std::copy(rho.begin(), rho.end(), seckey.template subspan<skoff0, skoff1 - skoff0>().begin());
  std::copy(key.begin(), key.end(), seckey.template subspan<skoff1, skoff2 - skoff1>().begin());
  std::copy(tr.begin(), tr.end(), seckey.template subspan<skoff2, skoff3 - skoff2>().begin());

  ml_dsa_polyvec::sub_from_x<l, η>(s1);
  ml_dsa_polyvec::sub_from_x<k, η>(s2);

  ml_dsa_polyvec::encode<l, eta_bw>(s1, seckey.template subspan<skoff3, skoff4 - skoff3>());
  ml_dsa_polyvec::encode<k, eta_bw>(s2, seckey.template subspan<skoff4, skoff5 - skoff4>());

  constexpr uint32_t t0_rng = 1u << (d - 1);

  ml_dsa_polyvec::sub_from_x<k, t0_rng>(t0);
  ml_dsa_polyvec::encode<k, d>(t0, seckey.template subspan<skoff5, skoff6 - skoff5>());
}

// Given a ML-DSA secret key and message (can be empty too), this routine computes a hedged/ deterministic signature.
//
// Notice, first parameter of this function, `rnd`, which lets you pass 32 -bytes randomness for generating default
// "hedged" signature. In case you don't need randomized message signature, you can instead fill `rnd` with zeros, and
// it'll generate a deterministic signature.
//
// Note, hedged signing is the default and recommended version.
//
// See algorithm 2 of ML-DSA draft standard @ https://doi.org/10.6028/NIST.FIPS.204.ipd.
template<size_t k, size_t l, size_t d, uint32_t η, uint32_t γ1, uint32_t γ2, uint32_t τ, uint32_t β, size_t ω, size_t λ>
static inline constexpr bool
sign(std::span<const uint8_t, RND_BYTE_LEN> rnd,
     std::span<const uint8_t, ml_dsa_utils::sec_key_len(k, l, η, d)> seckey,
     std::span<const uint8_t> msg,
     std::span<const uint8_t> ctx,
     std::span<uint8_t, ml_dsa_utils::sig_len(k, l, γ1, ω, λ)> sig)
  requires(ml_dsa_params::check_signing_params(k, l, d, η, γ1, γ2, τ, β, ω, λ))
{
  if (ctx.size() > std::numeric_limits<uint8_t>::max()) {
    return false;
  }
  constexpr uint32_t t0_rng = 1u << (d - 1);

  constexpr size_t eta_bw = std::bit_width(2 * η);
  constexpr size_t s1_len = l * eta_bw * 32;
  constexpr size_t s2_len = k * eta_bw * 32;

  constexpr size_t skoff0 = 0;
  constexpr size_t skoff1 = skoff0 + 32;
  constexpr size_t skoff2 = skoff1 + 32;
  constexpr size_t skoff3 = skoff2 + 64;
  constexpr size_t skoff4 = skoff3 + s1_len;
  constexpr size_t skoff5 = skoff4 + s2_len;

  auto rho = seckey.template subspan<skoff0, skoff1 - skoff0>();
  auto key = seckey.template subspan<skoff1, skoff2 - skoff1>();
  auto tr = seckey.template subspan<skoff2, skoff3 - skoff2>();

  std::array<ml_dsa_field::zq_t, k * l * ml_dsa_ntt::N> A{};
  ml_dsa_sampling::expand_a<k, l>(rho, A);

  std::array<uint8_t, 64> mu{};
  auto mu_span = std::span(mu);

  const std::array<uint8_t, 2> domain_separator{ 0, static_cast<uint8_t>(ctx.size()) };

  shake256::shake256_t hasher;
  hasher.absorb(tr);
  hasher.absorb(domain_separator);
  hasher.absorb(ctx);
  hasher.absorb(msg);
  hasher.finalize();
  hasher.squeeze(mu_span);

  std::array<uint8_t, 64> rho_prime{};

  hasher.reset();
  hasher.absorb(key);
  hasher.absorb(rnd);
  hasher.absorb(mu_span);
  hasher.finalize();
  hasher.squeeze(rho_prime);

  std::array<ml_dsa_field::zq_t, l * ml_dsa_ntt::N> s1{};
  std::array<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> s2{};
  std::array<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> t0{};

  ml_dsa_polyvec::decode<l, eta_bw>(seckey.template subspan<skoff3, skoff4 - skoff3>(), s1);
  ml_dsa_polyvec::decode<k, eta_bw>(seckey.template subspan<skoff4, skoff5 - skoff4>(), s2);
  ml_dsa_polyvec::decode<k, d>(seckey.template subspan<skoff5, seckey.size() - skoff5>(), t0);

  ml_dsa_polyvec::sub_from_x<l, η>(s1);
  ml_dsa_polyvec::sub_from_x<k, η>(s2);
  ml_dsa_polyvec::sub_from_x<k, t0_rng>(t0);

  ml_dsa_polyvec::ntt<l>(s1);
  ml_dsa_polyvec::ntt<k>(s2);
  ml_dsa_polyvec::ntt<k>(t0);

  bool has_signed = false;
  uint16_t kappa = 0;

  std::array<ml_dsa_field::zq_t, l * ml_dsa_ntt::N> z{};
  std::array<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> h{};

  std::array<uint8_t, (2 * λ) / std::numeric_limits<uint8_t>::digits> c_tilda{};
  auto c_tilda_span = std::span(c_tilda);

  while (!has_signed) {
    std::array<ml_dsa_field::zq_t, l * ml_dsa_ntt::N> y{};
    std::array<ml_dsa_field::zq_t, l * ml_dsa_ntt::N> y_prime{};
    std::array<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> w{};

    ml_dsa_sampling::expand_mask<γ1, l>(rho_prime, kappa, y);

    std::copy(y.begin(), y.end(), y_prime.begin());

    ml_dsa_polyvec::ntt<l>(y_prime);
    ml_dsa_polyvec::matrix_multiply<k, l, l, 1>(A, y_prime, w);
    ml_dsa_polyvec::intt<k>(w);

    constexpr uint32_t α = γ2 << 1;
    constexpr uint32_t m = (ml_dsa_field::Q - 1u) / α;
    constexpr size_t w1bw = std::bit_width(m - 1u);

    std::array<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> w1{};
    std::array<uint8_t, k * w1bw * 32> w1_encoded{};

    ml_dsa_polyvec::highbits<k, α>(w, w1);
    ml_dsa_polyvec::encode<k, w1bw>(w1, w1_encoded);

    hasher.reset();
    hasher.absorb(mu_span);
    hasher.absorb(w1_encoded);
    hasher.finalize();
    hasher.squeeze(c_tilda_span);

    std::array<ml_dsa_field::zq_t, ml_dsa_ntt::N> c{};

    ml_dsa_sampling::sample_in_ball<τ, λ>(c_tilda_span, c);
    ml_dsa_ntt::ntt(c);

    ml_dsa_polyvec::mul_by_poly<l>(c, s1, z);
    ml_dsa_polyvec::intt<l>(z);
    ml_dsa_polyvec::add_to<l>(y, z);

    std::array<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> r0{};
    std::array<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> r1{};

    ml_dsa_polyvec::mul_by_poly<k>(c, s2, r1);
    ml_dsa_polyvec::intt<k>(r1);
    ml_dsa_polyvec::neg<k>(r1);
    ml_dsa_polyvec::add_to<k>(w, r1);
    ml_dsa_polyvec::lowbits<k, α>(r1, r0);

    const ml_dsa_field::zq_t z_norm = ml_dsa_polyvec::infinity_norm<l>(z);
    const ml_dsa_field::zq_t r0_norm = ml_dsa_polyvec::infinity_norm<k>(r0);

    constexpr ml_dsa_field::zq_t bound0(γ1 - β);
    constexpr ml_dsa_field::zq_t bound1(γ2 - β);

    if ((z_norm >= ml_dsa_field::zq_t(γ1 - β)) || (r0_norm >= ml_dsa_field::zq_t(γ2 - β))) {
      has_signed = false;
    } else {
      std::array<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> h0{};
      std::array<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> h1{};

      ml_dsa_polyvec::mul_by_poly<k>(c, t0, h0);
      ml_dsa_polyvec::intt<k>(h0);

      std::copy(h0.begin(), h0.end(), h1.begin());

      ml_dsa_polyvec::neg<k>(h0);
      ml_dsa_polyvec::add_to<k>(h1, r1);
      ml_dsa_polyvec::make_hint<k, α>(h0, r1, h);

      const ml_dsa_field::zq_t ct0_norm = ml_dsa_polyvec::infinity_norm<k>(h1);
      const size_t count_1s = ml_dsa_polyvec::count_1s<k>(h);

      constexpr ml_dsa_field::zq_t bound2(γ2);

      if ((ct0_norm >= ml_dsa_field::zq_t(γ2)) || (count_1s > ω)) {
        has_signed = false;
      } else {
        has_signed = true;
      }
    }

    kappa += static_cast<uint16_t>(l);
  }

  constexpr size_t gamma1_bw = std::bit_width(γ1);

  constexpr size_t sigoff0 = 0;
  constexpr size_t sigoff1 = sigoff0 + c_tilda_span.size();
  constexpr size_t sigoff2 = sigoff1 + (32 * l * gamma1_bw);
  constexpr size_t sigoff3 = sig.size();

  std::copy(c_tilda_span.begin(), c_tilda_span.end(), sig.template subspan<sigoff0, sigoff1 - sigoff0>().begin());

  ml_dsa_polyvec::sub_from_x<l, γ1>(z);
  ml_dsa_polyvec::encode<l, gamma1_bw>(z, sig.template subspan<sigoff1, sigoff2 - sigoff1>());

  ml_dsa_bit_packing::encode_hint_bits<k, ω>(h, sig.template subspan<sigoff2, sigoff3 - sigoff2>());

  return has_signed;
}

// Given a ML-DSA public key, message (can be empty too) and serialized signature, this routine verifies the correctness
// of signature, returning boolean result, denoting status of signature verification. For example, say it returns true,
// it means signature is valid for given message and public key.
//
// See algorithm 3 of ML-DSA draft standard @ https://doi.org/10.6028/NIST.FIPS.204.ipd.
template<size_t k, size_t l, size_t d, uint32_t γ1, uint32_t γ2, uint32_t τ, uint32_t β, size_t ω, size_t λ>
static inline constexpr bool
verify(std::span<const uint8_t, ml_dsa_utils::pub_key_len(k, d)> pubkey,
       std::span<const uint8_t> msg,
       std::span<const uint8_t> ctx,
       std::span<const uint8_t, ml_dsa_utils::sig_len(k, l, γ1, ω, λ)> sig)
  requires(ml_dsa_params::check_verify_params(k, l, d, γ1, γ2, τ, β, ω, λ))
{
  if (ctx.size() > std::numeric_limits<uint8_t>::max()) {
    return false;
  }

  constexpr size_t t1_bw = std::bit_width(ml_dsa_field::Q) - d;
  constexpr size_t gamma1_bw = std::bit_width(γ1);

  // Decode signature
  constexpr size_t sigoff0 = 0;
  constexpr size_t sigoff1 = sigoff0 + (2 * λ) / std::numeric_limits<uint8_t>::digits;
  constexpr size_t sigoff2 = sigoff1 + (32 * l * gamma1_bw);
  constexpr size_t sigoff3 = sig.size();

  auto c_tilda = sig.template first<sigoff1 - sigoff0>();
  auto z_encoded = sig.template subspan<sigoff1, sigoff2 - sigoff1>();
  auto h_encoded = sig.template subspan<sigoff2, sigoff3 - sigoff2>();

  std::array<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> h{};
  const bool has_failed = ml_dsa_bit_packing::decode_hint_bits<k, ω>(h_encoded, h);
  if (has_failed) {
    return false;
  }

  const size_t count_1s = ml_dsa_polyvec::count_1s<k>(h);
  if (count_1s > ω) {
    return false;
  }

  std::array<ml_dsa_field::zq_t, ml_dsa_ntt::N> c{};
  ml_dsa_sampling::sample_in_ball<τ, λ>(c_tilda, c);
  ml_dsa_ntt::ntt(c);

  std::array<ml_dsa_field::zq_t, l * ml_dsa_ntt::N> z{};
  ml_dsa_polyvec::decode<l, gamma1_bw>(z_encoded, z);
  ml_dsa_polyvec::sub_from_x<l, γ1>(z);

  const ml_dsa_field::zq_t z_norm = ml_dsa_polyvec::infinity_norm<l>(z);
  if (z_norm >= ml_dsa_field::zq_t(γ1 - β)) {
    return false;
  }

  // Decode public key
  constexpr size_t pkoff0 = 0;
  constexpr size_t pkoff1 = pkoff0 + 32;
  constexpr size_t pkoff2 = pubkey.size();

  auto rho = pubkey.template subspan<pkoff0, pkoff1 - pkoff0>();
  auto t1_encoded = pubkey.template subspan<pkoff1, pkoff2 - pkoff1>();

  std::array<ml_dsa_field::zq_t, k * l * ml_dsa_ntt::N> A{};
  std::array<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> t1{};

  ml_dsa_sampling::expand_a<k, l>(rho, A);
  ml_dsa_polyvec::decode<k, t1_bw>(t1_encoded, t1);

  std::array<uint8_t, 64> tr{};
  std::array<uint8_t, 64> mu{};

  shake256::shake256_t hasher;
  hasher.absorb(pubkey);
  hasher.finalize();
  hasher.squeeze(tr);

  const std::array<uint8_t, 2> domain_separator{ 0, static_cast<uint8_t>(ctx.size()) };

  hasher.reset();
  hasher.absorb(tr);
  hasher.absorb(domain_separator);
  hasher.absorb(ctx);
  hasher.absorb(msg);
  hasher.finalize();
  hasher.squeeze(mu);

  std::array<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> w0{};
  std::array<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> w1{};
  std::array<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> w2{};

  ml_dsa_polyvec::ntt<l>(z);
  ml_dsa_polyvec::matrix_multiply<k, l, l, 1>(A, z, w0);

  ml_dsa_polyvec::shl<k, d>(t1);
  ml_dsa_polyvec::ntt<k>(t1);
  ml_dsa_polyvec::mul_by_poly<k>(c, t1, w2);
  ml_dsa_polyvec::neg<k>(w2);

  ml_dsa_polyvec::add_to<k>(w0, w2);
  ml_dsa_polyvec::intt<k>(w2);

  constexpr uint32_t α = γ2 << 1;
  constexpr uint32_t m = (ml_dsa_field::Q - 1u) / α;
  constexpr size_t w1bw = std::bit_width(m - 1u);

  ml_dsa_polyvec::use_hint<k, α>(h, w2, w1);

  std::array<uint8_t, k * w1bw * 32> w1_encoded{};
  ml_dsa_polyvec::encode<k, w1bw>(w1, w1_encoded);

  std::array<uint8_t, c_tilda.size()> c_tilda_prime{};

  hasher.reset();
  hasher.absorb(mu);
  hasher.absorb(w1_encoded);
  hasher.finalize();
  hasher.squeeze(c_tilda_prime);

  return std::equal(c_tilda.begin(), c_tilda.end(), c_tilda_prime.begin());
}

}
