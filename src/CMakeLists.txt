cmake_minimum_required(VERSION 3.14)
project(MyProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

# Adjust this if you’re on Intel macOS:
# set(GTEST_ROOT /usr/local/opt/googletest)
# On Apple Silicon (M1/M2/M3), Homebrew installs under /opt/homebrew
set(GTEST_ROOT /opt/homebrew/opt/googletest)

find_package(GTest REQUIRED CONFIG PATHS ${GTEST_ROOT})

# Your main library
add_library(OrderBook OrderBook.cpp)
target_include_directories(OrderBook PUBLIC OrderBook)

# Your test executable
add_executable(OrderBookTest OrderBookTest/test.cpp)
target_link_libraries(OrderBookTest PRIVATE OrderBook GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(OrderBookTest)
