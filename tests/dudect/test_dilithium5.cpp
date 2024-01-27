#include "dilithium5.hpp"
#include <cstdio>

#define DUDECT_IMPLEMENTATION
#define DUDECT_VISIBLITY_STATIC
#include "dudect.h"

constexpr size_t SEED_LEN = 32; // Byte length of seed(s)

uint8_t
do_one_computation(uint8_t* const data)
{
  constexpr uint32_t α = dilithium5::γ2 << 1;
  constexpr uint32_t m = (field::Q - 1u) / α;
  constexpr size_t w1bw = std::bit_width(m - 1u);

  constexpr size_t doff0 = 0;
  constexpr size_t doff1 = doff0 + 2 * SEED_LEN;

  std::array<field::zq_t, dilithium5::l * ntt::N> vec{};
  std::array<field::zq_t, vec.size()> vec_high{};
  std::array<field::zq_t, vec.size()> vec_low{};
  std::array<field::zq_t, vec_high.size()> vec_hint{};
  std::array<uint8_t, (vec_high.size() * w1bw) / 8> encoded{};
  std::array<field::zq_t, vec_high.size()> decoded{};
  std::array<uint8_t, dilithium5::ω + dilithium5::l> encoded_hints{};
  std::array<field::zq_t, vec.size()> decoded_hints{};

  auto seed = std::span<const uint8_t, 2 * SEED_LEN>(data + doff0, doff1 - doff0);
  const uint16_t kappa = (static_cast<uint16_t>(data[doff1 + 1]) << 8) | (static_cast<uint16_t>(data[doff1 + 0]) << 0);

  uint8_t ret_val = 0;

  sampling::expand_mask<dilithium5::γ1, dilithium5::l>(seed, kappa, vec);
  ret_val ^= static_cast<uint8_t>(vec[0].raw() ^ vec[vec.size() - 1].raw());

  polyvec::ntt<dilithium5::l>(vec);
  ret_val ^= static_cast<uint8_t>(vec[0].raw() ^ vec[vec.size() - 1].raw());

  polyvec::intt<dilithium5::l>(vec);
  ret_val ^= static_cast<uint8_t>(vec[0].raw() ^ vec[vec.size() - 1].raw());

  polyvec::highbits<dilithium5::l, α>(vec, vec_high);
  ret_val ^= static_cast<uint8_t>(vec_high[0].raw() ^ vec_high[vec_high.size() - 1].raw());

  polyvec::lowbits<dilithium5::l, α>(vec, vec_low);
  ret_val ^= static_cast<uint8_t>(vec_low[0].raw() ^ vec_low[vec_low.size() - 1].raw());

  polyvec::encode<dilithium5::l, w1bw>(vec_high, encoded);
  ret_val ^= encoded[0] ^ encoded[encoded.size() - 1];

  polyvec::decode<dilithium5::l, w1bw>(encoded, decoded);
  ret_val ^= static_cast<uint8_t>(decoded[0].raw() ^ decoded[decoded.size() - 1].raw());

  const auto z_norm = polyvec::infinity_norm<dilithium5::l>(vec);
  ret_val ^= static_cast<uint8_t>(z_norm.raw());

  polyvec::make_hint<dilithium5::l, α>(vec, vec_high, vec_hint);
  ret_val ^= static_cast<uint8_t>(vec_high[0].raw() ^ vec_hint[vec_hint.size() - 1].raw());

  const auto count_1 = polyvec::count_1s<dilithium5::l>(vec_hint);
  ret_val ^= static_cast<uint8_t>(count_1);

  bit_packing::encode_hint_bits<dilithium5::l, dilithium5::ω>(vec_hint, encoded_hints);
  ret_val ^= encoded_hints[0] ^ encoded_hints[encoded_hints.size() - 1];

  bit_packing::decode_hint_bits<dilithium5::l, dilithium5::ω>(encoded_hints, decoded_hints);
  ret_val ^= static_cast<uint8_t>(decoded_hints[0].raw() ^ decoded_hints[decoded_hints.size() - 1].raw());

  return ret_val;
}

void
prepare_inputs(dudect_config_t* const c, uint8_t* const input_data, uint8_t* const classes)
{
  randombytes(input_data, c->number_measurements * c->chunk_size);

  for (size_t i = 0; i < c->number_measurements; i++) {
    classes[i] = randombit();
    if (classes[i] == 0) {
      std::memset(input_data + i * c->chunk_size, 0x00, c->chunk_size);
    }
  }
}

dudect_state_t
test_dilithium5()
{
  constexpr size_t chunk_size = 2 * SEED_LEN + 2 + SEED_LEN;
  constexpr size_t number_measurements = 1e5;

  dudect_config_t config = {
    chunk_size,
    number_measurements,
  };
  dudect_ctx_t ctx;
  dudect_init(&ctx, &config);

  dudect_state_t state = DUDECT_NO_LEAKAGE_EVIDENCE_YET;
  while (state == DUDECT_NO_LEAKAGE_EVIDENCE_YET) {
    state = dudect_main(&ctx);
  }

  dudect_free(&ctx);

  printf("Detected timing leakage in \"%s\", defined in file \"%s\"\n", __func__, __FILE_NAME__);
  return state;
}

int
main()
{
  if (test_dilithium5() != DUDECT_NO_LEAKAGE_EVIDENCE_YET) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
