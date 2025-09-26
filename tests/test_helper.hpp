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
// This is supposed to be used with hex strings for which we know the size at program compile-time.
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
// This is supposed to be used with hex strings for which we don't know the size at compile-time. That is why
// it returns a dynamically allocated vector of bytes.
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

// Given a string of following format, this function can extract and parse the hex string portion,
// returning a byte array of requested length.
//
// DATA = 010203....0d0e0f
template<size_t byte_len>
static inline std::array<uint8_t, byte_len>
extract_and_parse_fixed_length_hex_string(std::string_view in_str)
{
  using namespace std::literals;

  const auto hex_str = in_str.substr(in_str.find("="sv) + 2, in_str.size());
  return from_hex<byte_len>(hex_str);
};

// Given a string of following format, this function can extract and parse the hex string portion,
// returning a byte array of requested length.
//
// DATA = 010203....0d0e0f
static inline std::vector<uint8_t>
extract_and_parse_variable_length_hex_string(std::string_view in_str)
{
  using namespace std::literals;

  const auto hex_str = in_str.substr(in_str.find("="sv) + 2, in_str.size());
  return from_hex(hex_str);
};

// Given a string of following format, this function can extract and parse the integer portion,
// returning an integer value.
//
// MSG_LENGTH = 33
static inline size_t
extract_and_parse_integer(std::string_view in_str)
{
  using namespace std::literals;

  size_t int_val = 0;

  auto int_val_str = in_str.substr(in_str.find("="sv) + 2, in_str.size());
  std::from_chars(int_val_str.data(), int_val_str.data() + int_val_str.size(), int_val);

  return int_val;
};

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
