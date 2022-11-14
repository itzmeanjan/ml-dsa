#pragma once
#include "ntt.hpp"
#include <cstring>

// Utility functions for Dilithium Post-Quantum Digital Signature Algorithm
namespace dilithium_utils {

// Compile-time check to ensure that significant bit width of Z_q element
// doesn't cross maximum bit width of field prime q ( = 2^23 - 2^13 + 1 )
static inline constexpr bool
check_sbw(const size_t sbw)
{
  constexpr size_t mbw = std::bit_width(ff::Q - 1);
  return sbw <= mbw;
}

// Given a degree-255 polynomial, where significant portion of each ( total 256
// of them ) coefficient ∈ [0, 2^sbw), this routine serializes the polynomial to
// a byte array of length 32 * sbw -bytes
//
// See section 5.2 ( which describes bit packing ) of Dilithium specification
// https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf
template<const size_t sbw>
static void
encode(const ff::ff_t* const __restrict poly, uint8_t* const __restrict arr)
  requires(check_sbw(sbw))
{
  constexpr size_t blen = ntt::N * sbw;
  constexpr size_t len = blen >> 3;

  std::memset(arr, 0, len);

  if constexpr (sbw == 3) {
    constexpr size_t itr_cnt = ntt::N >> 3;
    constexpr uint32_t mask3 = 0b111u;
    constexpr uint32_t mask2 = 0b11u;
    constexpr uint32_t mask1 = 0b1u;

    for (size_t i = 0; i < itr_cnt; i++) {
      const size_t poff = i << 3;
      const size_t boff = i * 3;

      arr[boff + 0] = (static_cast<uint8_t>(poly[poff + 2].v & mask2) << 6) |
                      (static_cast<uint8_t>(poly[poff + 1].v & mask3) << 3) |
                      (static_cast<uint8_t>(poly[poff + 0].v & mask3) << 0);
      arr[boff + 1] = (static_cast<uint8_t>(poly[poff + 5].v & mask1) << 7) |
                      (static_cast<uint8_t>(poly[poff + 4].v & mask3) << 4) |
                      (static_cast<uint8_t>(poly[poff + 3].v & mask3) << 1) |
                      static_cast<uint8_t>((poly[poff + 2].v >> 2) & mask1);
      arr[boff + 2] = (static_cast<uint8_t>(poly[poff + 7].v & mask3) << 5) |
                      (static_cast<uint8_t>(poly[poff + 6].v & mask3) << 2) |
                      static_cast<uint8_t>((poly[poff + 5].v >> 1) & mask2);
    }
  } else if constexpr (sbw == 4) {
    constexpr size_t itr_cnt = ntt::N >> 1;
    constexpr uint32_t mask = 0b1111u;

    for (size_t i = 0; i < itr_cnt; i++) {
      const size_t off = i << 1;
      const uint8_t byte = (static_cast<uint8_t>(poly[off + 1].v & mask) << 4) |
                           (static_cast<uint8_t>(poly[off + 0].v & mask) << 0);

      arr[i] = byte;
    }
  } else if constexpr (sbw == 6) {
    constexpr size_t itr_cnt = ntt::N >> 2;
    constexpr uint32_t mask6 = 0b111111u;
    constexpr uint32_t mask4 = 0b1111u;
    constexpr uint32_t mask2 = 0b11u;

    for (size_t i = 0; i < itr_cnt; i++) {
      const size_t poff = i << 2;
      const size_t boff = i * 3;

      arr[boff + 0] = (static_cast<uint8_t>(poly[poff + 1].v & mask2) << 6) |
                      (static_cast<uint8_t>(poly[poff + 0].v & mask6) << 0);
      arr[boff + 1] = (static_cast<uint8_t>(poly[poff + 2].v & mask4) << 4) |
                      static_cast<uint8_t>((poly[poff + 1].v >> 2) & mask4);
      arr[boff + 2] = (static_cast<uint8_t>(poly[poff + 3].v & mask6) << 2) |
                      static_cast<uint8_t>((poly[poff + 2].v >> 4) & mask2);
    }
  } else if constexpr (sbw == 10) {
    constexpr size_t itr_cnt = ntt::N >> 2;
    constexpr uint32_t mask6 = 0b111111u;
    constexpr uint32_t mask4 = 0b1111u;
    constexpr uint32_t mask2 = 0b11u;

    for (size_t i = 0; i < itr_cnt; i++) {
      const size_t poff = i << 2;
      const size_t boff = i * 5;

      arr[boff + 0] = static_cast<uint8_t>(poly[poff + 0].v);
      arr[boff + 1] = static_cast<uint8_t>((poly[poff + 1].v & mask6) << 2) |
                      static_cast<uint8_t>((poly[poff + 0].v >> 8) & mask2);
      arr[boff + 2] = static_cast<uint8_t>((poly[poff + 2].v & mask4) << 4) |
                      static_cast<uint8_t>((poly[poff + 1].v >> 6) & mask4);
      arr[boff + 3] = static_cast<uint8_t>((poly[poff + 3].v & mask2) << 6) |
                      static_cast<uint8_t>((poly[poff + 2].v >> 4) & mask6);
      arr[boff + 4] = static_cast<uint8_t>(poly[poff + 3].v >> 2);
    }
  } else if constexpr (sbw == 13) {
    constexpr size_t itr_cnt = ntt::N >> 3;
    constexpr uint32_t mask7 = 0b1111111u;
    constexpr uint32_t mask6 = 0b111111u;
    constexpr uint32_t mask5 = 0b11111u;
    constexpr uint32_t mask4 = 0b1111u;
    constexpr uint32_t mask3 = 0b111u;
    constexpr uint32_t mask2 = 0b11u;
    constexpr uint32_t mask1 = 0b1u;

    for (size_t i = 0; i < itr_cnt; i++) {
      const size_t poff = i << 3;
      const size_t boff = i * 13;

      arr[boff + 0] = static_cast<uint8_t>(poly[poff + 0].v);
      arr[boff + 1] = static_cast<uint8_t>((poly[poff + 1].v & mask3) << 5) |
                      static_cast<uint8_t>((poly[poff + 0].v >> 8) & mask5);
      arr[boff + 2] = static_cast<uint8_t>(poly[poff + 1].v >> 3);
      arr[boff + 3] = static_cast<uint8_t>((poly[poff + 2].v & mask6) << 2) |
                      static_cast<uint8_t>((poly[poff + 1].v >> 11) & mask2);
      arr[boff + 4] = static_cast<uint8_t>((poly[poff + 3].v & mask1) << 7) |
                      static_cast<uint8_t>((poly[poff + 2].v >> 6) & mask7);
      arr[boff + 5] = static_cast<uint8_t>(poly[poff + 3].v >> 1);
      arr[boff + 6] = static_cast<uint8_t>((poly[poff + 4].v & mask4) << 4) |
                      static_cast<uint8_t>((poly[poff + 3].v >> 9) & mask4);
      arr[boff + 7] = static_cast<uint8_t>(poly[poff + 4].v >> 4);
      arr[boff + 8] = static_cast<uint8_t>((poly[poff + 5].v & mask7) << 1) |
                      static_cast<uint8_t>((poly[poff + 4].v >> 12) & mask1);
      arr[boff + 9] = static_cast<uint8_t>((poly[poff + 6].v & mask2) << 6) |
                      static_cast<uint8_t>((poly[poff + 5].v >> 7) & mask6);
      arr[boff + 10] = static_cast<uint8_t>(poly[poff + 6].v >> 2);
      arr[boff + 11] = static_cast<uint8_t>((poly[poff + 7].v & mask5) << 3) |
                       static_cast<uint8_t>((poly[poff + 6].v >> 10) & mask3);
      arr[boff + 12] = static_cast<uint8_t>(poly[poff + 7].v >> 5);
    }
  } else if constexpr (sbw == 18) {
    constexpr size_t itr_cnt = ntt::N >> 2;
    constexpr uint32_t mask6 = 0b111111u;
    constexpr uint32_t mask4 = 0b1111u;
    constexpr uint32_t mask2 = 0b11u;

    for (size_t i = 0; i < itr_cnt; i++) {
      const size_t poff = i << 2;
      const size_t boff = i * 9;

      arr[boff + 0] = static_cast<uint8_t>(poly[poff + 0].v);
      arr[boff + 1] = static_cast<uint8_t>(poly[poff + 0].v >> 8);
      arr[boff + 2] = static_cast<uint8_t>((poly[poff + 1].v & mask6) << 2) |
                      static_cast<uint8_t>((poly[poff + 0].v >> 16) & mask2);
      arr[boff + 3] = static_cast<uint8_t>(poly[poff + 1].v >> 6);
      arr[boff + 4] = static_cast<uint8_t>((poly[poff + 2].v & mask4) << 4) |
                      static_cast<uint8_t>((poly[poff + 1].v >> 14) & mask4);
      arr[boff + 5] = static_cast<uint8_t>(poly[poff + 2].v >> 4);
      arr[boff + 6] = static_cast<uint8_t>((poly[poff + 3].v & mask2) << 6) |
                      static_cast<uint8_t>((poly[poff + 2].v >> 12) & mask6);
      arr[boff + 7] = static_cast<uint8_t>(poly[poff + 3].v >> 2);
      arr[boff + 8] = static_cast<uint8_t>(poly[poff + 3].v >> 10);
    }
  } else if constexpr (sbw == 20) {
    constexpr size_t itr_cnt = ntt::N >> 1;
    constexpr uint32_t mask4 = 0b1111u;

    for (size_t i = 0; i < itr_cnt; i++) {
      const size_t poff = i << 1;
      const size_t boff = i * 5;

      arr[boff + 0] = static_cast<uint8_t>(poly[poff + 0].v);
      arr[boff + 1] = static_cast<uint8_t>(poly[poff + 0].v >> 8);
      arr[boff + 2] = static_cast<uint8_t>((poly[poff + 1].v & mask4) << 4) |
                      static_cast<uint8_t>((poly[poff + 0].v >> 16) & mask4);
      arr[boff + 3] = static_cast<uint8_t>(poly[poff + 1].v >> 4);
      arr[boff + 4] = static_cast<uint8_t>(poly[poff + 1].v >> 12);
    }
  } else {
    for (size_t i = 0; i < blen; i++) {
      const size_t pidx = i / sbw;
      const size_t poff = i % sbw;

      const size_t aidx = i >> 3;
      const size_t aoff = i & 7ul;

      const uint8_t bit = static_cast<uint8_t>((poly[pidx].v >> poff) & 0b1);
      arr[aidx] = arr[aidx] ^ (bit << aoff);
    }
  }
}

// Given a byte array of length 32 * sbw -bytes, this routine attempts to
// extract out 256 coefficients of degree-255 polynomial s.t. significant
// portion of each coefficient ∈ [0, 2^sbw)
//
// This is just the opposite of above `encode` routine. You may want to see
// Dilithium specification's section 5.2
// https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf
template<const size_t sbw>
static void
decode(const uint8_t* const __restrict arr, ff::ff_t* const __restrict poly)
  requires(check_sbw(sbw))
{
  constexpr size_t blen = ntt::N * sbw;

  std::memset(poly, 0, ntt::N * sizeof(ff::ff_t));

  if constexpr (sbw == 3) {
    constexpr size_t itr_cnt = ntt::N >> 3;
    constexpr uint8_t mask3 = 0b111;
    constexpr uint8_t mask2 = 0b11;
    constexpr uint8_t mask1 = 0b1;

    for (size_t i = 0; i < itr_cnt; i++) {
      const size_t poff = i << 3;
      const size_t boff = i * 3;

      poly[poff + 0].v = static_cast<uint32_t>((arr[boff + 0] >> 0) & mask3);
      poly[poff + 1].v = static_cast<uint32_t>((arr[boff + 0] >> 3) & mask3);
      poly[poff + 2].v = static_cast<uint32_t>((arr[boff + 1] & mask1) << 2) |
                         static_cast<uint32_t>(arr[boff + 0] >> 6);
      poly[poff + 3].v = static_cast<uint32_t>((arr[boff + 1] >> 1) & mask3);
      poly[poff + 4].v = static_cast<uint32_t>((arr[boff + 1] >> 4) & mask3);
      poly[poff + 5].v = static_cast<uint32_t>((arr[boff + 2] & mask2) << 1) |
                         static_cast<uint32_t>(arr[boff + 1] >> 7);
      poly[poff + 6].v = static_cast<uint32_t>((arr[boff + 2] >> 2) & mask3);
      poly[poff + 7].v = static_cast<uint32_t>(arr[boff + 2] >> 5);
    }
  } else if constexpr (sbw == 4) {
    constexpr size_t itr_cnt = ntt::N >> 1;
    constexpr uint8_t mask = 0b1111;

    for (size_t i = 0; i < itr_cnt; i++) {
      const size_t off = i << 1;
      const uint8_t byte = arr[i];

      poly[off + 0].v = static_cast<uint32_t>((byte >> 0) & mask);
      poly[off + 1].v = static_cast<uint32_t>((byte >> 4) & mask);
    }
  } else if constexpr (sbw == 6) {
    constexpr size_t itr_cnt = ntt::N >> 2;
    constexpr uint8_t mask6 = 0b111111;
    constexpr uint8_t mask4 = 0b1111;
    constexpr uint8_t mask2 = 0b11;

    for (size_t i = 0; i < itr_cnt; i++) {
      const size_t poff = i << 2;
      const size_t boff = i * 3;

      poly[poff + 0].v = static_cast<uint32_t>(arr[boff + 0] & mask6);
      poly[poff + 1].v = static_cast<uint32_t>((arr[boff + 1] & mask4) << 2) |
                         static_cast<uint32_t>(arr[boff + 0] >> 6);
      poly[poff + 2].v = static_cast<uint32_t>((arr[boff + 2] & mask2) << 4) |
                         static_cast<uint32_t>(arr[boff + 1] >> 4);
      poly[poff + 3].v = static_cast<uint32_t>(arr[boff + 2] >> 2);
    }
  } else if constexpr (sbw == 10) {
    constexpr size_t itr_cnt = ntt::N >> 2;
    constexpr uint8_t mask6 = 0b111111;
    constexpr uint8_t mask4 = 0b1111;
    constexpr uint8_t mask2 = 0b11;

    for (size_t i = 0; i < itr_cnt; i++) {
      const size_t poff = i << 2;
      const size_t boff = i * 5;

      poly[poff + 0].v = (static_cast<uint16_t>(arr[boff + 1] & mask2) << 8) |
                         static_cast<uint16_t>(arr[boff + 0]);
      poly[poff + 1].v = (static_cast<uint16_t>(arr[boff + 2] & mask4) << 6) |
                         static_cast<uint16_t>(arr[boff + 1] >> 2);
      poly[poff + 2].v = (static_cast<uint16_t>(arr[boff + 3] & mask6) << 4) |
                         static_cast<uint16_t>(arr[boff + 2] >> 4);
      poly[poff + 3].v = (static_cast<uint16_t>(arr[boff + 4]) << 2) |
                         static_cast<uint16_t>(arr[boff + 3] >> 6);
    }
  } else if constexpr (sbw == 13) {
    constexpr size_t itr_cnt = ntt::N >> 3;
    constexpr uint8_t mask7 = 0b1111111;
    constexpr uint8_t mask6 = 0b111111;
    constexpr uint8_t mask5 = 0b11111;
    constexpr uint8_t mask4 = 0b1111;
    constexpr uint8_t mask3 = 0b111;
    constexpr uint8_t mask2 = 0b11;
    constexpr uint8_t mask1 = 0b1;

    for (size_t i = 0; i < itr_cnt; i++) {
      const size_t poff = i << 3;
      const size_t boff = i * 13;

      poly[poff + 0] = (static_cast<uint32_t>(arr[boff + 1] & mask5) << 8) |
                       static_cast<uint32_t>(arr[boff + 0]);
      poly[poff + 1] = (static_cast<uint32_t>(arr[boff + 3] & mask2) << 11) |
                       (static_cast<uint32_t>(arr[boff + 2]) << 3) |
                       static_cast<uint32_t>(arr[boff + 1] >> 5);
      poly[poff + 2] = (static_cast<uint32_t>(arr[boff + 4] & mask7) << 6) |
                       static_cast<uint32_t>(arr[boff + 3] >> 2);
      poly[poff + 3] = (static_cast<uint32_t>(arr[boff + 6] & mask4) << 9) |
                       (static_cast<uint32_t>(arr[boff + 5]) << 1) |
                       static_cast<uint32_t>(arr[boff + 4] >> 7);
      poly[poff + 4] = (static_cast<uint32_t>(arr[boff + 8] & mask1) << 12) |
                       (static_cast<uint32_t>(arr[boff + 7]) << 4) |
                       static_cast<uint32_t>(arr[boff + 6] >> 4);
      poly[poff + 5] = (static_cast<uint32_t>(arr[boff + 9] & mask6) << 7) |
                       static_cast<uint32_t>(arr[boff + 8] >> 1);
      poly[poff + 6] = (static_cast<uint32_t>(arr[boff + 11] & mask3) << 10) |
                       (static_cast<uint32_t>(arr[boff + 10]) << 2) |
                       static_cast<uint32_t>(arr[boff + 9] >> 6);
      poly[poff + 7] = (static_cast<uint32_t>(arr[boff + 12]) << 5) |
                       static_cast<uint32_t>(arr[boff + 11] >> 3);
    }
  } else if constexpr (sbw == 18) {
    constexpr size_t itr_cnt = ntt::N >> 2;
    constexpr uint8_t mask6 = 0b111111;
    constexpr uint8_t mask4 = 0b1111;
    constexpr uint8_t mask2 = 0b11;

    for (size_t i = 0; i < itr_cnt; i++) {
      const size_t poff = i << 2;
      const size_t boff = i * 9;

      poly[poff + 0].v = (static_cast<uint32_t>(arr[boff + 2] & mask2) << 16) |
                         (static_cast<uint32_t>(arr[boff + 1]) << 8) |
                         static_cast<uint32_t>(arr[boff + 0]);
      poly[poff + 1].v = (static_cast<uint32_t>(arr[boff + 4] & mask4) << 14) |
                         (static_cast<uint32_t>(arr[boff + 3]) << 6) |
                         static_cast<uint32_t>(arr[boff + 2] >> 2);
      poly[poff + 2].v = (static_cast<uint32_t>(arr[boff + 6] & mask6) << 12) |
                         (static_cast<uint32_t>(arr[boff + 5]) << 4) |
                         static_cast<uint32_t>(arr[boff + 4] >> 4);
      poly[poff + 3].v = (static_cast<uint32_t>(arr[boff + 8]) << 10) |
                         (static_cast<uint32_t>(arr[boff + 7]) << 2) |
                         static_cast<uint32_t>(arr[boff + 6] >> 6);
    }
  } else if constexpr (sbw == 20) {
    constexpr size_t itr_cnt = ntt::N >> 1;
    constexpr uint8_t mask4 = 0b1111;

    for (size_t i = 0; i < itr_cnt; i++) {
      const size_t poff = i << 1;
      const size_t boff = i * 5;

      poly[poff + 0].v = (static_cast<uint32_t>(arr[boff + 2] & mask4) << 16) |
                         (static_cast<uint32_t>(arr[boff + 1]) << 8) |
                         static_cast<uint32_t>(arr[boff + 0]);
      poly[poff + 1].v = (static_cast<uint32_t>(arr[boff + 4]) << 12) |
                         (static_cast<uint32_t>(arr[boff + 3]) << 4) |
                         static_cast<uint32_t>(arr[boff + 2] >> 4);
    }
  } else {
    for (size_t i = 0; i < blen; i++) {
      const size_t aidx = i >> 3;
      const size_t aoff = i & 7ul;

      const size_t pidx = i / sbw;
      const size_t poff = i % sbw;

      const uint8_t bit = (arr[aidx] >> aoff) & 0b1;
      poly[pidx].v = poly[pidx].v ^ static_cast<uint32_t>(bit) << poff;
    }
  }
}

// Given a vector of hint bits ( of dimension k x 1 ), this routine encodes hint
// bits into (ω + k) -bytes, following the description in section 5.4 ( see
// point `Signature` on page 21 ) of Dilithium specification
// https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf
template<const size_t k, const size_t ω>
static void
encode_hint_bits(const ff::ff_t* const __restrict h,
                 uint8_t* const __restrict arr)
{
  constexpr size_t len = ω + k;
  constexpr ff::ff_t zero{ 0u };

  std::memset(arr, 0, len);

  size_t idx = 0;
  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ntt::N;

    for (size_t j = 0; j < ntt::N; j++) {
      const bool flg = h[off + j] != zero;
      const uint8_t br[]{ arr[idx], static_cast<uint8_t>(j) };

      arr[idx] = br[flg];
      idx += 1ul * flg;
    }

    arr[ω + i] = idx;
  }
}

