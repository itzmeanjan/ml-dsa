#pragma once
#include "ff.hpp"

// (inverse) Number Theoretic Transform for degree-255 polynomial, over
// Dilithium Prime Field Z_q | q = 2^23 - 2^13 + 1
namespace ntt {

constexpr size_t LOG2N = 8;
constexpr size_t N = 1 << LOG2N;

// First primitive 512 -th root of unity modulo q | q = 2^23 - 2^13 + 1
//
// Meaning, 1753 ** 512 == 1 mod q
constexpr ff::ff_t ζ{ 1753 };

// Multiplicative inverse of N over Z_q | q = 2^23 - 2^13 + 1, N = 256
constexpr auto INV_N = ff::ff_t{ N }.inv();

// Precomputed constants ( powers of ζ ), used when computing (i)NTT over
// degree-255 polynomial
constexpr ff::ff_t ζ_EXP[]{
  1,       4808194, 3765607, 3761513, 5178923, 5496691, 5234739, 5178987,
  7778734, 3542485, 2682288, 2129892, 3764867, 7375178, 557458,  7159240,
  5010068, 4317364, 2663378, 6705802, 4855975, 7946292, 676590,  7044481,
  5152541, 1714295, 2453983, 1460718, 7737789, 4795319, 2815639, 2283733,
  3602218, 3182878, 2740543, 4793971, 5269599, 2101410, 3704823, 1159875,
  394148,  928749,  1095468, 4874037, 2071829, 4361428, 3241972, 2156050,
  3415069, 1759347, 7562881, 4805951, 3756790, 6444618, 6663429, 4430364,
  5483103, 3192354, 556856,  3870317, 2917338, 1853806, 3345963, 1858416,
  3073009, 1277625, 5744944, 3852015, 4183372, 5157610, 5258977, 8106357,
  2508980, 2028118, 1937570, 4564692, 2811291, 5396636, 7270901, 4158088,
  1528066, 482649,  1148858, 5418153, 7814814, 169688,  2462444, 5046034,
  4213992, 4892034, 1987814, 5183169, 1736313, 235407,  5130263, 3258457,
  5801164, 1787943, 5989328, 6125690, 3482206, 4197502, 7080401, 6018354,
  7062739, 2461387, 3035980, 621164,  3901472, 7153756, 2925816, 3374250,
  1356448, 5604662, 2683270, 5601629, 4912752, 2312838, 7727142, 7921254,
  348812,  8052569, 1011223, 6026202, 4561790, 6458164, 6143691, 1744507,
  1753,    6444997, 5720892, 6924527, 2660408, 6600190, 8321269, 2772600,
  1182243, 87208,   636927,  4415111, 4423672, 6084020, 5095502, 4663471,
  8352605, 822541,  1009365, 5926272, 6400920, 1596822, 4423473, 4620952,
  6695264, 4969849, 2678278, 4611469, 4829411, 635956,  8129971, 5925040,
  4234153, 6607829, 2192938, 6653329, 2387513, 4768667, 8111961, 5199961,
  3747250, 2296099, 1239911, 4541938, 3195676, 2642980, 1254190, 8368000,
  2998219, 141835,  8291116, 2513018, 7025525, 613238,  7070156, 6161950,
  7921677, 6458423, 4040196, 4908348, 2039144, 6500539, 7561656, 6201452,
  6757063, 2105286, 6006015, 6346610, 586241,  7200804, 527981,  5637006,
  6903432, 1994046, 2491325, 6987258, 507927,  7192532, 7655613, 6545891,
  5346675, 8041997, 2647994, 3009748, 5767564, 4148469, 749577,  4357667,
  3980599, 2569011, 6764887, 1723229, 1665318, 2028038, 1163598, 5011144,
  3994671, 8368538, 7009900, 3020393, 3363542, 214880,  545376,  7609976,
  3105558, 7277073, 508145,  7826699, 860144,  3430436, 140244,  6866265,
  6195333, 3123762, 2358373, 6187330, 5365997, 6663603, 2926054, 7987710,
  8077412, 3531229, 4405932, 4606686, 1900052, 7598542, 1054478, 7648983
};

// Given a 64 -bit unsigned integer, this routine extracts specified many
// contiguous bits from ( least significant bits ) LSB side & reverses their bit
// order, returning bit reversed `mbw` -bit wide number
//
// See
// https://github.com/itzmeanjan/kyber/blob/3cd41a5/include/ntt.hpp#L74-L93
// for source of inspiration
template<const size_t mbw>
inline static constexpr size_t
bit_rev(const size_t v)
{
  size_t v_rev = 0ul;

  for (size_t i = 0; i < mbw; i++) {
    const size_t bit = (v >> i) & 0b1;
    v_rev ^= bit << (mbw - 1ul - i);
  }

  return v_rev;
}

// Given a polynomial f with 256 coefficients over Z_q | q = 2^23 - 2^13 + 1,
// this routine computes number theoretic transform using Cooley-Tukey
// algorithm, producing polynomial f' s.t. its coefficients are placed in
// bit-reversed order
//
// Note, this routine mutates input i.e. it's an in-place NTT implementation.
//
// Implementation inspired from
// https://github.com/itzmeanjan/kyber/blob/3cd41a5/include/ntt.hpp#L95-L129
static void
ntt(ff::ff_t* const __restrict poly)
{
  for (int64_t l = LOG2N - 1; l >= 0; l--) {
    const size_t len = 1ul << l;
    const size_t lenx2 = len << 1;
    const size_t k_beg = N >> (l + 1);

    for (size_t start = 0; start < N; start += lenx2) {
      const size_t k_now = k_beg + (start >> (l + 1));
      // Looking up precomputed constant, though it can be computed using
      //
      // ζ ^ bit_rev<LOG2N>(k_now)
      //
      // That's how above table is generated !
      const ff::ff_t ζ_exp = ζ_EXP[k_now];

      for (size_t i = start; i < start + len; i++) {
        auto tmp = ζ_exp * poly[i + len];

        poly[i + len] = poly[i] - tmp;
        poly[i] = poly[i] + tmp;
      }
    }
  }
}

// Given a polynomial f with 256 coefficients over Z_q | q = 2^23 - 2^13 + 1,
// s.t. its coefficients are placed in bit-reversed order, this routine computes
// inverse number theoretic transform using Gentleman-Sande algorithm, producing
// polynomial f' s.t. its coefficients are placed in standard order
//
// Note, this routine mutates input i.e. it's an in-place iNTT implementation.
//
// Implementation inspired from
// https://github.com/itzmeanjan/kyber/blob/3cd41a5/include/ntt.hpp#L131-L172
static void
intt(ff::ff_t* const __restrict poly)
{
  for (size_t l = 0; l < LOG2N; l++) {
    const size_t len = 1ul << l;
    const size_t lenx2 = len << 1;
    const size_t k_beg = (N >> l) - 1;

    for (size_t start = 0; start < N; start += lenx2) {
      const size_t k_now = k_beg - (start >> (l + 1));
      // Looking up precomputed constant, though it can be computed using
      //
      // -(ζ ^ bit_rev<LOG2N>(k_now))
      const ff::ff_t neg_ζ_exp = -ζ_EXP[k_now];

      for (size_t i = start; i < start + len; i++) {
        const auto tmp = poly[i];

        poly[i] = poly[i] + poly[i + len];
        poly[i + len] = tmp - poly[i + len];
        poly[i + len] = poly[i + len] * neg_ζ_exp;
      }
    }
  }

  for (size_t i = 0; i < N; i++) {
    poly[i] = poly[i] * INV_N;
  }
}

}
