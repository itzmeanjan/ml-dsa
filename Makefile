CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -pedantic
OPTFLAGS = -O3 -march=native -mtune=native
IFLAGS = -I ./include
DEPFLAGS = -I ./sha3/include

all: testing test_kat

wrapper/libdilithium.so: wrapper/dilithium.cpp include/*.hpp sha3/include/*.hpp
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) $(IFLAGS) $(DEPFLAGS) -fPIC --shared $< -o $@

lib: wrapper/libdilithium.so

test/a.out: test/main.cpp include/*.hpp
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) $(IFLAGS) $(DEPFLAGS) $< -o $@

testing: test/a.out
	./$<

test_kat:
	bash test_kat.sh

clean:
	find . -name '*.out' -o -name '*.o' -o -name '*.so' -o -name '*.gch' | xargs rm -rf

format:
	find . -name '*.cpp' -o -name '*.hpp' | xargs clang-format -i --style=Mozilla && python3 -m black wrapper/python/*.py

bench/a.out: bench/main.cpp include/*.hpp
	# make sure you've google-benchmark globally installed;
	# see https://github.com/google/benchmark/tree/60b16f1#installation
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) $(IFLAGS) $(DEPFLAGS) $< -lbenchmark -o $@

benchmark: bench/a.out
	# Ensure you've CPU frequency scaling disabled when running benchmarks, 
	# follow https://github.com/google/benchmark/blob/b177433f3ee2513b1075140c723d73ab8901790f/docs/reducing_variance.md
	#
	# No repeatition, showing mean time taken
	./$< --benchmark_time_unit=us --benchmark_counters_tabular=true
	# N(>0) repeatitions, showing only aggregates
	./$< --benchmark_time_unit=us --benchmark_repetitions=32 --benchmark_counters_tabular=true --benchmark_display_aggregates_only=true
