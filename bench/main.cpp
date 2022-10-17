#include "bench_dilithium.hpp"

// register for benchmarking prime field arithmetic
BENCHMARK(bench_dilithium::ff_add);

// benchmark runner main routine
BENCHMARK_MAIN();
