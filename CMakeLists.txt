cmake_minimum_required(VERSION 3.5)
project(Sin VERSION 0.9.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LIB_NAME sin)

option(BUILD_TEST "Build catch2 tests" ON)

#Find and configure Boost
find_package(Boost REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

set(SOURCES
  sin.cpp
  sin_value.cpp
)

add_library(${LIB_NAME} SHARED ${SOURCES})
target_link_libraries(${LIB_NAME} PUBLIC ${Boost_LIBRARIES})

include_directories(".")

if(BUILD_TEST) 
  add_subdirectory(test)
endif()