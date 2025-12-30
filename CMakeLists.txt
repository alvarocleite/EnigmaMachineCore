
# Minimum CMake version required
cmake_minimum_required(VERSION 3.15)

# Set Default Build Type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

# Define project name, version, and language
project(EnigmaMachineCore VERSION 0.0.1 LANGUAGES CXX)

# --- C++ Standard ---
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Source Files ---
# Group core logic files for easier maintenance
set(CORE_SOURCES
    EnigmaMachine/src/EnigmaMachine.cpp
    PlugBoard/src/PlugBoard.cpp
    RotorBox/src/RotorBox.cpp
    RotorBox/Transformer/src/Transformer.cpp
    RotorBox/Transformer/Reflector/src/Reflector.cpp
    RotorBox/Transformer/Rotor/src/Rotor.cpp
)

# Application entry point
set(APP_SOURCES
    main.cpp
)

# Create the main executable target
add_executable(${PROJECT_NAME}
    ${APP_SOURCES}
    ${CORE_SOURCES}
)

# --- Compiler Warnings ---
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# --- Include Directories ---
# Define search paths for header files
set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/RotorBox/include
    ${CMAKE_CURRENT_SOURCE_DIR}/RotorBox/Transformer/include
    ${CMAKE_CURRENT_SOURCE_DIR}/RotorBox/Transformer/Reflector/include
    ${CMAKE_CURRENT_SOURCE_DIR}/RotorBox/Transformer/Rotor/include
    ${CMAKE_CURRENT_SOURCE_DIR}/PlugBoard/include
    ${CMAKE_CURRENT_SOURCE_DIR}/EnigmaMachine/include
    ${CMAKE_CURRENT_SOURCE_DIR}/config/
)

# Apply include paths to the target
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})

# --- External Dependencies ---
# Initialize and link toml11 library (header-only submodule)
add_subdirectory(external/toml11)
target_link_libraries(${PROJECT_NAME} PRIVATE toml11::toml11)

# --- Asset Handling ---
# Copy assets to build directory after build
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        ${CMAKE_CURRENT_BINARY_DIR}/assets
    COMMENT "Copying assets to build directory"
)

# --- Documentation ---
# Include external module for Doxygen and PlantUML configuration
include(cmake/Doc.cmake)
