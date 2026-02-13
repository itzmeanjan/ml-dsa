# ML-DSA Example

A standalone example project demonstrating how to use the [ml-dsa](https://github.com/itzmeanjan/ml-dsa) library.

## Quick Start

```bash
cd examples

# Configure and build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Run
./build/ml_dsa_44
```

## Using `ml-dsa` in Your Own Project

The simplest way to integrate `ml-dsa` into your CMake project is via `FetchContent`:

```cmake
cmake_minimum_required(VERSION 3.30)
project(my_app LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(
  ml-dsa
  GIT_REPOSITORY https://github.com/itzmeanjan/ml-dsa.git
  GIT_TAG master
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(ml-dsa)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE ml-dsa)
target_compile_features(my_app PRIVATE cxx_std_20)
```

Alternatively, install `ml-dsa` system-wide and use `find_package`:

```bash
# From the ml-dsa root directory
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build build
sudo cmake --install build
```

```cmake
find_package(ml-dsa REQUIRED)
add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE ml-dsa)
```
