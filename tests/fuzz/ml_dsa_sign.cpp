#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

#if defined(ML_DSA_44)
#include "ml_dsa/ml_dsa_44.hpp"
namespace md = ml_dsa_44;
#elif defined(ML_DSA_65)
#include "ml_dsa/ml_dsa_65.hpp"
namespace md = ml_dsa_65;
#elif defined(ML_DSA_87)
#include "ml_dsa/ml_dsa_87.hpp"
namespace md = ml_dsa_87;
#else
#error "ML-DSA security level not defined"
#endif

extern "C" int
LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
  constexpr size_t OFF_DISCRIMINATOR = 0;

  // Malformed Input Mode (Mode A)
  constexpr size_t OFF_MALFORM_RND = OFF_DISCRIMINATOR + 1;
  constexpr size_t OFF_MALFORM_SKEY = OFF_MALFORM_RND + md::SigningSeedByteLen;
  constexpr size_t OFF_MALFORM_M_PRIME = OFF_MALFORM_SKEY + md::SecKeyByteLen;
  constexpr size_t OFF_MALFORM_END = OFF_MALFORM_M_PRIME + md::MessageRepresentativeByteLen;

  // Logic Discovery Mode (Mode B)
  constexpr size_t OFF_LOGIC_SEED = OFF_DISCRIMINATOR + 1;
  constexpr size_t OFF_LOGIC_RND = OFF_LOGIC_SEED + md::KeygenSeedByteLen;
  constexpr size_t OFF_LOGIC_M_PRIME = OFF_LOGIC_RND + md::SigningSeedByteLen;
  constexpr size_t OFF_LOGIC_END = OFF_LOGIC_M_PRIME + md::MessageRepresentativeByteLen;

  if (size < OFF_DISCRIMINATOR + 1) {
    return -1;
  }

  const bool is_mode_a = (data[OFF_DISCRIMINATOR] % 2 == 1);
  const size_t required_min = is_mode_a ? OFF_MALFORM_END : OFF_LOGIC_END;

  if (size < required_min) {
    return -1;
  }

  std::array<uint8_t, md::SigningSeedByteLen> rnd{};
  std::array<uint8_t, md::SecKeyByteLen> skey{};
  std::array<uint8_t, md::MessageRepresentativeByteLen> m_prime{};
  std::array<uint8_t, md::SigByteLen> sig{};

  if (is_mode_a) {
    // Mode A: Possibly malformed input mode
    std::copy(data + OFF_MALFORM_RND, data + OFF_MALFORM_SKEY, rnd.begin());
    std::copy(data + OFF_MALFORM_SKEY, data + OFF_MALFORM_M_PRIME, skey.begin());
    std::copy(data + OFF_MALFORM_M_PRIME, data + OFF_MALFORM_END, m_prime.begin());
  } else {
    // Mode B: Valid input mode
    std::array<uint8_t, md::KeygenSeedByteLen> seed{};
    std::array<uint8_t, md::PubKeyByteLen> pk{};

    std::copy(data + OFF_LOGIC_SEED, data + OFF_LOGIC_RND, seed.begin());
    std::copy(data + OFF_LOGIC_RND, data + OFF_LOGIC_M_PRIME, rnd.begin());
    std::copy(data + OFF_LOGIC_M_PRIME, data + OFF_LOGIC_END, m_prime.begin());

    md::keygen(seed, pk, skey);
  }

  (void)md::sign_internal(rnd, skey, m_prime, sig);
  return 0;
}
