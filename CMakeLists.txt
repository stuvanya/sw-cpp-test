cmake_minimum_required(VERSION 3.27)
project(sw_battle_test)

set(CMAKE_CXX_STANDARD 20)

# ---------------------------------------------------------------------------
# Production executable
# ---------------------------------------------------------------------------
file(GLOB_RECURSE SOURCES src/*.cpp src/*.hpp)
add_executable(sw_battle_test ${SOURCES})
target_include_directories(sw_battle_test PUBLIC src/)

# ---------------------------------------------------------------------------
# Test executable
# Includes all src sources EXCEPT src/main.cpp (which has its own main()),
# plus all files under tests/.
# ---------------------------------------------------------------------------
file(GLOB_RECURSE SRC_SOURCES src/*.cpp src/*.hpp)
list(FILTER SRC_SOURCES EXCLUDE REGEX ".*/main\\.cpp$")

file(GLOB_RECURSE TEST_SOURCES tests/*.cpp tests/*.hpp)

add_executable(sw_tests ${SRC_SOURCES} ${TEST_SOURCES})
target_include_directories(sw_tests PUBLIC src/ tests/)

enable_testing()
add_test(NAME sw_tests COMMAND sw_tests)
