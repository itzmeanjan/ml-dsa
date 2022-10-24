#pragma once
#include "ntt.hpp"
#include <cstring>

// Utility functions for Dilithium Post-Quantum Digital Signature Algorithm
namespace dilithium_utils {

// Given a degree-255 polynomial, where significant portion of each ( total 256
// of them ) coefficient ∈ [0, 2^sbw), this routine serializes the polynomial to
// a byte array of length 32 * sbw -bytes
//
// See section 5.2 ( which describes bit packing ) of Dilithium specification,
// as submitted to NIST final round call
// https://csrc.nist.gov/CSRC/media/Projects/post-quantum-cryptography/documents/round-3/submissions/Dilithium-Round3.zip
template<const size_t sbw>
static void
encode(const ff::ff_t* const __restrict poly, uint8_t* const __restrict arr)
{
  constexpr size_t blen = ntt::N * sbw;
  constexpr size_t len = blen >> 3;

  std::memset(arr, 0, len);

  for (size_t i = 0; i < blen; i++) {
    const size_t pidx = i / sbw;
    const size_t poff = i % sbw;

    const size_t aidx = i >> 3;
    const size_t aoff = i & 7ul;

    const uint8_t bit = static_cast<uint8_t>((poly[pidx].v >> poff) & 0b1);
    arr[aidx] = arr[aidx] ^ (bit << aoff);
  }
}

// Given a byte array of length 32 * sbw -bytes, this routine attempts to
// extract out 256 coefficients of degree-255 polynomial s.t. significant
// portion of each coefficient ∈ [0, 2^sbw)
template<const size_t sbw>
static void
decode(const uint8_t* const __restrict arr, ff::ff_t* const __restrict poly)
{
  constexpr size_t blen = ntt::N * sbw;

  std::memset(poly, 0, ntt::N * sizeof(ff::ff_t));

  for (size_t i = 0; i < blen; i++) {
    const size_t aidx = i >> 3;
    const size_t aoff = i & 7ul;

    const size_t pidx = i / sbw;
    const size_t poff = i % sbw;

    const uint8_t bit = (arr[aidx] >> aoff) & 0b1;
    poly[pidx].v = poly[pidx].v ^ static_cast<uint32_t>(bit) << poff;
  }
}

// Given a vector of hint bits ( of dimension k x 1 ), this routine encodes hint
// bits into (ω + k) -bytes, following the description in section 5.4 of
// Dilithium specification
// https://csrc.nist.gov/CSRC/media/Projects/post-quantum-cryptography/documents/round-3/submissions/Dilithium-Round3.zip
template<const size_t k, const size_t omega>
static void
encode_hint_bits(const ff::ff_t* const __restrict h,
                 uint8_t* const __restrict arr)
{
  constexpr size_t len = omega + k;
  constexpr ff::ff_t zero{ 0u };

  std::memset(arr, 0, len);

  size_t idx = 0;
  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ntt::N;

    for (size_t j = 0; j < ntt::N; j++) {
      const bool flg = h[off + j] != zero;

      arr[idx] = j * flg;
      idx = idx + 1 * flg;
    }

    arr[omega + i] = idx;
  }
}

}
