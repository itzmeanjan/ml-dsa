#pragma once
#include "ml_dsa/internals/poly/polyvec.hpp"
#include "ml_dsa/internals/poly/sampling.hpp"
#include "ml_dsa/internals/utility/params.hpp"
#include "ml_dsa/internals/utility/utils.hpp"
#include <limits>
#include <span>

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
template<size_t k, size_t l, size_t d, uint32_t η>
static inline void
keygen(std::span<const uint8_t, 32> seed,
       std::span<uint8_t, ml_dsa_utils::pub_key_len(k, d)> pubkey,
       std::span<uint8_t, ml_dsa_utils::sec_key_len(k, l, η, d)> seckey)
  requires(ml_dsa_params::check_keygen_params(k, l, d, η))
{
  std::array<uint8_t, 32 + 64 + 32> seed_hash{};
  auto _seed_hash = std::span(seed_hash);

  shake256::shake256_t hasher;
  hasher.absorb(seed);
  hasher.finalize();
  hasher.squeeze(_seed_hash);

  auto rho = _seed_hash.template subspan<0, 32>();
  auto rho_prime = _seed_hash.template subspan<rho.size(), 64>();
  auto key = _seed_hash.template subspan<rho.size() + rho_prime.size(), 32>();

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

  std::memcpy(pubkey.template subspan<pkoff0, pkoff1 - pkoff0>().data(), rho.data(), rho.size());
  ml_dsa_polyvec::encode<k, t1_bw>(t1, pubkey.template subspan<pkoff1, pkoff2 - pkoff1>());

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

  std::memcpy(seckey.template subspan<skoff0, skoff1 - skoff0>().data(), rho.data(), rho.size());
  std::memcpy(seckey.template subspan<skoff1, skoff2 - skoff1>().data(), key.data(), key.size());
  std::memcpy(seckey.template subspan<skoff2, skoff3 - skoff2>().data(), tr.data(), tr.size());

  ml_dsa_polyvec::sub_from_x<l, η>(s1);
  ml_dsa_polyvec::sub_from_x<k, η>(s2);

  ml_dsa_polyvec::encode<l, eta_bw>(s1, seckey.template subspan<skoff3, skoff4 - skoff3>());
  ml_dsa_polyvec::encode<k, eta_bw>(s2, seckey.template subspan<skoff4, skoff5 - skoff4>());

  constexpr uint32_t t0_rng = 1u << (d - 1);

  ml_dsa_polyvec::sub_from_x<k, t0_rng>(t0);
  ml_dsa_polyvec::encode<k, d>(t0, seckey.template subspan<skoff5, skoff6 - skoff5>());
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
template<size_t k, size_t l, size_t d, uint32_t η, uint32_t γ1, uint32_t γ2, uint32_t τ, uint32_t β, size_t ω, size_t λ>
static inline void
sign(std::span<const uint8_t, 32> rnd,
     std::span<const uint8_t, ml_dsa_utils::sec_key_len(k, l, η, d)> seckey,
     std::span<const uint8_t> msg,
     std::span<uint8_t, ml_dsa_utils::sig_len(k, l, γ1, ω, λ)> sig)
  requires(ml_dsa_params::check_signing_params(k, l, d, η, γ1, γ2, τ, β, ω, λ))
{
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
  auto _mu = std::span(mu);

  shake256::shake256_t hasher;
  hasher.absorb(tr);
  hasher.absorb(msg);
  hasher.finalize();
  hasher.squeeze(_mu);

  std::array<uint8_t, 64> rho_prime{};

  hasher.reset();
  hasher.absorb(key);
  hasher.absorb(rnd);
  hasher.absorb(_mu);
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
  auto c1_tilda = c_tilda_span.template first<32>();
  auto c2_tilda = c_tilda_span.template last<32>();

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
    std::array<uint8_t, _mu.size() + (k * w1bw * 32)> hash_in{};
    auto _hash_in = std::span(hash_in);
    std::array<ml_dsa_field::zq_t, ml_dsa_ntt::N> c{};

    ml_dsa_polyvec::highbits<k, α>(w, w1);

    std::memcpy(_hash_in.template subspan<0, _mu.size()>().data(), _mu.data(), _mu.size());
    ml_dsa_polyvec::encode<k, w1bw>(w1, _hash_in.template subspan<_mu.size(), _hash_in.size() - _mu.size()>());

    hasher.reset();
    hasher.absorb(_hash_in);
    hasher.finalize();
    hasher.squeeze(c_tilda);

    ml_dsa_sampling::sample_in_ball<τ>(c1_tilda, c);
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

    const bool flg0 = z_norm >= bound0;
    const bool flg1 = r0_norm >= bound1;
    const bool flg2 = flg0 | flg1;

    has_signed = !flg2;

    std::array<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> h0{};
    std::array<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> h1{};

    ml_dsa_polyvec::mul_by_poly<k>(c, t0, h0);
    ml_dsa_polyvec::intt<k>(h0);
    std::copy(h0.begin(), h0.end(), h1.begin());
    ml_dsa_polyvec::neg<k>(h0);
    ml_dsa_polyvec::add_to<k>(h1, r1);
    ml_dsa_polyvec::make_hint<k, α>(h0, r1, h);

    const ml_dsa_field::zq_t ct0_norm = ml_dsa_polyvec::infinity_norm<k>(h1);
    const size_t count_1 = ml_dsa_polyvec::count_1s<k>(h);

    constexpr ml_dsa_field::zq_t bound2(γ2);

    const bool flg3 = ct0_norm >= bound2;
    const bool flg4 = count_1 > ω;
    const bool flg5 = flg3 | flg4;

    has_signed = has_signed & !flg5;
    kappa += static_cast<uint16_t>(l);
  }

  constexpr size_t gamma1_bw = std::bit_width(γ1);
  constexpr size_t sigoff0 = 0;
  constexpr size_t sigoff1 = sigoff0 + c_tilda.size();
  constexpr size_t sigoff2 = sigoff1 + (32 * l * gamma1_bw);
  constexpr size_t sigoff3 = sig.size();

  std::memcpy(sig.template subspan<sigoff0, sigoff1 - sigoff0>().data(), c_tilda.data(), c_tilda.size());
  ml_dsa_polyvec::sub_from_x<l, γ1>(z);
  ml_dsa_polyvec::encode<l, gamma1_bw>(z, sig.template subspan<sigoff1, sigoff2 - sigoff1>());
  ml_dsa_bit_packing::encode_hint_bits<k, ω>(h, sig.template subspan<sigoff2, sigoff3 - sigoff2>());
}

