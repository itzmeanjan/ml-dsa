#pragma once
#include "ff.hpp"
#include "ntt.hpp"
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
// as submitted to NIST final round call
// https://csrc.nist.gov/CSRC/media/Projects/post-quantum-cryptography/documents/round-3/submissions/Dilithium-Round3.zip
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
check_eta(const uint32_t eta)
{
  return (eta == 2u) || (eta == 4u);
}

// Compile-time check to ensure that starting nonce belongs to allowed set of
// values when uniform sampling polynomial coefficients in [-η, η]
inline static constexpr bool
check_nonce(const size_t nonce)
{
  return (nonce == 0) || (nonce == 4) || (nonce == 5) || (nonce == 7);
}

// Uniform sampling k -many degree-255 polynomials s.t. each coefficient of
// those polynomials belong to [-η, η].
//
// Sampling is performed deterministically, by seeding SHAKE256 XOF with
// 32 -bytes seed and single byte nonce, whose starting value is provided ( see
// template parameter ). Consecutive nonces are computed by adding 1 to previous
// one.
//
// Rejection sampling logic is adapted from
// https://github.com/pq-crystals/dilithium/blob/3e9b9f1/ref/poly.c#L370-L417
//
// Note, sampled polynomial coefficients are kept in canonical form.
template<const uint32_t eta, const size_t k, const size_t nonce>
static void
uniform_sample_eta(const uint8_t* const __restrict sigma,
                   ff::ff_t* const __restrict vec) requires(check_eta(eta) &&
                                                            check_nonce(nonce))
{
  constexpr ff::ff_t eta_{ eta };

  uint8_t msg[34]{};
  uint8_t buf = 0;

  std::memcpy(msg, sigma, 32);

  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ntt::N;
    const uint16_t nonce_ = static_cast<uint16_t>(nonce + i);

    msg[32] = static_cast<uint8_t>(nonce_ >> 0);
    msg[33] = static_cast<uint8_t>(nonce_ >> 8);

    shake256::shake256 hasher{};
    hasher.hash(msg, sizeof(msg));

    size_t n = 0;
    while (n < ntt::N) {
      hasher.read(&buf, 1);

      const uint8_t t0 = buf & 0x0f;
      const uint8_t t1 = buf >> 4;

      if constexpr (eta == 2u) {
        if (t0 < 15) {
          const uint32_t t2 = static_cast<uint32_t>(t0 % 5);

          vec[off + n] = eta_ - ff::ff_t{ t2 };
          n += 1;
        }

        if ((t1 < 15) && (n < ntt::N)) {
          const uint32_t t2 = static_cast<uint32_t>(t1 % 5);

          vec[off + n] = eta_ - ff::ff_t{ t2 };
          n += 1;
        }
      } else {
        if (t0 < 9) {
          vec[off + n] = eta_ - ff::ff_t{ static_cast<uint32_t>(t0) };
          n += 1;
        }

        if ((t1 < 9) && (n < ntt::N)) {
          vec[off + n] = eta_ - ff::ff_t{ static_cast<uint32_t>(t1) };
          n += 1;
        }
      }
    }
  }
}

}
