#pragma once
#include <cassert>
#include <charconv>
#include <cstdint>
#include <random>
#include <span>
#include <string_view>
#include <vector>

namespace ml_dsa_test_helper {

// Given a hex encoded string of length 2*L, this routine can be used for parsing it as a byte array of length L.
//
// Note, template parameter and return type. Compare it with next function definition.
template<size_t L>
static inline std::array<uint8_t, L>
from_hex(std::string_view bytes)
{
  const size_t blen = bytes.length();

  assert(blen % 2 == 0);
  assert(blen / 2 == L);

  std::array<uint8_t, L> res{};

  for (size_t i = 0; i < L; i++) {
    const size_t off = i * 2;

    uint8_t byte = 0;
    auto sstr = bytes.substr(off, 2);
    std::from_chars(sstr.data(), sstr.data() + 2, byte, 16);

    res[i] = byte;
  }

  return res;
}

// Given a hex encoded string of length 2*L, this routine can be used for parsing it as a byte array of length L.
//
// Note, no template type required and return type is different from above function definition.
static inline std::vector<uint8_t>
from_hex(std::string_view hex)
{
  const size_t hlen = hex.length();
  assert(hlen % 2 == 0);

  const size_t blen = hlen / 2;
  std::vector<uint8_t> res(blen, 0);

  for (size_t i = 0; i < blen; i++) {
    const size_t off = i * 2;

    uint8_t byte = 0;
    auto sstr = hex.substr(off, 2);
    std::from_chars(sstr.data(), sstr.data() + 2, byte, 16);

    res[i] = byte;
  }

  return res;
}

// Given a byte array, this routine randomly selects a bit and flips it. This routine is used for generating faulty data
// during testing.
//
// Collects inspiration from
// https://github.com/itzmeanjan/gift-cofb/blob/0bd9baa/wrapper/python/test_gift_cofb.py#L79-L101
static inline void
random_bit_flip(std::span<uint8_t> arr)
{
  std::random_device rd;
  std::mt19937_64 gen(rd());
  std::uniform_int_distribution<size_t> dis{ 0, arr.size() - 1 };

  const size_t idx = dis(gen);
  const size_t bidx = dis(gen) & 7ul;

  const uint8_t mask0 = 0xff << (bidx + 1);
  const uint8_t mask1 = 0xff >> (8 - bidx);
  const uint8_t mask2 = 1 << bidx;

  const uint8_t msb = arr[idx] & mask0;
  const uint8_t lsb = arr[idx] & mask1;
  const uint8_t bit = (arr[idx] & mask2) >> bidx;

  arr[idx] = msb | ((1 - bit) << bidx) | lsb;
}

}
