#include "bench/bench_dilithium.hpp"

// register for benchmarking Dilithium Key Generation, Signing & Verification
using namespace bench_dilithium;

BENCHMARK(keygen<4, 4, 13, 2>)->Arg(32);
BENCHMARK(sign<4, 4, 13, 2, 131072, 95232, 39, 78, 80>)->Arg(32);
BENCHMARK(verify<4, 4, 13, 2, 131072, 95232, 39, 78, 80>)->Arg(32);
BENCHMARK(keygen<6, 5, 13, 4>)->Arg(32);
BENCHMARK(sign<6, 5, 13, 4, 524288, 261888, 49, 196, 55>)->Arg(32);
BENCHMARK(verify<6, 5, 13, 4, 524288, 261888, 49, 196, 55>)->Arg(32);
BENCHMARK(keygen<8, 7, 13, 2>)->Arg(32);
BENCHMARK(sign<8, 7, 13, 2, 524288, 261888, 60, 120, 75>)->Arg(32);
BENCHMARK(verify<8, 7, 13, 2, 524288, 261888, 60, 120, 75>)->Arg(32);

// benchmark runner main routine
BENCHMARK_MAIN();
