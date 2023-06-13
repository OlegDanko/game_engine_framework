cmake_minimum_required(VERSION 3.5)

project(game_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/engine_framework)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/io)

add_executable(game_engine main.cpp)
target_link_libraries(game_engine
    game_engine_framework_lib
    io
)

target_include_directories(game_engine PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/engine_framework/include/
)
target_compile_options(game_engine PRIVATE -Wall -Wextra -Wpedantic -Werror)

install(TARGETS game_engine
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
