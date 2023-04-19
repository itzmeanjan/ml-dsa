#pragma once
#include "params.hpp"
#include "polyvec.hpp"
#include "sampling.hpp"
#include "utils.hpp"

// Dilithium Post-Quantum Digital Signature Algorithm
namespace dilithium {

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
static bool
verify(const uint8_t* const __restrict pubkey,
       const uint8_t* const __restrict msg,
       const size_t mlen,
       const uint8_t* const __restrict sig)
  requires(dilithium_params::check_verify_params(k, l, d, γ1, γ2, τ, β, ω))
{
  constexpr size_t t1_bw = std::bit_width(field::Q) - d;
  constexpr size_t pklen = dilithium_utils::pubkey_length<k, d>();

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
