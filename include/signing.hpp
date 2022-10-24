#pragma once
#include "polyvec.hpp"
#include "sampling.hpp"

// Dilithium Post-Quantum Digital Signature Algorithm
namespace dilithium {

// Given secret key and message ( of statically known length = mlen ), this
// routine uses Dilithium signature generation algorithm for computing
// deterministic signature over input messsage, using provided parameters.
//
// Signing algorithm is described in figure 4 of Dilithium specification, as
// submitted to NIST final round call
// https://csrc.nist.gov/CSRC/media/Projects/post-quantum-cryptography/documents/round-3/submissions/Dilithium-Round3.zip
//
// For Dilithium parameters, see table 2 of specification.
//
// Generated signature is of ((32 * l * gamma1_bw) + (omega + k) + 32) -bytes
//
// s.t. gamma1_bw = floor(log2(gamma1)) + 1
//
// See section 5.4 of specification for understanding how signature is byte
// serialized.
template<const size_t k,
         const size_t l,
         const size_t d,
         const uint32_t eta,
         const uint32_t gamma1,
         const uint32_t gamma2,
         const uint32_t tau,
         const uint32_t beta,
         const size_t omega,
         const size_t mlen>
static void
sign(const uint8_t* const __restrict seckey,
     const uint8_t* const __restrict msg,
     uint8_t* const __restrict sig)
{
  constexpr uint32_t t0_rng = 1u << (d - 1);

  constexpr size_t eta_bw = std::bit_width(2 * eta);
  constexpr size_t s1_len = l * eta_bw * 32;
  constexpr size_t s2_len = k * eta_bw * 32;

  constexpr size_t secoff0 = 0;
  constexpr size_t secoff1 = secoff0 + 32;
  constexpr size_t secoff2 = secoff1 + 32;
  constexpr size_t secoff3 = secoff2 + 48;
  constexpr size_t secoff4 = secoff3 + s1_len;
  constexpr size_t secoff5 = secoff4 + s2_len;

  const uint8_t* const rho = seckey + secoff0;
  const uint8_t* const key = seckey + secoff1;
  const uint8_t* const tr = seckey + secoff2;

  ff::ff_t A[k * l * ntt::N]{};

  dilithium_utils::expand_a<k, l>(rho, A);

  uint8_t crh_in[48 + mlen]{};
  uint8_t mu[48]{};

  std::memcpy(crh_in + 0, tr, 48);
  std::memcpy(crh_in + 48, msg, mlen);

  shake256::shake256 crh0{};
  crh0.hash(crh_in, sizeof(crh_in));
  crh0.read(mu, sizeof(mu));

  uint8_t rho_prime[48]{};

  std::memcpy(crh_in + 0, key, 32);
  std::memcpy(crh_in + 32, mu, sizeof(mu));

  shake256::shake256 crh1{};
  crh1.hash(crh_in, 32 + sizeof(mu));
  crh1.read(rho_prime, sizeof(rho_prime));

  ff::ff_t s1[l * ntt::N]{};
  ff::ff_t s2[k * ntt::N]{};
  ff::ff_t t0[k * ntt::N]{};

  dilithium_utils::polyvec_decode<l, eta_bw>(seckey + secoff3, s1);
  dilithium_utils::polyvec_decode<k, eta_bw>(seckey + secoff4, s2);
  dilithium_utils::polyvec_decode<k, d>(seckey + secoff5, t0);

  dilithium_utils::polyvec_sub_from_x<l, eta>(s1);
  dilithium_utils::polyvec_sub_from_x<k, eta>(s2);
  dilithium_utils::polyvec_sub_from_x<k, t0_rng>(t0);

  dilithium_utils::polyvec_ntt<l>(s1);
  dilithium_utils::polyvec_ntt<k>(s2);
  dilithium_utils::polyvec_ntt<k>(t0);

  bool has_signed = false;
  size_t nonce = 0;

  ff::ff_t z[l * ntt::N]{};
  ff::ff_t h[k * ntt::N]{};
  uint8_t hash_out[32]{};

  while (!has_signed) {
    ff::ff_t y[l * ntt::N]{};
    ff::ff_t y_prime[l * ntt::N]{};
    ff::ff_t w[k * ntt::N]{};

    dilithium_utils::expand_mask<gamma1, l>(rho_prime, nonce, y);
    std::memcpy(y_prime, y, sizeof(y));

    dilithium_utils::polyvec_ntt<l>(y_prime);
    dilithium_utils::matrix_multiply<k, l, l, 1>(A, y_prime, w);
    dilithium_utils::polyvec_intt<k>(w);

    constexpr uint32_t m = (ff::Q - 1u) / (gamma2 << 1);
    constexpr size_t w1bw = std::bit_width(m - 1u);

    ff::ff_t w1[k * ntt::N]{};
    uint8_t hash_in[48 + (k * w1bw * 32)]{};
    ff::ff_t c[ntt::N]{};

    dilithium_utils::polyvec_highbits<k, gamma2 << 1>(w, w1);

    std::memcpy(hash_in, mu, 48);
    dilithium_utils::polyvec_encode<k, w1bw>(w1, hash_in + 48);

    shake256::shake256 hasher{};
    hasher.hash(hash_in, sizeof(hash_in));
    hasher.read(hash_out, sizeof(hash_out));

    dilithium_utils::sample_in_ball<tau>(hash_out, c);
    ntt::ntt(c);

    dilithium_utils::polyvec_mul_poly<l>(c, s1, z);
    dilithium_utils::polyvec_intt<l>(z);
    dilithium_utils::polyvec_add_to<l>(y, z);

    ff::ff_t r0[k * ntt::N]{};
    ff::ff_t r1[k * ntt::N]{};

    dilithium_utils::polyvec_mul_poly<k>(c, s2, r1);
    dilithium_utils::polyvec_intt<k>(r1);
    dilithium_utils::polyvec_neg<k>(r1);
    dilithium_utils::polyvec_add_to<k>(w, r1);
    dilithium_utils::polyvec_lowbits<k, gamma2 << 1>(r1, r0);

    const ff::ff_t z_norm = dilithium_utils::polyvec_infinity_norm<l>(z);
    const ff::ff_t r0_norm = dilithium_utils::polyvec_infinity_norm<k>(r0);

    constexpr ff::ff_t bound0{ gamma1 - beta };
    constexpr ff::ff_t bound1{ gamma2 - beta };

    const bool flg0 = z_norm >= bound0;
    const bool flg1 = r0_norm >= bound1;
    const bool flg2 = flg0 | flg1;

    has_signed = !flg2;

    ff::ff_t h0[k * ntt::N]{};
    ff::ff_t h1[k * ntt::N]{};

    dilithium_utils::polyvec_mul_poly<k>(c, t0, h0);
    dilithium_utils::polyvec_intt<k>(h0);
    std::memcpy(h1, h0, sizeof(h0));
    dilithium_utils::polyvec_neg<k>(h0);
    dilithium_utils::polyvec_add_to<k>(h1, r1);
    dilithium_utils::polyvec_make_hint<k, gamma2 << 1>(h0, r1, h);

    const ff::ff_t ct0_norm = dilithium_utils::polyvec_infinity_norm<k>(h1);
    const size_t count_1 = dilithium_utils::polyvec_count_1s<k>(h);

    constexpr ff::ff_t bound2{ gamma2 };

    const bool flg3 = ct0_norm >= bound2;
    const bool flg4 = count_1 > omega;
    const bool flg5 = flg3 | flg4;

    has_signed = has_signed & !flg5;
    nonce += l;
  }

  constexpr size_t gamma1_bw = std::bit_width(gamma1);
  constexpr size_t sigoff0 = 0;
  constexpr size_t sigoff1 = sigoff0 + (32 * l * gamma1_bw);
  constexpr size_t sigoff2 = sigoff1 + (omega + k);

  dilithium_utils::polyvec_sub_from_x<l, gamma1>(z);
  dilithium_utils::polyvec_encode<l, gamma1_bw>(z, sig + sigoff0);
  dilithium_utils::encode_hint_bits<k, omega>(h, sig + sigoff1);
  std::memcpy(sig + sigoff2, hash_out, 32);
}

}
