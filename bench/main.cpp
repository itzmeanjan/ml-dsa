#include "bench_dilithium.hpp"

// register for benchmarking prime field arithmetic
BENCHMARK(bench_dilithium::ff_add);
BENCHMARK(bench_dilithium::ff_sub);
BENCHMARK(bench_dilithium::ff_neg);
BENCHMARK(bench_dilithium::ff_mul);
BENCHMARK(bench_dilithium::ff_inv);
BENCHMARK(bench_dilithium::ff_div);
BENCHMARK(bench_dilithium::ff_exp);

// register for benchmarking (i)NTT
BENCHMARK(bench_dilithium::ntt);
BENCHMARK(bench_dilithium::intt);

// register for benchmarking coefficient reduction techniques
BENCHMARK(bench_dilithium::power2round);
BENCHMARK(bench_dilithium::decompose<190464>);
BENCHMARK(bench_dilithium::make_hint<190464>);
BENCHMARK(bench_dilithium::use_hint<190464>);
BENCHMARK(bench_dilithium::decompose<523776>);
BENCHMARK(bench_dilithium::make_hint<523776>);
BENCHMARK(bench_dilithium::use_hint<523776>);

// register for benchmarking sampling vector/ matrix from XOF
BENCHMARK(bench_dilithium::expand_a<4, 4>);
BENCHMARK(bench_dilithium::expand_a<6, 5>);
BENCHMARK(bench_dilithium::expand_a<8, 7>);
BENCHMARK(bench_dilithium::uniform_sampling_eta<2, 4>);
BENCHMARK(bench_dilithium::uniform_sampling_eta<2, 8>);
BENCHMARK(bench_dilithium::uniform_sampling_eta<4, 6>);
BENCHMARK(bench_dilithium::expand_mask<1u << 17, 4>);
BENCHMARK(bench_dilithium::expand_mask<1u << 19, 5>);
BENCHMARK(bench_dilithium::expand_mask<1u << 19, 7>);
BENCHMARK(bench_dilithium::sample_in_ball<39>);
BENCHMARK(bench_dilithium::sample_in_ball<49>);
BENCHMARK(bench_dilithium::sample_in_ball<60>);

// register for benchmarking serialization/ deserialization of polynomials
BENCHMARK(bench_dilithium::encode<3>);
BENCHMARK(bench_dilithium::decode<3>);
BENCHMARK(bench_dilithium::encode<4>);
BENCHMARK(bench_dilithium::decode<4>);
BENCHMARK(bench_dilithium::encode<6>);
BENCHMARK(bench_dilithium::decode<6>);
BENCHMARK(bench_dilithium::encode<10>);
BENCHMARK(bench_dilithium::decode<10>);
BENCHMARK(bench_dilithium::encode<13>);
BENCHMARK(bench_dilithium::decode<13>);
BENCHMARK(bench_dilithium::encode<18>);
BENCHMARK(bench_dilithium::decode<18>);
BENCHMARK(bench_dilithium::encode<20>);
BENCHMARK(bench_dilithium::decode<20>);

// register for benchmarking serialization/ deserialization of hint bits
BENCHMARK(bench_dilithium::encode_hint_bits<4, 80>);
BENCHMARK(bench_dilithium::decode_hint_bits<4, 80>);
BENCHMARK(bench_dilithium::encode_hint_bits<6, 55>);
BENCHMARK(bench_dilithium::decode_hint_bits<6, 55>);
BENCHMARK(bench_dilithium::encode_hint_bits<8, 75>);
BENCHMARK(bench_dilithium::decode_hint_bits<8, 75>);

// register for benchmarking Dilithium Key Generation, Signing & Verification
BENCHMARK(bench_dilithium::keygen<4, 4, 13, 2>); // NIST security level 2
BENCHMARK(bench_dilithium::sign<4, 4, 13, 2, 1u << 17, 95232, 39, 78, 80>);
BENCHMARK(bench_dilithium::verify<4, 4, 13, 2, 1u << 17, 95232, 39, 78, 80>);
BENCHMARK(bench_dilithium::keygen<6, 5, 13, 4>); // NIST security level 3
BENCHMARK(bench_dilithium::sign<6, 5, 13, 4, 1u << 19, 261888, 49, 196, 55>);
BENCHMARK(bench_dilithium::verify<6, 5, 13, 4, 1u << 19, 261888, 49, 196, 55>);
BENCHMARK(bench_dilithium::keygen<8, 7, 13, 2>); // NIST security level 5
BENCHMARK(bench_dilithium::sign<8, 7, 13, 2, 1u << 19, 261888, 60, 120, 75>);
BENCHMARK(bench_dilithium::verify<8, 7, 13, 2, 1u << 19, 261888, 60, 120, 75>);

// benchmark runner main routine
BENCHMARK_MAIN();
