#pragma once
#include "bit_packing.hpp"
#include "field.hpp"
#include "ntt.hpp"
#include "params.hpp"
#include "poly.hpp"
#include "shake128.hpp"
#include "shake256.hpp"
#include <array>
#include <cstdint>
#include <cstring>

// Sampling polynomials/ vector of polynomials related routines
namespace sampling {

using poly_t = std::span<field::zq_t, ntt::N>;

// Given a 32 -bytes uniform seed ρ, a k x l matrix is deterministically sampled ( using the method of rejection
// sampling ), where each coefficient is a degree-255 polynomial ∈ R_q | q = 2^23 - 2^13 + 1
//
// Shake128 Xof is used for expanding 32 -bytes seed to matrix over R_q^(k x l).
//
// See `Expanding the Matrix A` point in section 5.3 of Dilithium specification,
// https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf
template<size_t k, size_t l>
static inline constexpr void
expand_a(std::span<const uint8_t, 32> rho, std::span<field::zq_t, k * l * ntt::N> mat)
{
  std::array<uint8_t, rho.size() + 2> msg{};
  auto _msg = std::span(msg);

  std::memcpy(_msg.template subspan<0, rho.size()>().data(), rho.data(), rho.size());

  for (size_t i = 0; i < k; i++) {
    for (size_t j = 0; j < l; j++) {
      const size_t off = (i * l + j) * ntt::N;
      const uint16_t nonce = static_cast<uint16_t>(i * 256ul + j);

      msg[32] = static_cast<uint8_t>(nonce >> 0);
      msg[33] = static_cast<uint8_t>(nonce >> 8);

      shake128::shake128_t hasher;
      hasher.absorb(_msg);
      hasher.finalize();

      std::array<uint8_t, shake128::RATE / 8> buf{};
      auto _buf = std::span(buf);

      size_t n = 0;
      while (n < ntt::N) {
        hasher.squeeze(_buf);

        for (size_t boff = 0; (boff < _buf.size()) && (n < ntt::N); boff += 3) {
          const uint32_t t0 = static_cast<uint32_t>(_buf[boff + 2] & 0b01111111);
          const uint32_t t1 = static_cast<uint32_t>(_buf[boff + 1]);
          const uint32_t t2 = static_cast<uint32_t>(_buf[boff + 0]);

          const uint32_t t3 = (t0 << 16) ^ (t1 << 8) ^ (t2 << 0);
          if (t3 < field::Q) {
            mat[off + n] = field::zq_t(t3);
            n++;
          }
        }
      }
    }
  }
}

// Uniform rejection sampling k -many degree-255 polynomials s.t. each coefficient of
// those polynomials ∈ [-η, η].
//
// Sampling is performed deterministically, by seeding Shake256 Xof with
// 64 -bytes seed and two byte nonce, whose starting value is provided ( see
// template parameter ). Consecutive nonces are computed by adding 1 to previous
// one.
//
// Note, sampled polynomial coefficients are kept in canonical form.
//
// See `Sampling the vectors s1 and s2` point in section 5.3 of Dilithium
// specification https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf
template<uint32_t η, size_t k, uint16_t nonce>
static inline constexpr void
expand_s(std::span<const uint8_t, 64> rho_prime, std::span<field::zq_t, k * ntt::N> vec)
  requires(dilithium_params::check_η(η) && dilithium_params::check_nonce(nonce))
{
  constexpr auto eta_ = field::zq_t(η);

  std::array<uint8_t, rho_prime.size() + 2> msg{};
  auto _msg = std::span(msg);

  std::memcpy(_msg.template subspan<0, rho_prime.size()>().data(), rho_prime.data(), rho_prime.size());

  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ntt::N;
    const uint16_t nonce_ = nonce + static_cast<uint16_t>(i);

    msg[64] = static_cast<uint8_t>(nonce_ >> 0);
    msg[65] = static_cast<uint8_t>(nonce_ >> 8);

    shake256::shake256_t hasher;
    hasher.absorb(_msg);
    hasher.finalize();

    std::array<uint8_t, shake256::RATE / 8> buf{};
    auto _buf = std::span(buf);

    size_t n = 0;
    while (n < ntt::N) {
      hasher.squeeze(_buf);

      for (size_t boff = 0; (boff < _buf.size()) && (n < ntt::N); boff++) {
        const uint8_t t0 = _buf[boff] & 0x0f;
        const uint8_t t1 = _buf[boff] >> 4;

        if constexpr (η == 2u) {
          const uint32_t t2 = static_cast<uint32_t>(t0 % 5);
          const bool flg0 = t0 < 15;

          vec[off + n] = eta_ - field::zq_t(t2);
          n += flg0 * 1;

          const uint32_t t3 = static_cast<uint32_t>(t1 % 5);
          const bool flg1 = (t1 < 15) & (n < ntt::N);
          const field::zq_t br[]{ vec[off], eta_ - field::zq_t(t3) };

          vec[off + flg1 * n] = br[flg1];
          n += flg1 * 1;
        } else {
          const bool flg0 = t0 < 9;

          vec[off + n] = eta_ - field::zq_t(static_cast<uint32_t>(t0));
          n += flg0 * 1;

          const bool flg1 = (t1 < 9) & (n < ntt::N);
          const auto t2 = eta_ - field::zq_t(static_cast<uint32_t>(t1));
          const field::zq_t br[]{ vec[off], t2 };

          vec[off + flg1 * n] = br[flg1];
          n += flg1 * 1;
        }
      }
    }
  }
}

