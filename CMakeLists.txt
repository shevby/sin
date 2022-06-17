cmake_minimum_required(VERSION 3.5)
project(Sin VERSION 0.9.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LIB_NAME sin)

option(BUILD_TEST "Build catch2 tests" ON)

set(SOURCES
  sin.cpp
  sin_value.cpp
  sin_parser.cpp
)

add_library(${LIB_NAME} SHARED ${SOURCES})

include_directories(".")

if(BUILD_TEST) 
  add_subdirectory(test)
endif()
