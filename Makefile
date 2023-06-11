CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -pedantic
OPTFLAGS = -O3 -march=native -mtune=native
IFLAGS = -I ./include
DEPFLAGS = -I ./sha3/include

all: test

tests/a.out: tests/main.cpp include/*.hpp include/tests/*.hpp sha3/include/*.hpp
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) $(IFLAGS) $(DEPFLAGS) $< -o $@

test: tests/a.out
	./$<

clean:
	find . -name '*.out' -o -name '*.o' -o -name '*.so' -o -name '*.gch' | xargs rm -rf

format:
	find . -name '*.cpp' -o -name '*.hpp' | xargs clang-format -i --style=Mozilla

benchmarks/a.out: benchmarks/main.cpp include/*.hpp include/benchmarks/*.hpp sha3/include/*.hpp
	# make sure you've google-benchmark globally installed;
	# see https://github.com/google/benchmark/tree/60b16f1#installation
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) $(IFLAGS) $(DEPFLAGS) $< -lbenchmark -o $@

benchmark: benchmarks/a.out
	# Ensure you've CPU frequency scaling disabled when running benchmarks, 
	# follow https://github.com/google/benchmark/blob/b177433f3ee2513b1075140c723d73ab8901790f/docs/reducing_variance.md
	#
	# No repeatition, showing mean time taken
	./$< --benchmark_time_unit=us --benchmark_counters_tabular=true
	# N(>0) repeatitions, showing only aggregates
	./$< --benchmark_time_unit=us --benchmark_repetitions=32 --benchmark_counters_tabular=true --benchmark_display_aggregates_only=true
