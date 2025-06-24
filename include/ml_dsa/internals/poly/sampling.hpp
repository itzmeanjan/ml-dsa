#pragma once
#include "bit_packing.hpp"
#include "ml_dsa/internals/math/field.hpp"
#include "ml_dsa/internals/utility/params.hpp"
#include "ntt.hpp"
#include "poly.hpp"
#include "sha3/shake128.hpp"
#include "sha3/shake256.hpp"
#include <cstdint>
#include <limits>

// Routines related to sampling of polynomials/ vector of polynomials
namespace ml_dsa_sampling {

using poly_t = std::span<ml_dsa_field::zq_t, ml_dsa_ntt::N>;

// Given a 32 -bytes uniform seed ρ, a k x l matrix is deterministically sampled ( using the method of rejection
// sampling ), where each coefficient is a degree-255 polynomial ∈ R_q.
//
// See algorithm 32 of ML-DSA standard @ https://doi.org/10.6028/NIST.FIPS.204.
template<size_t k, size_t l>
static inline constexpr void
expand_a(std::span<const uint8_t, 32> rho, std::span<ml_dsa_field::zq_t, k * l * ml_dsa_ntt::N> mat)
{
  std::array<uint8_t, rho.size() + 2> msg{};
  auto msg_span = std::span(msg);

  std::copy(rho.begin(), rho.end(), msg_span.begin());

  for (size_t i = 0; i < k; i++) {
    for (size_t j = 0; j < l; j++) {
      const size_t off = (i * l + j) * ml_dsa_ntt::N;

      msg[32] = static_cast<uint8_t>(j);
      msg[33] = static_cast<uint8_t>(i);

      shake128::shake128_t hasher;
      hasher.absorb(msg_span);
      hasher.finalize();

      std::array<uint8_t, shake128::RATE / std::numeric_limits<uint8_t>::digits> buf{};
      auto buf_span = std::span(buf);

      size_t n = 0;
      while (n < ml_dsa_ntt::N) {
        hasher.squeeze(buf_span);

        for (size_t boff = 0; (boff < buf_span.size()) && (n < ml_dsa_ntt::N); boff += 3) {
          const uint32_t t0 = static_cast<uint32_t>(buf_span[boff + 2] & 0b01111111);
          const uint32_t t1 = static_cast<uint32_t>(buf_span[boff + 1]);
          const uint32_t t2 = static_cast<uint32_t>(buf_span[boff + 0]);

          const uint32_t t3 = (t0 << 16) ^ (t1 << 8) ^ (t2 << 0);
          if (t3 < ml_dsa_field::Q) {
            mat[off + n] = ml_dsa_field::zq_t(t3);
            n++;
          }
        }
      }
    }
  }
}

// Uniform rejection sampling k -many degree-255 polynomials s.t. each coefficient of those polynomials ∈ [-eta, eta].
//
// Sampling is performed deterministically, by seeding Shake256 Xof with 64 -bytes seed and two nonce bytes, whose
// starting value is provided ( see template parameter ). Consecutive nonces are computed by adding 1 to previous value.
//
// Note, sampled polynomial coefficients are kept in canonical form.
//
// See algorithm 33 of ML-DSA standard @ https://doi.org/10.6028/NIST.FIPS.204.
template<uint32_t eta, size_t k, uint16_t nonce>
static inline constexpr void
expand_s(std::span<const uint8_t, 64> rho_prime, std::span<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> vec)
  requires(ml_dsa_params::check_eta(eta) && ml_dsa_params::check_nonce(nonce))
{
  constexpr auto eta_value = ml_dsa_field::zq_t(eta);

  std::array<uint8_t, rho_prime.size() + 2> msg{};
  auto msg_span = std::span(msg);

  std::copy(rho_prime.begin(), rho_prime.end(), msg_span.begin());

  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ml_dsa_ntt::N;
    const uint16_t new_nonce = nonce + static_cast<uint16_t>(i);

    msg[64] = static_cast<uint8_t>(new_nonce >> 0);
    msg[65] = static_cast<uint8_t>(new_nonce >> 8);

    shake256::shake256_t hasher;
    hasher.absorb(msg_span);
    hasher.finalize();

    std::array<uint8_t, shake256::RATE / std::numeric_limits<uint8_t>::digits> buf{};
    auto buf_span = std::span(buf);

    size_t n = 0;
    while (n < ml_dsa_ntt::N) {
      hasher.squeeze(buf_span);

      for (size_t boff = 0; (boff < buf_span.size()) && (n < ml_dsa_ntt::N); boff++) {
        const uint8_t t0 = buf_span[boff] & 0x0f;
        const uint8_t t1 = buf_span[boff] >> 4;

        if constexpr (eta_value == 2u) {
          const uint32_t t2 = static_cast<uint32_t>(t0 % 5);
          const bool flg0 = t0 < 15;

          vec[off + n] = eta_value - ml_dsa_field::zq_t(t2);
          n += flg0 * 1;

          const uint32_t t3 = static_cast<uint32_t>(t1 % 5);
          const bool flg1 = (t1 < 15) & (n < ml_dsa_ntt::N);
          const ml_dsa_field::zq_t br[]{ vec[off], eta_value - ml_dsa_field::zq_t(t3) };

          vec[off + flg1 * n] = br[flg1];
          n += flg1 * 1;
        } else {
          const bool flg0 = t0 < 9;

          vec[off + n] = eta_value - ml_dsa_field::zq_t(static_cast<uint32_t>(t0));
          n += flg0 * 1;

          const bool flg1 = (t1 < 9) & (n < ml_dsa_ntt::N);
          const auto t2 = eta_value - ml_dsa_field::zq_t(static_cast<uint32_t>(t1));
          const ml_dsa_field::zq_t br[]{ vec[off], t2 };

          vec[off + flg1 * n] = br[flg1];
          n += flg1 * 1;
        }
      }
    }
  }
}

