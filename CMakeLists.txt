cmake_minimum_required(VERSION 2.8)

cmake_policy(SET CMP0015 NEW)

project(tct)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

include_directories( "${PROJECT_SOURCE_DIR}/tct")

# you must change this
# set the (include)path of boost, so we can use #include <boost/...> in source files
include_directories( "../boost/boost_1_53_0/build/include/boost-1_53")

# you must change this
# set the (libs)path of boost, you should have builded two modules ('system' and 'filesystem') of boost.
link_directories("../boost/boost_1_53_0/build/lib")

# you must change this
# set the (include)path of glags, so we can use #include <gflags/...> in source files
include_directories( "../gflags-2.0/gflags-2.0/src/windows")

# you must change this
# set the (libs)path of libs of boost,you should have builded the gflags.
link_directories( "../gflags-2.0/gflags-2.0/Release")

# you may change this
# if you are using linux. you may remove the lib from the head of the flags(???)
link_libraries(tct "libgflags")

file(GLOB_RECURSE tct_SRCS "${PROJECT_SOURCE_DIR}/src/*.cpp" "${PROJECT_SOURCE_DIR}/src/*.h")

add_executable(tct ${tct_SRCS})
