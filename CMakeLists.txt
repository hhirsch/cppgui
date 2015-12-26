cmake_minimum_required(VERSION 2.6)

project(GWEN CXX)

file(GLOB_RECURSE GWEN_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src" "*.cpp")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

add_library(GWEN STATIC ${GWEN_SOURCES})