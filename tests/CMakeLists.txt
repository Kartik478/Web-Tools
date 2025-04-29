# Tests configuration for CrossDev Toolkit

# Include Catch2 for testing
include(FetchContent)
FetchContent_Declare(
    catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v2.13.8
)
FetchContent_MakeAvailable(catch2)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)

# Filesystem tests
add_executable(filesystem_tests filesystem_tests.cpp)
target_link_libraries(filesystem_tests PRIVATE crossdev Catch2::Catch2)

# Add the test to CTest
add_test(NAME filesystem_tests COMMAND filesystem_tests)

# Set output directory for test binaries
set_target_properties(filesystem_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests
) 