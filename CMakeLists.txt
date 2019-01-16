cmake_minimum_required(VERSION 2.8)
project(ll1_analyzer)
set(CMAKE_CXX_STANDARD 11)
add_executable(ll1_analyzer main.cpp)
include_directories(${PROJECT_SOURCE_DIR}/include)
add_library(bprinter STATIC
        include/bprinter/table_printer.h
        include/bprinter/impl/table_printer.tpp.h
        src/table_printer.cpp
        )
target_link_libraries(ll1_analyzer bprinter)