// Given a 64 -bytes seed and 2 -bytes nonce, this routine does uniform sampling from output of Shake256 Xof, computing
// a l x 1 vector of degree-255 polynomials s.t. each coefficient ∈ [-(gamma1-1), gamma1].
//
// See algorithm 34 of ML-DSA standard @ https://doi.org/10.6028/NIST.FIPS.204.
template<uint32_t gamma1, size_t l>
static inline constexpr void
expand_mask(std::span<const uint8_t, 64> seed, const uint16_t nonce, std::span<ml_dsa_field::zq_t, l * ml_dsa_ntt::N> vec)
  requires(ml_dsa_params::check_gamma1(gamma1))
{
  constexpr size_t gamma1_bitwidth = std::bit_width(gamma1);

  std::array<uint8_t, seed.size() + 2> msg{};
  std::array<uint8_t, (ml_dsa_ntt::N * gamma1_bitwidth) / std::numeric_limits<uint8_t>::digits> buf{};

  auto msg_span = std::span(msg);
  auto buf_span = std::span(buf);

  std::copy(seed.begin(), seed.end(), msg_span.begin());

  for (size_t i = 0; i < l; i++) {
    const size_t off = i * ml_dsa_ntt::N;
    const uint16_t new_nonce = nonce + static_cast<uint16_t>(i);

    msg[64] = static_cast<uint8_t>(new_nonce >> 0);
    msg[65] = static_cast<uint8_t>(new_nonce >> 8);

    shake256::shake256_t hasher;
    hasher.absorb(msg_span);
    hasher.finalize();
    hasher.squeeze(buf_span);

    ml_dsa_bit_packing::decode<gamma1_bitwidth>(buf_span, poly_t(vec.subspan(off, ml_dsa_ntt::N)));
    ml_dsa_poly::sub_from_x<gamma1>(poly_t(vec.subspan(off, ml_dsa_ntt::N)));
  }
}

// Given a (lambda/4) -bytes seed, this routine creates a degree-255 polynomial with tau -many coefficients set to +/- 1, while
// remaining (256 - tau) -many set to 0 s.t. lambda = bit-security level of the respective ML-DSA instantiation.
//
// See algorithm 29 of ML-DSA standard @ https://doi.org/10.6028/NIST.FIPS.204.
template<uint32_t tau, size_t lambda>
static inline constexpr void
sample_in_ball(std::span<const uint8_t, (2 * lambda) / std::numeric_limits<uint8_t>::digits> seed, std::span<ml_dsa_field::zq_t, ml_dsa_ntt::N> poly)
  requires(ml_dsa_params::check_tau(tau))
{
  std::array<uint8_t, 8> tau_bits{};
  std::array<uint8_t, shake256::RATE / std::numeric_limits<uint8_t>::digits> buf{};

  auto tau_bits_span = std::span(tau_bits);
  auto buf_span = std::span(buf);

  shake256::shake256_t hasher;
  hasher.absorb(seed);
  hasher.finalize();
  hasher.squeeze(tau_bits_span);

  constexpr size_t frm = ml_dsa_ntt::N - tau;
  size_t i = frm;

  while (i < ml_dsa_ntt::N) {
    hasher.squeeze(buf_span);

    for (size_t off = 0; (off < buf_span.size()) && (i < ml_dsa_ntt::N); off++) {
      const size_t tau_bit = i - frm;

      const size_t tau_byte_off = tau_bit >> 3;
      const size_t tau_bit_off = tau_bit & 7ul;

      const uint8_t s = (tau_bits_span[tau_byte_off] >> tau_bit_off) & 0b1;
      const bool s_ = static_cast<bool>(s);

      const auto tmp = buf_span[off];
      const bool flg = tmp <= static_cast<uint8_t>(i);

      const ml_dsa_field::zq_t br0[]{ poly[i], poly[tmp] };
      const ml_dsa_field::zq_t br1[]{ poly[tmp], ml_dsa_field::zq_t::one() - ml_dsa_field::zq_t(2u * s_) };

      poly[i] = br0[flg];
      poly[tmp] = br1[flg];

      i += 1ul * flg;
    }
  };
}

}
