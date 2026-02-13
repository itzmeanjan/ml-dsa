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
  constexpr size_t OFF_SEED = 0;
  constexpr size_t TOTAL_REQUIRED_NUM_BYTES = OFF_SEED + md::KeygenSeedByteLen;

  if (size < TOTAL_REQUIRED_NUM_BYTES) {
    return -1;
  }

  std::array<uint8_t, md::KeygenSeedByteLen> seed{};
  std::array<uint8_t, md::PubKeyByteLen> pk{};
  std::array<uint8_t, md::SecKeyByteLen> sk{};

  std::copy(data + OFF_SEED, data + TOTAL_REQUIRED_NUM_BYTES, seed.begin());

  md::keygen(seed, pk, sk);
  return 0;
}
