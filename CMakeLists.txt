# Minimum CMake version required
cmake_minimum_required(VERSION 3.15)

# Set Default Build Type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

# Define project name, version, and language
project(EnigmaMachineCore VERSION 0.0.1 LANGUAGES CXX)

# --- Submodule Support & Options ---
# Check if this project is the top-level project
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(ENIGMA_IS_TOP_LEVEL ON)
else()
    set(ENIGMA_IS_TOP_LEVEL OFF)
endif()

# Define options to control build targets
option(ENIGMA_BUILD_CLI "Build the Enigma Machine CLI executable" ${ENIGMA_IS_TOP_LEVEL})
option(ENIGMA_BUILD_TESTS "Build the test suite" ${ENIGMA_IS_TOP_LEVEL})

# --- C++ Standard ---
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Source Files ---
# Group core logic files for the library
set(CORE_SOURCES
    EnigmaMachine/src/EnigmaMachine.cpp
    EnigmaMachine/src/EnigmaMachineConfig.cpp
    EnigmaMachine/src/EnigmaConfigLoader.cpp
    EnigmaMachine/src/FileAssetProvider.cpp
    PlugBoard/src/PlugBoard.cpp
    RotorBox/src/RotorBox.cpp
    RotorBox/src/Transformer.cpp
    RotorBox/src/Reflector.cpp
    RotorBox/src/Rotor.cpp
)

# Create Target (EnigmaCore)
# Create a static library contains the business logic, to be testable without main()
add_library(EnigmaCore STATIC
    ${CORE_SOURCES}
)

# --- Include Directories ---
# Define search paths for header files
set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/RotorBox/include
    ${CMAKE_CURRENT_SOURCE_DIR}/PlugBoard/include
    ${CMAKE_CURRENT_SOURCE_DIR}/EnigmaMachine/include
    ${CMAKE_CURRENT_SOURCE_DIR}/config/
)

# Apply include paths to EnigmaCore
# PUBLIC visibility ensures any target linking EnigmaCore also inherits these paths
target_include_directories(EnigmaCore PUBLIC ${INCLUDE_DIRS})

# --- Compiler Warnings ---
if(MSVC)
    target_compile_options(EnigmaCore PRIVATE /W4)
else()
    target_compile_options(EnigmaCore PRIVATE -Wall -Wextra -Wpedantic)
endif()

# --- Static Analysis (clang-tidy) ---
option(ENABLE_CLANG_TIDY "Enable static analysis with clang-tidy" OFF)

if(ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE clang-tidy)
    if(CLANG_TIDY_EXE)
        message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
        set(CLANG_TIDY_CHECKS
            "-*,bugprone-*,modernize-*,performance-*,readability-*"
            "-modernize-use-trailing-return-type"
        )
        string(REPLACE ";" "," CLANG_TIDY_CHECKS_STR "${CLANG_TIDY_CHECKS}")
        
        set_target_properties(EnigmaCore PROPERTIES
            CXX_CLANG_TIDY "${CLANG_TIDY_EXE};-checks=${CLANG_TIDY_CHECKS_STR}"
        )
    else()
        message(WARNING "clang-tidy not found. Static analysis will not be run.")
    endif()
endif()

# --- External Dependencies ---
# Initialize and link toml11 library (header-only submodule)
add_subdirectory(external/toml11)
target_link_libraries(EnigmaCore PUBLIC toml11::toml11)

# --- Main Executable Target ---
if(ENIGMA_BUILD_CLI)
    # Application entry point
    set(APP_SOURCES
        app/main.cpp
    )

    # Initialize and link CLI11 library (Only needed for the CLI app)
    add_subdirectory(external/CLI11)

    # The EnigmaMachineCore application executable, which is just a wrapper around the EnigmaCore
    add_executable(${PROJECT_NAME}
        ${APP_SOURCES}
    )

    # Link the core library logic to the executable
    target_link_libraries(${PROJECT_NAME} PRIVATE EnigmaCore CLI11::CLI11)

    # --- Asset Handling ---
    # Copy assets to build directory after build
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/assets
            ${CMAKE_CURRENT_BINARY_DIR}/assets
        COMMENT "Copying assets to build directory"
    )
endif()

# --- Documentation ---
# Include external module for Doxygen and PlantUML configuration
include(cmake/Doc.cmake)

# --- Testing (GoogleTest) ---
if(ENIGMA_BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()
    add_subdirectory(tests)
endif()

# --- Code Formatting (clang-format) ---
find_program(CLANG_FORMAT_EXE clang-format)
if(CLANG_FORMAT_EXE)
    file(GLOB_RECURSE ALL_SOURCES
        "EnigmaMachine/*.cpp" "EnigmaMachine/*.hpp"
        "PlugBoard/*.cpp" "PlugBoard/*.hpp"
        "RotorBox/*.cpp" "RotorBox/*.hpp"
        "tests/*.cpp"
    )
    list(APPEND ALL_SOURCES "app/main.cpp" "config/config.hpp")
    
    add_custom_target(format
        COMMAND ${CLANG_FORMAT_EXE} -i -style=file ${ALL_SOURCES}
        COMMENT "Formatting source code with clang-format"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
else()
    message(WARNING "clang-format not found. The 'format' target will not be available.")
endif()
