cmake_minimum_required(VERSION 3.5)

project(game_engine_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB TEST_SRC
    TestAttributeStateFrame.cpp
    TestFlatteningLifoQueue.cpp
    TestAttributeStateQueue.cpp
)

find_package(Boost REQUIRED COMPONENTS unit_test_framework)
add_executable(game_engine_tests main.cpp ${TEST_SRC})
target_link_libraries(game_engine_tests Boost::unit_test_framework)

target_include_directories(game_engine_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(game_engine_tests PRIVATE ${CMAKE_SOURCE_DIR}/include/)
target_include_directories(game_engine_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/deps/FakeIt/single_header/boost)