// Given a 64 -bytes seed and 2 -bytes nonce, this routine does uniform sampling
// from output of Shake256 Xof, computing a l x 1 vector of degree-255
// polynomials s.t. each coefficient ∈ [-(γ1-1), γ1]
//
// See `Sampling the vectors y` point in section 5.3 of Dilithium
// specification
// https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf
template<uint32_t γ1, size_t l>
static inline constexpr void
expand_mask(std::span<const uint8_t, 64> seed, const uint16_t nonce, std::span<field::zq_t, l * ntt::N> vec)
  requires(dilithium_params::check_γ1(γ1))
{
  constexpr size_t gbw = std::bit_width(2 * γ1 - 1u);

  std::array<uint8_t, seed.size() + 2> msg{};
  std::array<uint8_t, ntt::N * gbw / 8> buf{};

  auto _msg = std::span(msg);
  auto _buf = std::span(buf);

  std::memcpy(_msg.template subspan<0, seed.size()>().data(), seed.data(), seed.size());

  for (size_t i = 0; i < l; i++) {
    const size_t off = i * ntt::N;
    const uint16_t nonce_ = nonce + static_cast<uint16_t>(i);

    msg[64] = static_cast<uint8_t>(nonce_ >> 0);
    msg[65] = static_cast<uint8_t>(nonce_ >> 8);

    shake256::shake256_t hasher;
    hasher.absorb(_msg);
    hasher.finalize();
    hasher.squeeze(_buf);

    bit_packing::decode<gbw>(buf, poly_t(vec.subspan(off, ntt::N)));
    poly::sub_from_x<γ1>(poly_t(vec.subspan(off, ntt::N)));
  }
}

// Given a 32 -bytes seed, this routine creates a degree-255 polynomial with τ
// -many coefficients set to +/- 1, while remaining (256 - τ) -many set to 0.
//
// See hashing to a ball algorithm described in figure 2 and section 5.3 of
// Dilithium specification
// https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf
template<uint32_t τ>
static inline constexpr void
sample_in_ball(std::span<const uint8_t, 32> seed, std::span<field::zq_t, ntt::N> poly)
  requires(dilithium_params::check_τ(τ))
{
  std::array<uint8_t, 8> tau_bits{};
  std::array<uint8_t, shake256::RATE / 8> buf{};

  auto _tau_bits = std::span(tau_bits);
  auto _buf = std::span(buf);

  shake256::shake256_t hasher;
  hasher.absorb(seed);
  hasher.finalize();
  hasher.squeeze(_tau_bits);

  constexpr size_t frm = ntt::N - τ;
  size_t i = frm;

  while (i < ntt::N) {
    hasher.squeeze(_buf);

    for (size_t off = 0; (off < _buf.size()) && (i < ntt::N); off++) {
      const size_t tau_bit = i - frm;

      const size_t tau_byte_off = tau_bit >> 3;
      const size_t tau_bit_off = tau_bit & 7ul;

      const uint8_t s = (_tau_bits[tau_byte_off] >> tau_bit_off) & 0b1;
      const bool s_ = static_cast<bool>(s);

      const auto tmp = _buf[off];
      const bool flg = tmp <= static_cast<uint8_t>(i);

      const field::zq_t br0[]{ poly[i], poly[tmp] };
      const field::zq_t br1[]{ poly[tmp], field::zq_t::one() - field::zq_t(2u * s_) };

      poly[i] = br0[flg];
      poly[tmp] = br1[flg];

      i += 1ul * flg;
    }
  };
}

}
