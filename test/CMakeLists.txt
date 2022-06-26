set(TEST_MAIN test_sin)

set(TEST_SOURCES
  main.cpp
  test_parser.cpp
  ../sin.cpp
  ../sin_parser.cpp
)

Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.0.1 # or a later release
)

FetchContent_MakeAvailable(Catch2)

add_executable(${TEST_MAIN} ${TEST_SOURCES})

target_link_libraries(
  ${TEST_MAIN}
  PRIVATE
  Catch2::Catch2WithMain
)

set_target_properties(
  ${TEST_MAIN}
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
)