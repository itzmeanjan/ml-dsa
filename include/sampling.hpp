#pragma once
#include "ff.hpp"
#include "ntt.hpp"
#include "poly.hpp"
#include "shake128.hpp"
#include "shake256.hpp"
#include <cstring>

// Utility functions for Dilithium Post-Quantum Digital Signature Algorithm
namespace dilithium_utils {

// Given a 32 -bytes uniform seed ρ, a k x l matrix is deterministically
// sampled, where each coefficient is a degree-255 polynomial ∈ R_q
// | q = 2^23 - 2^13 + 1
//
// SHAKE128 XOF is used for expanding 32 -bytes seed to matrix over R_q^(k x l).
//
// See `Expanding the Matrix A` point in section 5.3 of Dilithium specification,
// https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf
template<const size_t k, const size_t l>
static void
expand_a(const uint8_t* const __restrict rho, ff::ff_t* const __restrict mat)
{
  uint8_t msg[34]{};
  uint8_t buf[3]{};

  std::memcpy(msg, rho, 32);

  for (size_t i = 0; i < k; i++) {
    for (size_t j = 0; j < l; j++) {
      const size_t off = (i * l + j) * ntt::N;
      const uint16_t nonce = static_cast<uint16_t>(i * 256ul + j);

      msg[32] = static_cast<uint8_t>(nonce >> 0);
      msg[33] = static_cast<uint8_t>(nonce >> 8);

      shake128::shake128 hasher{};
      hasher.hash(msg, sizeof(msg));

      size_t n = 0;
      while (n < ntt::N) {
        hasher.read(buf, sizeof(buf));

        const uint32_t t0 = static_cast<uint32_t>(buf[2] & 0b01111111);
        const uint32_t t1 = static_cast<uint32_t>(buf[1]);
        const uint32_t t2 = static_cast<uint32_t>(buf[0]);

        const uint32_t t3 = (t0 << 16) ^ (t1 << 8) ^ (t2 << 0);
        const bool flg = t3 < ff::Q;

        mat[off + n] = ff::ff_t{ static_cast<uint32_t>(t3 * flg) };
        n = n + flg * 1;
      }
    }
  }
}

// Compile-time check to ensure that η ∈ {2, 4}, so that sampled secret key
// range stays short i.e. [-η, η]
inline static constexpr bool
check_η(const uint32_t η)
{
  return (η == 2u) || (η == 4u);
}

// Compile-time check to ensure that starting nonce belongs to allowed set of
// values when uniform sampling polynomial coefficients in [-η, η]
inline static constexpr bool
check_nonce(const size_t nonce)
{
  return (nonce == 0) || (nonce == 4) || (nonce == 5) || (nonce == 7);
}

// Uniform sampling k -many degree-255 polynomials s.t. each coefficient of
// those polynomials ∈ [-η, η].
//
// Sampling is performed deterministically, by seeding SHAKE256 XOF with
// 64 -bytes seed and two byte nonce, whose starting value is provided ( see
// template parameter ). Consecutive nonces are computed by adding 1 to previous
// one.
//
// Note, sampled polynomial coefficients are kept in canonical form.
//
// See `Sampling the vectors s1 and s2` point in section 5.3 of Dilithium
// specification
// https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf
template<const uint32_t η, const size_t k, const uint16_t nonce>
static void
expand_s(const uint8_t* const __restrict rho_prime,
         ff::ff_t* const __restrict vec)
  requires(check_η(η) && check_nonce(nonce))
{
  constexpr ff::ff_t eta_{ η };

  uint8_t msg[66]{};
  uint8_t buf = 0;

  std::memcpy(msg, rho_prime, 64);

  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ntt::N;
    const uint16_t nonce_ = nonce + static_cast<uint16_t>(i);

    msg[64] = static_cast<uint8_t>(nonce_ >> 0);
    msg[65] = static_cast<uint8_t>(nonce_ >> 8);

    shake256::shake256 hasher{};
    hasher.hash(msg, sizeof(msg));

    size_t n = 0;
    while (n < ntt::N) {
      hasher.read(&buf, 1);

      const uint8_t t0 = buf & 0x0f;
      const uint8_t t1 = buf >> 4;

      if constexpr (η == 2u) {
        const uint32_t t2 = static_cast<uint32_t>(t0 % 5);
        const bool flg0 = t0 < 15;

        vec[off + n] = eta_ - ff::ff_t{ t2 };
        n += flg0 * 1;

        const uint32_t t3 = static_cast<uint32_t>(t1 % 5);
        const bool flg1 = (t1 < 15) & (n < ntt::N);
        const ff::ff_t br[]{ vec[off], eta_ - ff::ff_t{ t3 } };

        vec[off + flg1 * n] = br[flg1];
        n += flg1 * 1;
      } else {
        const bool flg0 = t0 < 9;

        vec[off + n] = eta_ - ff::ff_t{ static_cast<uint32_t>(t0) };
        n += flg0 * 1;

        const bool flg1 = (t1 < 9) & (n < ntt::N);
        const ff::ff_t t2 = eta_ - ff::ff_t{ static_cast<uint32_t>(t1) };
        const ff::ff_t br[]{ vec[off], t2 };

        vec[off + flg1 * n] = br[flg1];
        n += flg1 * 1;
      }
    }
  }
}

