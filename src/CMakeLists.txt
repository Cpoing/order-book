cmake_minimum_required(VERSION 3.14)
project(MyProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

set(GTEST_ROOT /opt/homebrew/opt/googletest)

find_package(GTest REQUIRED CONFIG PATHS ${GTEST_ROOT})

add_library(OrderBook OrderBook.cpp)
target_include_directories(OrderBook PUBLIC OrderBook)

add_executable(OrderBookTest OrderBookTest/test.cpp)
target_link_libraries(OrderBookTest PRIVATE OrderBook GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(OrderBookTest)
