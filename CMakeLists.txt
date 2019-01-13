# cmake_minimum_required(VERSION <specify CMake version here>)
project(untitle)

set(CMAKE_CXX_STANDARD 11)

add_executable(untitle main.cpp)

include_directories(${PROJECT_SOURCE_DIR}/include)
add_library(bprinter STATIC
        include/bprinter/table_printer.h
        include/bprinter/impl/table_printer.tpp.h
        src/table_printer.cpp
        )
target_link_libraries(untitle bprinter)