// Compile-time check to ensure that γ1 has recommended value
static inline constexpr bool
check_γ1(const uint32_t γ1)
{
  return (γ1 == (1u << 17)) || (γ1 == (1u << 19));
}

// Given a 64 -bytes seed and 2 -bytes nonce, this routine does uniform sampling
// from output of SHAKE256 XOF, computing a l x 1 vector of degree-255
// polynomials s.t. each coefficient ∈ [-(γ1-1), γ1]
//
// See `Sampling the vectors y` point in section 5.3 of Dilithium
// specification
// https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf
template<const uint32_t γ1, const size_t l>
static void
expand_mask(const uint8_t* const __restrict seed,
            const uint16_t nonce,
            ff::ff_t* const __restrict vec)
  requires(check_γ1(γ1))
{
  constexpr size_t gbw = std::bit_width(2 * γ1 - 1u);

  uint8_t msg[66]{};
  uint8_t buf[gbw * 32]{};

  std::memcpy(msg, seed, 64);

  for (size_t i = 0; i < l; i++) {
    const size_t off = i * ntt::N;
    const uint16_t nonce_ = nonce + static_cast<uint16_t>(i);

    msg[64] = static_cast<uint8_t>(nonce_ >> 0);
    msg[65] = static_cast<uint8_t>(nonce_ >> 8);

    shake256::shake256 hasher{};
    hasher.hash(msg, sizeof(msg));
    hasher.read(buf, sizeof(buf));

    decode<gbw>(buf, vec + off);
    poly_sub_from_x<γ1>(vec + off);
  }
}

// Compile-time check to ensure that τ is set to parameter recommended in
// Dilithium specification
static inline constexpr bool
check_τ(const uint32_t τ)
{
  return (τ == 39) || (τ == 49) || (τ == 60);
}

// Given a 32 -bytes seed, this routine creates a degree-255 polynomial with τ
// -many coefficients set to +/- 1, while remaining (256 - τ) -many set to 0.
//
// See hashing to a ball algorithm described in figure 2 and section 5.3 of
// Dilithium specification
// https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf
template<const uint32_t τ>
static void
sample_in_ball(const uint8_t* const __restrict seed,
               ff::ff_t* const __restrict poly)
  requires(check_τ(τ))
{
  uint8_t tau_bits[8]{};
  uint8_t buf = 0;

  shake256::shake256 hasher{};
  hasher.hash(seed, 32);
  hasher.read(tau_bits, sizeof(tau_bits));

  constexpr size_t frm = ntt::N - τ;
  size_t i = frm;

  while (i < ntt::N) {
    const size_t tau_bit = i - frm;

    const size_t tau_byte_off = tau_bit >> 3;
    const size_t tau_bit_off = tau_bit & 7ul;

    const uint8_t s = (tau_bits[tau_byte_off] >> tau_bit_off) & 0b1;
    const bool s_ = static_cast<bool>(s);

    hasher.read(&buf, 1);

    const bool flg = buf <= static_cast<uint8_t>(i);

    const ff::ff_t br0[]{ poly[i], poly[buf] };
    const ff::ff_t br1[]{ poly[buf], ff::ff_t{ 1u } - ff::ff_t{ 2u * s_ } };

    poly[i] = br0[flg];
    poly[buf] = br1[flg];

    i += 1ul * flg;
  };
}

}
