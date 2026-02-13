cmake_minimum_required(VERSION 3.30)
project(ml-dsa-example LANGUAGES CXX)

# --- Fetch ml-dsa ---
# If ml-dsa is installed on the system, find_package will find it.
# Otherwise, fetch it from the parent directory (or a remote repository).
find_package(ml-dsa QUIET)

if(NOT ml-dsa_FOUND)
  message(STATUS "ml-dsa not found via find_package, using FetchContent...")
  include(FetchContent)
  FetchContent_Declare(
    ml-dsa
    # Option A: For local development, point to the parent directory:
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..
    # Option B: For production use, point to the GitHub repository:
    #
    # GIT_REPOSITORY https://github.com/itzmeanjan/ml-dsa.git
    # GIT_TAG master
    # GIT_SHALLOW TRUE
  )
  FetchContent_MakeAvailable(ml-dsa)
endif()

# --- Example Executable ---
add_executable(ml_dsa_44 ml_dsa_44.cpp)
target_link_libraries(ml_dsa_44 PRIVATE ml-dsa)
target_compile_features(ml_dsa_44 PRIVATE cxx_std_20)
