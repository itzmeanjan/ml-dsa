cmake_minimum_required(VERSION 3.28)

project(ml-dsa VERSION 0.1.0 LANGUAGES CXX)

# --- Options ---
option(ML_DSA_BUILD_TESTS "Build tests" OFF)
option(ML_DSA_BUILD_EXAMPLES "Build examples" OFF)
option(ML_DSA_BUILD_BENCHMARKS "Build benchmarks" OFF)
option(ML_DSA_BUILD_FUZZERS "Build fuzzers (requires clang)" OFF)
option(ML_DSA_FETCH_DEPS "Fetch missing dependencies (GTest, Benchmark)" OFF)

# --- Top-level-only settings (skipped when consumed via FetchContent/add_subdirectory) ---
if(PROJECT_IS_TOP_LEVEL)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

  option(ML_DSA_ENABLE_LTO "Enable Interprocedural Optimization (LTO)" ON)
  option(ML_DSA_NATIVE_OPT "Enable -march=native (not suitable for cross-compilation)" OFF)
  option(ML_DSA_ASAN "Enable AddressSanitizer" OFF)
  option(ML_DSA_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)

  # --- Tools ---
  find_program(CLANG_TIDY_EXE clang-tidy)
  find_program(CLANG_FORMAT_EXE clang-format)

  if(CLANG_TIDY_EXE)
    file(GLOB_RECURSE TIDY_SOURCES "examples/*.cpp")

    add_custom_target(tidy
      COMMAND ${CLANG_TIDY_EXE} -p ${CMAKE_BINARY_DIR} --config-file=${CMAKE_CURRENT_SOURCE_DIR}/.clang-tidy --header-filter=${CMAKE_CURRENT_SOURCE_DIR}/include/ml_dsa/.* ${TIDY_SOURCES}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      COMMENT "Running clang-tidy"
    )
  endif()

  if(CLANG_FORMAT_EXE)
    file(GLOB_RECURSE FORMAT_SOURCES
         "include/**/*.hpp"
         "examples/*.cpp"
         "benchmarks/*.hpp"
         "benchmarks/*.cpp"
         "tests/*.hpp"
         "tests/*.cpp")

    add_custom_target(format
      COMMAND ${CLANG_FORMAT_EXE} -i -style=file ${FORMAT_SOURCES}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      COMMENT "Running clang-format"
    )
  endif()

  # --- Standard settings ---
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)

  # --- Compiler Warnings ---
  if(MSVC)
    set(ML_DSA_WARNING_FLAGS /W4)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_WARNINGS)
  else()
    set(ML_DSA_WARNING_FLAGS -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wformat=2 -Wcast-qual -Wold-style-cast -Wundef -Werror)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
      list(APPEND ML_DSA_WARNING_FLAGS -Wno-pass-failed)
    endif()
  endif()

  # --- Sanitizers ---
  if(ML_DSA_ASAN)
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls)
    add_link_options(-fsanitize=address)
  endif()

  if(ML_DSA_UBSAN)
    add_compile_options(-fsanitize=undefined -fno-omit-frame-pointer -fno-optimize-sibling-calls -fno-sanitize-recover=undefined)
    add_link_options(-fsanitize=undefined)
  endif()

  # --- Native Optimization ---
  if(ML_DSA_NATIVE_OPT)
    add_compile_options(-march=native)
    message(STATUS "Enabled -march=native (machine-specific optimization)")
  endif()

  # --- LTO ---
  if(ML_DSA_ENABLE_LTO)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT result OUTPUT output)
    if(result)
      set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
      message(WARNING "LTO is not supported: ${output}")
    endif()
  endif()
endif()

# --- Dependencies ---
include(FetchContent)

