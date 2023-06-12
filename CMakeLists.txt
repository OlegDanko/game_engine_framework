cmake_minimum_required(VERSION 3.5)

project(game_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)

file(GLOB SRC
    src/events/EventTicket.cpp
    src/events/EventReceiver.cpp
    src/events/EventTicketCourier.cpp
    src/events/EventSource.cpp
)

add_library(game_engine_lib STATIC ${SRC})
add_executable(game_engine main.cpp)
target_link_libraries(game_engine game_engine_lib)

target_include_directories(game_engine_lib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/)
target_include_directories(game_engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/)
target_compile_options(game_engine PRIVATE -Wall -Wextra -Wpedantic -Werror)

install(TARGETS game_engine
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
