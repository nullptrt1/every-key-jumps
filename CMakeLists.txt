cmake_minimum_required(VERSION 3.5)

if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    message(FATAL_ERROR "Build in Release mode (-DCMAKE_BUILD_TYPE=Release)")
endif()

project(every-key-jumps-2.2)

add_library(every-key-jumps-2.2 SHARED dllmain.cpp)

target_include_directories(every-key-jumps-2.2 PRIVATE libs/MinHook/include)
target_link_libraries(every-key-jumps-2.2 ${CMAKE_SOURCE_DIR}/libs/MinHook/libMinHook-x86-v141-mtd.lib)