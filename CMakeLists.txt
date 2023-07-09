cmake_minimum_required(VERSION 3.5)

project(engine_framework LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)

file(GLOB SRC
    src/events/EventTicket.cpp
    src/events/EventReceiver.cpp
    src/events/EventTicketCourier.cpp
    src/events/EventSource.cpp
)

add_library(engine_framework_lib STATIC ${SRC})
target_include_directories(engine_framework_lib PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
)
target_compile_options(engine_framework_lib PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Werror
)

add_executable(engine_framework main.cpp)
target_link_libraries(engine_framework
    engine_framework_lib
)

target_include_directories(engine_framework PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/FakeIt/single_header/boost/
)

install(TARGETS engine_framework_lib engine_framework
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
