cmake_minimum_required(VERSION 3.0)

project(tct)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

include_directories( "${PROJECT_SOURCE_DIR}/tct")

file(GLOB_RECURSE tct_SRCS "${PROJECT_SOURCE_DIR}/src/*.cpp" "${PROJECT_SOURCE_DIR}/src/*.h")

add_executable(tct ${tct_SRCS})
