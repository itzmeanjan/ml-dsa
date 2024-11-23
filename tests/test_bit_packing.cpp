#include "ml_dsa/internals/poly/bit_packing.hpp"
#include "randomshake/randomshake.hpp"
#include <gtest/gtest.h>

// Check for functional correctness of
//
// - polynomial to byte array encoding
// - decoding of polynomial from byte array
template<size_t sbw>
static void
test_encode_decode()
  requires(ml_dsa_params::check_sbw(sbw))
{
  constexpr size_t poly_byte_len = (sbw * ml_dsa_ntt::N) / 8;
  constexpr size_t mask = (1u << sbw) - 1u;

  std::array<ml_dsa_field::zq_t, ml_dsa_ntt::N> polya{};
  std::array<ml_dsa_field::zq_t, ml_dsa_ntt::N> polyb{};
  std::array<uint8_t, poly_byte_len> poly_bytes{};

  randomshake::randomshake_t<256> csprng;

  for (size_t i = 0; i < ml_dsa_ntt::N; i++) {
    polya[i] = ml_dsa_field::zq_t::random(csprng);
  }

  ml_dsa_bit_packing::encode<sbw>(polya, poly_bytes);
  ml_dsa_bit_packing::decode<sbw>(poly_bytes, polyb);

  for (size_t i = 0; i < ml_dsa_ntt::N; i++) {
    EXPECT_EQ(polya[i].raw() & mask, polyb[i].raw());
  }
}

TEST(ML_DSA, PolynomialEncodingDecoding)
{
  test_encode_decode<3>();
  test_encode_decode<4>();
  test_encode_decode<6>();
  test_encode_decode<10>();
  test_encode_decode<13>();
  test_encode_decode<18>();
  test_encode_decode<20>();
}

// Generates random hint bit polynomial vector of dimension k x 1, with <= ω coefficients set to 1.
template<size_t k, size_t ω>
static void
generate_random_hint_bits(std::span<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> poly)
{
  std::fill(poly.begin(), poly.end(), ml_dsa_field::zq_t::zero());

  constexpr size_t frm = 0;
  constexpr size_t to = poly.size() - 1;

  std::random_device rd;
  std::mt19937_64 gen(rd());
  std::uniform_int_distribution<size_t> dis{ frm, to };

  for (size_t i = 0; i < ω; i++) {
    const size_t idx = dis(gen);
    poly[idx] = ml_dsa_field::zq_t::one();
  }
}

// Test functional correctness of encoding and decoding of hint bit polynomial vector.
template<size_t k, size_t ω>
static void
test_encode_decode_hint_bits()
{
  constexpr size_t hint_byte_len = ω + k;

  std::array<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> h0{};
  std::array<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> h1{};
  std::array<ml_dsa_field::zq_t, k * ml_dsa_ntt::N> h2{};

  std::array<uint8_t, hint_byte_len> hint_poly_bytes0{};
  std::array<uint8_t, hint_byte_len> hint_poly_bytes1{};

  generate_random_hint_bits<k, ω>(h0);

  ml_dsa_bit_packing::encode_hint_bits<k, ω>(h0, hint_poly_bytes0);

  std::copy(hint_poly_bytes0.begin(), hint_poly_bytes0.end(), hint_poly_bytes1.begin());
  hint_poly_bytes1[hint_byte_len - 1] = ~hint_poly_bytes1[hint_byte_len - 1];

  const bool failed0 = ml_dsa_bit_packing::decode_hint_bits<k, ω>(hint_poly_bytes0, h1);
  EXPECT_FALSE(failed0);
  EXPECT_TRUE(std::equal(h0.begin(), h0.end(), h1.begin()));

  const bool failed1 = ml_dsa_bit_packing::decode_hint_bits<k, ω>(hint_poly_bytes1, h2);
  EXPECT_TRUE(failed1);
  EXPECT_FALSE(std::equal(h0.begin(), h0.end(), h2.begin()));
}

TEST(ML_DSA, HintBitPolynomialEncodingDecoding)
{
  test_encode_decode_hint_bits<4, 80>();
  test_encode_decode_hint_bits<6, 55>();
  test_encode_decode_hint_bits<8, 75>();
}
