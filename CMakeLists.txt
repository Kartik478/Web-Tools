cmake_minimum_required(VERSION 3.12)
project(CrossDevToolkit VERSION 0.1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Option for building shared libraries
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

# Option for building Node.js native modules
option(BUILD_NODE_MODULES "Build Node.js native modules" OFF)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# Platform-specific settings
if(WIN32)
    add_definitions(-D_WIN32)
    set(PLATFORM_SOURCES
        src/core/filesystem_win.cpp
    )
elseif(APPLE)
    add_definitions(-D__APPLE__)
    set(PLATFORM_SOURCES
        src/core/filesystem_unix.cpp
    )
else()
    add_definitions(-D__unix__)
    set(PLATFORM_SOURCES
        src/core/filesystem_unix.cpp
    )
endif()

# Core library
add_library(crossdev
    ${PLATFORM_SOURCES}
)

# Set output directory
set_target_properties(crossdev PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Installation
install(TARGETS crossdev
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES
    src/core/filesystem.hpp
    DESTINATION include/crossdev
)

# Node.js native module (optional)
if(BUILD_NODE_MODULES)
    # Check for Node.js and npm
    find_program(NODE_EXECUTABLE node)
    find_program(NPM_EXECUTABLE npm)
    
    if(NOT NODE_EXECUTABLE OR NOT NPM_EXECUTABLE)
        message(FATAL_ERROR "Node.js and npm are required to build Node.js modules")
    endif()
    
    # Set up Node-API module
    set(NODE_MODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/bindings/node)
    
    # Custom target to build the Node.js module
    add_custom_target(node_modules
        COMMAND ${NPM_EXECUTABLE} install
        COMMAND ${NPM_EXECUTABLE} run build
        WORKING_DIRECTORY ${NODE_MODULE_DIR}
        COMMENT "Building Node.js native module"
        VERBATIM
    )
    
    # Make node_modules depend on the core library
    add_dependencies(node_modules crossdev)
endif()

# Add example executables
add_executable(filesystem_example tools/filesystem_example.cpp)
target_link_libraries(filesystem_example crossdev)

# Set output directory for examples
set_target_properties(filesystem_example PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Add tests if enabled
option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif() 