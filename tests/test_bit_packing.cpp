#include "bit_packing.hpp"
#include "field.hpp"
#include <cstdint>
#include <gtest/gtest.h>
#include <vector>

// Check for functional correctness of
//
// - polynomial to byte array encoding
// - decoding of polynomial from byte array
template<size_t sbw>
void
test_encode_decode()
  requires(dilithium_params::check_sbw(sbw))
{
  // Encoded byte length of the polynomial
  constexpr size_t enc_len = (sbw * ntt::N) / 8;

  std::vector<field::zq_t> polya(ntt::N, 0);
  std::vector<field::zq_t> polyb(ntt::N, 0);
  std::vector<uint8_t> arr(enc_len, 0);

  auto _polya = std::span<field::zq_t, ntt::N>(polya);
  auto _polyb = std::span<field::zq_t, ntt::N>(polyb);
  auto _arr = std::span<uint8_t, enc_len>(arr);

  prng::prng_t prng;

  for (size_t i = 0; i < ntt::N; i++) {
    _polya[i] = field::zq_t::random(prng);
  }

  bit_packing::encode<sbw>(_polya, _arr);
  bit_packing::decode<sbw>(_arr, _polyb);

  constexpr size_t mask = (1u << sbw) - 1u;
  bool flg = false;

  for (size_t i = 0; i < ntt::N; i++) {
    flg |= static_cast<bool>((_polya[i].raw() & mask) ^ _polyb[i].raw());
  }

  EXPECT_FALSE(flg);
}

TEST(Dilithium, PolynomialEncodingDecoding)
{
  test_encode_decode<3>();
  test_encode_decode<4>();
  test_encode_decode<6>();
  test_encode_decode<10>();
  test_encode_decode<13>();
  test_encode_decode<18>();
  test_encode_decode<20>();
}

// Generates random hint bit polynomial vector of dimension k x 1, with <= ω
// coefficients set to 1.
template<size_t k, size_t ω>
void
generate_random_hint_bits(std::span<field::zq_t, k * ntt::N> poly)
{
  std::fill(poly.begin(), poly.end(), field::zq_t::zero());

  constexpr size_t frm = 0;
  constexpr size_t to = poly.size() - 1;

  std::random_device rd;
  std::mt19937_64 gen(rd());
  std::uniform_int_distribution<size_t> dis{ frm, to };

  for (size_t i = 0; i < ω; i++) {
    const size_t idx = dis(gen);
    poly[idx] = field::zq_t::one();
  }
}

// Test functional correctness of encoding and decoding of hint bit polynomial
// vector.
template<size_t k, size_t ω>
void
test_encode_decode_hint_bits()
{
  // Encoded byte length of the hint polynomial
  constexpr size_t enc_len = ω + k;

  std::vector<field::zq_t> h0(k * ntt::N, 0);
  std::vector<field::zq_t> h1(k * ntt::N, 0);
  std::vector<field::zq_t> h2(k * ntt::N, 0);
  std::vector<uint8_t> arr0(enc_len, 0);
  std::vector<uint8_t> arr1(enc_len, 0);

  auto _h0 = std::span<field::zq_t, k * ntt::N>(h0);
  auto _h1 = std::span<field::zq_t, k * ntt::N>(h1);
  auto _h2 = std::span<field::zq_t, k * ntt::N>(h2);
  auto _arr0 = std::span<uint8_t, enc_len>(arr0);
  auto _arr1 = std::span<uint8_t, enc_len>(arr1);

  generate_random_hint_bits<k, ω>(_h0);

  bit_packing::encode_hint_bits<k, ω>(_h0, _arr0);
  std::copy(_arr0.begin(), _arr0.end(), _arr1.begin());
  _arr1[enc_len - 1] = ~_arr1[enc_len - 1];

  const bool failed0 = bit_packing::decode_hint_bits<k, ω>(_arr0, _h1);
  const bool failed1 = bit_packing::decode_hint_bits<k, ω>(_arr1, _h2);

  bool flg = true;

  for (size_t i = 0; i < k * ntt::N; i++) {
    flg &= (_h0[i] == _h1[i]);
  }

  EXPECT_TRUE(flg & !failed0 & failed1);
}

TEST(Dilithium, HintBitPolynomialEncodingDecoding)
{
  test_encode_decode_hint_bits<4, 80>();
  test_encode_decode_hint_bits<6, 55>();
  test_encode_decode_hint_bits<8, 75>();
}