// Given a Dilithium public key, message bytes and serialized signature, this
// routine verifies the correctness of signature, returning boolean result,
// denoting status of signature verification. For example, say it returns true,
// it means signature has successfully been verified.
//
// Verification algorithm is described in figure 4 of Dilithium specification
// https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf
template<size_t k, size_t l, size_t d, uint32_t γ1, uint32_t γ2, uint32_t τ, uint32_t β, size_t ω, size_t λ>
static inline bool
verify(std::span<const uint8_t, ml_dsa_utils::pub_key_len(k, d)> pubkey,
       std::span<const uint8_t> msg,
       std::span<const uint8_t, ml_dsa_utils::sig_len(k, l, γ1, ω, λ)> sig)
  requires(ml_dsa_params::check_verify_params(k, l, d, γ1, γ2, τ, β, ω, λ))
{
  constexpr size_t t1_bw = std::bit_width(ml_dsa_field::Q) - d;

  constexpr size_t pkoff0 = 0;
  constexpr size_t pkoff1 = pkoff0 + 32;
  constexpr size_t pkoff2 = pubkey.size();

  constexpr size_t gamma1_bw = std::bit_width(γ1);
  constexpr size_t sigoff0 = 0;
  constexpr size_t sigoff1 = sigoff0 + (2 * λ) / std::numeric_limits<uint8_t>::digits;
  constexpr size_t sigoff2 = sigoff1 + (32 * l * gamma1_bw);
  constexpr size_t sigoff3 = sig.size();

  std::array<ml_dsa_field::zq_t, k * l * ml_dsa_ntt::N> A{};
  std::array<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> t1{};

  ml_dsa_sampling::expand_a<k, l>(pubkey.template subspan<pkoff0, pkoff1 - pkoff0>(), A);
  ml_dsa_polyvec::decode<k, t1_bw>(pubkey.template subspan<pkoff1, pkoff2 - pkoff1>(), t1);

  std::array<uint8_t, 64> tr{};
  std::array<uint8_t, 64> mu{};

  shake256::shake256_t hasher;
  hasher.absorb(pubkey);
  hasher.finalize();
  hasher.squeeze(tr);

  hasher.reset();
  hasher.absorb(tr);
  hasher.absorb(msg);
  hasher.finalize();
  hasher.squeeze(mu);

  std::array<ml_dsa_field::zq_t, ml_dsa_ntt::N> c{};
  auto c_tilda = sig.template first<sigoff1 - sigoff0>();
  auto c1_tilda = c_tilda.template first<32>();
  auto c2_tilda = c_tilda.template last<32>();

  ml_dsa_sampling::sample_in_ball<τ>(c1_tilda, c);
  ml_dsa_ntt::ntt(c);

  std::array<ml_dsa_field::zq_t, l * ml_dsa_ntt::N> z{};
  std::array<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> h{};

  ml_dsa_polyvec::decode<l, gamma1_bw>(sig.template subspan<sigoff1, sigoff2 - sigoff1>(), z);
  ml_dsa_polyvec::sub_from_x<l, γ1>(z);
  const bool failed = ml_dsa_bit_packing::decode_hint_bits<k, ω>(sig.template subspan<sigoff2, sigoff3 - sigoff2>(), h);

  std::array<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> w0{};
  std::array<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> w1{};
  std::array<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> w2{};

  const ml_dsa_field::zq_t z_norm = ml_dsa_polyvec::infinity_norm<l>(z);
  const size_t count_1 = ml_dsa_polyvec::count_1s<k>(h);

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

  std::array<uint8_t, mu.size() + (k * w1bw * 32)> hash_in{};
  std::array<uint8_t, (2 * λ) / std::numeric_limits<uint8_t>::digits> c_tilda_prime{};

  auto _hash_in = std::span(hash_in);

  std::memcpy(_hash_in.template subspan<0, mu.size()>().data(), mu.data(), mu.size());
  ml_dsa_polyvec::encode<k, w1bw>(w1, _hash_in.template subspan<mu.size(), _hash_in.size() - mu.size()>());

  hasher.reset();
  hasher.absorb(_hash_in);
  hasher.finalize();
  hasher.squeeze(c_tilda_prime);

  constexpr ml_dsa_field::zq_t bound0(γ1 - β);

  const bool flg0 = z_norm < bound0;
  bool flg1 = false;
  const bool flg2 = count_1 <= ω;

  for (size_t i = 0; i < c_tilda_prime.size(); i++) {
    flg1 |= static_cast<bool>(sig[sigoff0 + i] ^ c_tilda_prime[i]);
  }

  const bool flg3 = flg0 & !flg1 & flg2;
  const bool flg4 = !failed & flg3;
  return flg4;
}

}
