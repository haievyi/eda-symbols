cmake_minimum_required(VERSION 3.15)

project(eda-symbols VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(eda-symbols
    src/iec.cpp
    src/ieee.cpp
)
add_library(eda::symbols ALIAS eda-symbols)

target_include_directories(eda-symbols
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(eda-symbols
    PUBLIC
        d2d1
)

option(BUILD_TESTING "Build unit tests and symbol image exporters" ON)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