# sha3 (cmake dependency)
FetchContent_Declare(
  sha3
  GIT_REPOSITORY https://github.com/itzmeanjan/sha3.git
  GIT_TAG master
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(sha3)

# randomshake (cmake dependency)
FetchContent_Declare(
  randomshake
  GIT_REPOSITORY https://github.com/itzmeanjan/RandomShake.git
  GIT_TAG main
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(randomshake)

# --- Library ---
add_library(ml-dsa INTERFACE)
target_include_directories(ml-dsa INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_link_libraries(ml-dsa INTERFACE sha3 randomshake)
target_compile_features(ml-dsa INTERFACE cxx_std_20)

# --- Tests ---
if(ML_DSA_BUILD_TESTS)
  enable_testing()
  if(ML_DSA_FETCH_DEPS)
    message(STATUS "Fetching GTest...")
    include(FetchContent)
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
      DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
  else()
    find_package(GTest REQUIRED)
  endif()

  file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
       "tests/kat/*.cpp"
       "tests/prop/*.cpp"
  )

  add_executable(ml_dsa_tests ${TEST_SOURCES})
  target_link_libraries(ml_dsa_tests PRIVATE ml-dsa GTest::gtest_main)
  target_include_directories(ml_dsa_tests PRIVATE tests)
  target_compile_options(ml_dsa_tests PRIVATE ${ML_DSA_WARNING_FLAGS})

  # Compile-time (constexpr) tests require higher evaluation limits for ML-DSA signing's rejection loop.
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set_source_files_properties(tests/prop/test_constexpr.cpp PROPERTIES COMPILE_OPTIONS "-fconstexpr-steps=4294967295")
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set_source_files_properties(tests/prop/test_constexpr.cpp PROPERTIES COMPILE_OPTIONS "-fconstexpr-ops-limit=4294967295")
  endif()

  include(GoogleTest)
  gtest_discover_tests(ml_dsa_tests)

  file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/kats" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
endif()

# --- Benchmarks ---
if(ML_DSA_BUILD_BENCHMARKS)
  if(ML_DSA_FETCH_DEPS)
    message(STATUS "Fetching Google Benchmark...")
    include(FetchContent)
    FetchContent_Declare(
      benchmark
      URL https://github.com/google/benchmark/archive/refs/tags/v1.9.5.zip
      DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
    set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(benchmark)
  else()
    find_package(benchmark REQUIRED)
  endif()

  file(GLOB BENCHMARK_SOURCES CONFIGURE_DEPENDS "benchmarks/*.cpp")
  find_library(LIBPFM pfm)

  add_executable(ml_dsa_benchmarks ${BENCHMARK_SOURCES})
  target_link_libraries(ml_dsa_benchmarks PRIVATE ml-dsa benchmark::benchmark_main)

  if(LIBPFM)
    target_link_libraries(ml_dsa_benchmarks PRIVATE ${LIBPFM})
    message(STATUS "Found libpfm: ${LIBPFM} - linking it to benchmarks")
  endif()

  target_include_directories(ml_dsa_benchmarks PRIVATE benchmarks)
  target_compile_options(ml_dsa_benchmarks PRIVATE ${ML_DSA_WARNING_FLAGS})
endif()

# --- Fuzzers ---
if(ML_DSA_BUILD_FUZZERS)
  if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(FATAL_ERROR "Fuzzers require Clang compiler")
  endif()

  set(FUZZ_FLAGS "-fsanitize=fuzzer,address,undefined" "-fno-sanitize-recover=undefined")

  set(VARIANTS 44 65 87)
  set(OPS keygen sign verify)

  foreach(variant ${VARIANTS})
    foreach(op ${OPS})
      set(fuzzer_target ml_dsa_${variant}_${op}_fuzzer)
      add_executable(${fuzzer_target} tests/fuzz/ml_dsa_${op}.cpp)
      target_compile_definitions(${fuzzer_target} PRIVATE ML_DSA_${variant})
      target_link_libraries(${fuzzer_target} PRIVATE ml-dsa)
      target_include_directories(${fuzzer_target} PRIVATE tests)
      target_compile_options(${fuzzer_target} PRIVATE ${ML_DSA_WARNING_FLAGS} ${FUZZ_FLAGS})
      target_link_options(${fuzzer_target} PRIVATE ${FUZZ_FLAGS})
    endforeach()
  endforeach()

  foreach(fuzzer field_arithmetic poly_ntt poly_sampling poly_bit_packing)
    set(fuzzer_target ${fuzzer}_fuzzer)
    add_executable(${fuzzer_target} tests/fuzz/${fuzzer}.cpp)
    target_link_libraries(${fuzzer_target} PRIVATE ml-dsa)
    target_include_directories(${fuzzer_target} PRIVATE tests)
    target_compile_options(${fuzzer_target} PRIVATE ${ML_DSA_WARNING_FLAGS} ${FUZZ_FLAGS})
    target_link_options(${fuzzer_target} PRIVATE ${FUZZ_FLAGS})
  endforeach()
endif()

# --- Examples ---
if(ML_DSA_BUILD_EXAMPLES)
  file(GLOB EXAMPLE_SOURCES CONFIGURE_DEPENDS "examples/*.cpp")
  foreach(ex_src ${EXAMPLE_SOURCES})
    get_filename_component(ex_name ${ex_src} NAME_WE)
    set(ex_target "${ex_name}_example")
    add_executable(${ex_target} ${ex_src})
    target_link_libraries(${ex_target} PRIVATE ml-dsa)
    target_compile_options(${ex_target} PRIVATE ${ML_DSA_WARNING_FLAGS})
  endforeach()
endif()

# --- Install ---
include(GNUInstallDirs)
install(TARGETS ml-dsa EXPORT ml-dsa-config INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT ml-dsa-config NAMESPACE ml-dsa:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ml-dsa)
