cmake_minimum_required(VERSION 3.16)

project(The_Game_Of_Dev LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(The_Game_Of_Dev
    src/main.cpp
)

add_executable(echo_server
    src/server/main.cpp
)

target_include_directories(echo_server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/httplib
)

add_library(calc_core
    src/calc/lexer.cpp
    src/calc/parser.cpp
    src/calc/eval.cpp
)

target_include_directories(calc_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(calc_cli
    src/calc_cli.cpp
)

target_link_libraries(calc_cli PRIVATE calc_core)

target_link_libraries(echo_server PRIVATE calc_core)

target_include_directories(echo_server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

