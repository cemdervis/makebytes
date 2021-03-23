cmake_policy(SET CMP0091 NEW)

project(sifu LANGUAGES CXX)

cmake_minimum_required(VERSION 3.15)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

file(GLOB ALL_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.*")

add_executable(makebytes ${ALL_FILES})

target_compile_features(makebytes PRIVATE cxx_std_17)
