cmake_minimum_required(VERSION 3.5)

set(CMAKE_BUILD_TYPE, Debug)

find_package(Boost REQUIRED)

set(SOURCES
    sin.cpp
    sin_value.cpp
    )

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

add_library(sin SHARED ${SOURCES} )
target_link_libraries(sin PUBLIC ${Boost_LIBRARIES})