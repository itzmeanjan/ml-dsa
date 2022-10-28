#pragma once
#include "polyvec.hpp"
#include "sampling.hpp"
#include "utils.hpp"

// Dilithium Post-Quantum Digital Signature Algorithm
namespace dilithium {

// Given a Dilithium public key, message and signature, this routine verifies
// the correctness of signature, returning boolean result, denoting status of
// signature verification.
//
// Verification algorithm is described in figure 4 of Dilithium specification,
// as submitted to NIST final round call
// https://csrc.nist.gov/CSRC/media/Projects/post-quantum-cryptography/documents/round-3/submissions/Dilithium-Round3.zip
template<const size_t k,
         const size_t l,
         const size_t d,
         const uint32_t γ1,
         const uint32_t γ2,
         const uint32_t τ,
         const uint32_t β,
         const size_t ω>
static bool
verify(
  const uint8_t* const __restrict pubkey,
  const uint8_t* const __restrict msg,
  const size_t mlen,
  const uint8_t* const __restrict sig) requires(dilithium_utils::check_d(d) &&
                                                dilithium_utils::check_γ1(γ1) &&
                                                dilithium_utils::check_τ(τ))
{
  constexpr size_t t1_bw = std::bit_width(ff::Q) - d;
  constexpr size_t pklen = dilithium_utils::pubkey_length<k, d>();

  constexpr size_t puboff0 = 0;
  constexpr size_t puboff1 = puboff0 + 32;

  constexpr size_t gamma1_bw = std::bit_width(γ1);
  constexpr size_t sigoff0 = 0;
  constexpr size_t sigoff1 = sigoff0 + (32 * l * gamma1_bw);
  constexpr size_t sigoff2 = sigoff1 + (ω + k);

  ff::ff_t A[k * l * ntt::N]{};
  ff::ff_t t1[k * ntt::N]{};

  dilithium_utils::expand_a<k, l>(pubkey + puboff0, A);
  dilithium_utils::polyvec_decode<k, t1_bw>(pubkey + puboff1, t1);

  uint8_t crh_in[48]{};
  uint8_t mu[48]{};

  shake256::shake256 hasher0{};
  hasher0.hash(pubkey, pklen);
  hasher0.read(crh_in, 48);

  shake256::shake256<true> hasher1{};
  hasher1.absorb(crh_in, sizeof(crh_in));
  hasher1.absorb(msg, mlen);
  hasher1.finalize();
  hasher1.read(mu, sizeof(mu));

  ff::ff_t c[ntt::N]{};

  dilithium_utils::sample_in_ball<τ>(sig + sigoff2, c);
  ntt::ntt(c);

  ff::ff_t z[l * ntt::N]{};
  ff::ff_t h[k * ntt::N]{};

  dilithium_utils::polyvec_decode<l, gamma1_bw>(sig + sigoff0, z);
  dilithium_utils::polyvec_sub_from_x<l, γ1>(z);
  bool failed = dilithium_utils::decode_hint_bits<k, ω>(sig + sigoff1, h);

  ff::ff_t w0[k * ntt::N]{};
  ff::ff_t w1[k * ntt::N]{};
  ff::ff_t w2[k * ntt::N]{};

  const ff::ff_t z_norm = dilithium_utils::polyvec_infinity_norm<l>(z);
  const size_t count_1 = dilithium_utils::polyvec_count_1s<k>(h);

  dilithium_utils::polyvec_ntt<l>(z);
  dilithium_utils::matrix_multiply<k, l, l, 1>(A, z, w0);

  dilithium_utils::polyvec_shl<k, d>(t1);
  dilithium_utils::polyvec_ntt<k>(t1);
  dilithium_utils::polyvec_mul_poly<k>(c, t1, w2);
  dilithium_utils::polyvec_neg<k>(w2);

  dilithium_utils::polyvec_add_to<k>(w0, w2);
  dilithium_utils::polyvec_intt<k>(w2);

  dilithium_utils::polyvec_use_hint<k, γ2 << 1>(h, w2, w1);

  constexpr uint32_t m = (ff::Q - 1u) / (γ2 << 1);
  constexpr size_t w1bw = std::bit_width(m - 1u);

  uint8_t hash_in[48 + (k * w1bw * 32)]{};
  uint8_t hash_out[32]{};

  std::memcpy(hash_in, mu, sizeof(mu));
  dilithium_utils::polyvec_encode<k, w1bw>(w1, hash_in + 48);

  shake256::shake256 hasher2{};
  hasher2.hash(hash_in, sizeof(hash_in));
  hasher2.read(hash_out, sizeof(hash_out));

  constexpr ff::ff_t bound0{ γ1 - β };

  const bool flg0 = z_norm < bound0;
  bool flg1 = false;
  const bool flg2 = count_1 <= ω;

  for (size_t i = 0; i < 32; i++) {
    flg1 |= static_cast<bool>(sig[sigoff2 + i] ^ hash_out[i]);
  }

  const bool flg3 = flg0 & !flg1 & flg2;
  const bool flg4 = !failed & flg3;
  return flg4;
}

}
