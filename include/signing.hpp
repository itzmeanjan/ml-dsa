#pragma once
#include "polyvec.hpp"
#include "prng.hpp"
#include "sampling.hpp"
#include "utils.hpp"

// Dilithium Post-Quantum Digital Signature Algorithm
namespace dilithium {

// Given a Dilithium secret key and message, this routine uses Dilithium
// signing algorithm for computing deterministic ( default choice ) or
// randomized signature for input messsage, using provided parameters.
//
// If you're interested in generating randomized signature, you should pass
// truth value to last template parameter ( find `randomized` ). By default,
// this implementation generates deterministic signature i.e. for same message
// M, it'll generate same signature everytime. Note, that when randomized
// signing is enabled ( at compile-time ), randomness of 64 -bytes is sampled
// from system randomness generator, which should ideally be a h/w secure
// element, though that might not be the case on all platforms. Which is why I
// suggest you read
// https://en.cppreference.com/w/cpp/numeric/random/random_device, because
// that's what we use for seeding the random byte generator.
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
static void
sign(const uint8_t* const __restrict seckey,
     const uint8_t* const __restrict msg,
     const size_t mlen,
     uint8_t* const __restrict sig)
  requires(dilithium_params::check_η(η) && dilithium_params::check_d(d) &&
           dilithium_params::check_γ1(γ1) && dilithium_params::check_τ(τ))
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
    prng::prng_t prng;
    prng.read(rho_prime, sizeof(rho_prime));
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

}