// Given a serialized byte array holding hint bits, this routine unpacks hint
// bits into a vector ( of dimension k x 1 ) of degree-255 polynomials s.t. <= ω
// many hint bits are set.
//
// Returns boolean result denoting status of decoding of byte serialized hint
// bits. For example, say return value is true, it denotes that decoding has
// failed.
template<const size_t k, const size_t ω>
static bool
decode_hint_bits(const uint8_t* const __restrict arr,
                 ff::ff_t* const __restrict h)
{
  std::memset(h, 0, sizeof(ff::ff_t) * k * ntt::N);

  size_t idx = 0;
  bool failed = false;

  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ntt::N;

    const bool flg0 = arr[ω + i] < idx;
    const bool flg1 = arr[ω + i] > ω;
    const bool flg2 = flg0 | flg1;

    failed |= flg2;

    const size_t till = arr[ω + i];
    for (size_t j = idx; j < till; j++) {
      const bool flg0 = j > idx;
      const bool flg1 = flg0 & (arr[j] <= arr[j - flg0 * 1]);

      failed |= flg1;

      h[off + arr[j]] = ff::ff_t{ 1u };
    }

    idx = arr[ω + i];
  }

  for (size_t i = idx; i < ω; i++) {
    const bool flg = arr[i] != 0;
    failed |= flg;
  }

  return failed;
}

}
