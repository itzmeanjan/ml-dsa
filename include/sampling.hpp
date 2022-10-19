#pragma once
#include "ff.hpp"
#include "ntt.hpp"
#include "shake128.hpp"
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
  uint8_t msg[34];
  uint8_t buf[3];

  std::memcpy(msg, rho, 32);

  for (size_t i = 0; i < k; i++) {
    for (size_t j = 0; j < l; j++) {
      const size_t off = (i * k + j) * ntt::N;

      msg[32] = j;
      msg[33] = i;

      shake128::shake128 hasher{};
      hasher.hash(msg, sizeof(msg));

      size_t n = 0;
      while (n < ntt::N) {
        hasher.read(buf, sizeof(buf));

        const uint32_t t0 = static_cast<uint32_t>(buf[2] & 0b01111111);
        const uint32_t t1 = static_cast<uint32_t>(buf[1]);
        const uint32_t t2 = static_cast<uint32_t>(buf[0]);

        const uint32_t t3 = (t0 << 16) ^ (t1 << 8) ^ (t0 << 0);
        const bool flg = t3 < ff::Q;

        mat[off + n] = ff::ff_t{ static_cast<uint32_t>(t3 * flg) };
        n = n + flg * 1;
      }
    }
  }
}

}
