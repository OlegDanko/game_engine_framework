cmake_minimum_required(VERSION 3.5)

project(game_engine_framework LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)

file(GLOB SRC
    src/events/EventTicket.cpp
    src/events/EventReceiver.cpp
    src/events/EventTicketCourier.cpp
    src/events/EventSource.cpp
)

add_library(game_engine_framework_lib STATIC ${SRC})
target_include_directories(game_engine_framework_lib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/)
target_compile_options(game_engine_framework_lib PRIVATE -Wall -Wextra -Wpedantic -Werror)
