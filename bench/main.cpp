#include "bench/bench_dilithium.hpp"

using namespace bench_dilithium;

// register for benchmarking Dilithium Key Generation, Signing & Verification

BENCHMARK(dilithium2_keygen)->UseManualTime();
BENCHMARK(dilithium2_sign)->Arg(32)->UseManualTime();
BENCHMARK(dilithium2_verify)->Arg(32)->UseManualTime();

BENCHMARK(dilithium3_keygen)->UseManualTime();
BENCHMARK(dilithium3_sign)->Arg(32)->UseManualTime();
BENCHMARK(dilithium3_verify)->Arg(32)->UseManualTime();

BENCHMARK(dilithium5_keygen)->UseManualTime();
BENCHMARK(dilithium5_sign)->Arg(32)->UseManualTime();
BENCHMARK(dilithium5_verify)->Arg(32)->UseManualTime();

// benchmark runner main routine
BENCHMARK_MAIN();
