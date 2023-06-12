cmake_minimum_required(VERSION 3.5)

project(game_engine_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB TEST_SRC
    TestFlatteningLifoQueue.cpp
    game_state/TestAttributeStateFrame.cpp
    game_state/TestAttributeStateQueue.cpp
    game_state/TestGameState.cpp
    game_state/TestGameStateGenerator.cpp
    game_state/TestGameStateGenNotifier.cpp
    game_state/TestAttributeProcessorBase.cpp
    utils/TestTypeChain.cpp
    utils/TestTypeUtils.cpp
    events/TestEventTicket.cpp
    events/TestEventSource.cpp
    events/TestEventTicketCourier.cpp
    events/TestEventReceiver.cpp
)

find_package(Boost REQUIRED COMPONENTS unit_test_framework)
add_executable(game_engine_tests main.cpp ${TEST_SRC})
target_link_libraries(game_engine_tests game_engine_lib Boost::unit_test_framework)
target_compile_options(game_engine_tests PRIVATE -Wall -Wextra -Wpedantic -Werror)

target_include_directories(game_engine_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/deps/FakeIt/single_header/boost
    ${CMAKE_SOURCE_DIR}/include
)
