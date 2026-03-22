# Minimum CMake version required
cmake_minimum_required(VERSION 3.15)

# Set Default Build Type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

# Define project name, version, and language
project(EnigmaMachineCore VERSION 0.1.0 LANGUAGES CXX)

# --- GNU Install Directories ---
include(GNUInstallDirs)

# --- Submodule Support & Options ---
# Check if this project is the top-level project
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(ENIGMA_IS_TOP_LEVEL ON)
else()
    set(ENIGMA_IS_TOP_LEVEL OFF)
endif()

# Define options to control build targets
option(ENIGMA_BUILD_CLI "Build the Enigma Machine CLI executable" ${ENIGMA_IS_TOP_LEVEL})
option(ENIGMA_BUILD_TESTS "Build the test suite" OFF)
option(ENIGMA_BUILD_BENCHMARKS "Build the benchmark suite" OFF)
option(ENIGMA_BUILD_DOCS "Build the documentation" OFF)

# --- C++ Standard ---
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Global Visibility Settings ---
# Hide symbols by default to enforce clean API
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

# --- Source Files ---
# Group core logic files for the library
set(CORE_SOURCES
    EnigmaMachine/src/EnigmaMachine.cpp
    EnigmaMachine/src/EnigmaMachineConfig.cpp
    EnigmaMachine/src/EnigmaConfigLoader.cpp
    EnigmaMachine/src/FileAssetProvider.cpp
    EnigmaMachine/src/IEnigmaObserver.cpp
    EnigmaMachine/src/IAssetProvider.cpp
    PlugBoard/src/PlugBoard.cpp
    RotorBox/src/RotorBox.cpp
    RotorBox/src/Transformer.cpp
    RotorBox/src/Reflector.cpp
    RotorBox/src/Rotor.cpp
)

# --- Include Directories ---
# Define search paths for header files
set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/RotorBox/include
    ${CMAKE_CURRENT_SOURCE_DIR}/PlugBoard/include
    ${CMAKE_CURRENT_SOURCE_DIR}/EnigmaMachine/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/expected-lite/include
)

# --- External Dependencies ---
# Initialize toml11 library (header-only submodule)
add_subdirectory(external/toml11)

# Ensure toml11 headers are treated as SYSTEM to suppress warnings and sanitizer noise
get_target_property(TOML11_INCLUDE_DIRS toml11 INTERFACE_INCLUDE_DIRECTORIES)
set_target_properties(toml11 PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${TOML11_INCLUDE_DIRS}")

# --- Core Targets ---

# 1. Create Object Library for all core logic
# This target contains all symbols but they are hidden by default.
# Unit tests will link directly to these objects.
add_library(EnigmaCore_OBJ OBJECT
    ${CORE_SOURCES}
)
target_include_directories(EnigmaCore_OBJ PUBLIC ${INCLUDE_DIRS})
target_link_libraries(EnigmaCore_OBJ PUBLIC toml11::toml11)
set_target_properties(EnigmaCore_OBJ PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Define the installed assets path for the library fallback
target_compile_definitions(EnigmaCore_OBJ PRIVATE
    ENIGMA_INSTALL_ASSETS_PATH="${CMAKE_INSTALL_FULL_DATADIR}/EnigmaMachineCore/assets/"
)

# On Windows, when building a shared library, the object files must also
# have the EXPORTS definition so they know to export symbols rather than import them.
if(BUILD_SHARED_LIBS)
    target_compile_definitions(EnigmaCore_OBJ PRIVATE EnigmaCore_EXPORTS)
endif()

# 2. Create the final EnigmaCore library target (Static or Shared)
# This target ONLY exports symbols annotated with ENIGMACORE_EXPORT.
add_library(EnigmaCore
    $<TARGET_OBJECTS:EnigmaCore_OBJ>
)
target_link_libraries(EnigmaCore PRIVATE $<BUILD_INTERFACE:toml11::toml11>)
target_include_directories(EnigmaCore
    PUBLIC
        $<BUILD_INTERFACE:${INCLUDE_DIRS}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Create namespaced alias for safer consumption
add_library(EnigmaMachineCore::EnigmaCore ALIAS EnigmaCore)

# --- Symbol Visibility (Shared Library Support) ---
include(GenerateExportHeader)
generate_export_header(EnigmaCore
    BASE_NAME ENIGMACORE
    EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/generated/EnigmaCore_EXPORT.hpp
)

# Add the generated directory to include paths
target_include_directories(EnigmaCore_OBJ PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated>
)
target_include_directories(EnigmaCore PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated>
)

# --- Compiler Warnings ---
if(MSVC)
    target_compile_options(EnigmaCore_OBJ PRIVATE /W4)
else()
    target_compile_options(EnigmaCore_OBJ PRIVATE -Wall -Wextra -Wpedantic)
endif()

# --- Static Analysis (clang-tidy) ---
option(ENIGMA_ENABLE_CLANG_TIDY "Enable static analysis with clang-tidy" OFF)

if(ENIGMA_ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE clang-tidy)
    if(CLANG_TIDY_EXE)
        message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
        set(CLANG_TIDY_CHECKS
            "-*,bugprone-*,modernize-*,performance-*,readability-*"
            "-modernize-use-trailing-return-type"
        )
        string(REPLACE ";" "," CLANG_TIDY_CHECKS_STR "${CLANG_TIDY_CHECKS}")

        set_target_properties(EnigmaCore_OBJ PROPERTIES
            CXX_CLANG_TIDY "${CLANG_TIDY_EXE};-checks=${CLANG_TIDY_CHECKS_STR}"
        )
    else()
        message(WARNING "clang-tidy not found. Static analysis will not be run.")
    endif()
endif()

# --- Static Analysis (cppcheck) ---
option(ENIGMA_ENABLE_CPPCHECK "Enable static analysis with cppcheck" OFF)

if(ENIGMA_ENABLE_CPPCHECK)
    find_program(CPPCHECK_EXE cppcheck)
    if(CPPCHECK_EXE)
        message(STATUS "cppcheck found: ${CPPCHECK_EXE}")

        # Define include directories for cppcheck to use
        set(CPPCHECK_INCLUDES "")
        foreach(inc ${INCLUDE_DIRS})
            list(APPEND CPPCHECK_INCLUDES "-I${inc}")
        endforeach()

        # Add custom target to run cppcheck
        # Using --project=compile_commands.json is usually better but requires CMAKE_EXPORT_COMPILE_COMMANDS=ON
        add_custom_target(enigma_cppcheck
            COMMAND ${CPPCHECK_EXE}
                --enable=performance,portability,style,unusedFunction
                --std=c++20
                --inline-suppr
                --suppress=missingIncludeSystem
                --force
                --quiet
                --error-exitcode=1
                ${CPPCHECK_INCLUDES}
                ${CORE_SOURCES}
                app/main.cpp
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Running cppcheck on core sources and application"
        )
    else()
        message(WARNING "cppcheck not found. The 'enigma_cppcheck' target will not be available.")
    endif()
endif()

# --- Sanitizers ---
option(ENIGMA_USE_ASAN "Enable AddressSanitizer" OFF)
option(ENIGMA_USE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)
option(ENIGMA_USE_MSAN "Enable MemorySanitizer (Clang only)" OFF)

set(SANITIZER_FLAGS "")
if(ENIGMA_USE_ASAN OR ENIGMA_USE_UBSAN OR ENIGMA_USE_MSAN)
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        message(WARNING "Sanitizers are intended for use with Clang as per project requirements.")
    endif()

    # Sanitizer Ignore List (excludes external libraries from instrumentation)
    set(IGNORE_LIST_FILE "${CMAKE_CURRENT_SOURCE_DIR}/.sanitizer_ignorelist.txt")
    set(IGNORE_LIST_FLAG "-fsanitize-ignorelist=${IGNORE_LIST_FILE}")

    if(ENIGMA_USE_ASAN)
        list(APPEND SANITIZER_FLAGS "-fsanitize=address" "${IGNORE_LIST_FLAG}")
    endif()
    if(ENIGMA_USE_UBSAN)
        list(APPEND SANITIZER_FLAGS "-fsanitize=undefined" "-fno-sanitize-recover=all" "${IGNORE_LIST_FLAG}")
    endif()
    if(ENIGMA_USE_MSAN)
        list(APPEND SANITIZER_FLAGS "-fsanitize=memory" "-fsanitize-memory-track-origins" "-fno-omit-frame-pointer" "${IGNORE_LIST_FLAG}")
    endif()

    if(SANITIZER_FLAGS)
        message(STATUS "Enabling Sanitizers: ${SANITIZER_FLAGS}")

        # Apply to core object library
        target_compile_options(EnigmaCore_OBJ PRIVATE ${SANITIZER_FLAGS})

        # Apply to core library (linkage)
        target_link_options(EnigmaCore PRIVATE ${SANITIZER_FLAGS})
    endif()
endif()

# --- Code Coverage ---
option(ENIGMA_ENABLE_COVERAGE "Enable code coverage reporting with gcov/lcov" OFF)

if(ENIGMA_ENABLE_COVERAGE)
    find_program(GCOV_EXE gcov)
    find_program(LCOV_EXE lcov)
    find_program(GENHTML_EXE genhtml)

    if(NOT GCOV_EXE OR NOT LCOV_EXE OR NOT GENHTML_EXE)
        message(WARNING "gcov, lcov, or genhtml not found. Coverage reporting will not be available.")
        set(ENIGMA_ENABLE_COVERAGE OFF)
    endif()

    if(ENIGMA_ENABLE_COVERAGE)
        message(STATUS "Code coverage enabled")
        set(COVERAGE_FLAGS -fprofile-arcs -ftest-coverage)

        target_compile_options(EnigmaCore_OBJ PRIVATE ${COVERAGE_FLAGS})
    endif()
endif()

# --- Main Executable Target ---
if(ENIGMA_BUILD_CLI)
    # Application entry point
    set(APP_SOURCES
        app/main.cpp
    )

    # Initialize and link CLI11 library (Only needed for the CLI app)
    add_subdirectory(external/CLI11)

    # Ensure CLI11 headers are treated as SYSTEM
    get_target_property(CLI11_INCLUDE_DIRS CLI11 INTERFACE_INCLUDE_DIRECTORIES)
    set_target_properties(CLI11 PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${CLI11_INCLUDE_DIRS}")

    # The EnigmaMachineCore application executable, which is just a wrapper around the EnigmaCore
    add_executable(${PROJECT_NAME}
        ${APP_SOURCES}
    )

    # Add include directories directly to the executable to ensure it can find headers
    target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})

    # Link the core library logic to the executable
    target_link_libraries(${PROJECT_NAME} PRIVATE EnigmaMachineCore::EnigmaCore CLI11::CLI11)

    # Apply sanitizers to CLI if enabled
    if(SANITIZER_FLAGS)
        target_compile_options(${PROJECT_NAME} PRIVATE ${SANITIZER_FLAGS})
        target_link_options(${PROJECT_NAME} PRIVATE ${SANITIZER_FLAGS})
    endif()

    # Apply coverage flags if enabled
    if(ENIGMA_ENABLE_COVERAGE)
        target_compile_options(${PROJECT_NAME} PRIVATE ${COVERAGE_FLAGS})
        target_link_options(${PROJECT_NAME} PRIVATE ${COVERAGE_FLAGS})
    endif()

    # Define the installed assets path for the CLI fallback
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        ENIGMA_INSTALL_ASSETS_PATH="${CMAKE_INSTALL_FULL_DATADIR}/EnigmaMachineCore/assets/"
    )

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
if(ENIGMA_BUILD_DOCS)
    include(cmake/Doc.cmake)
endif()

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

    # Create a separate non-instrumented object library for tests if sanitizers are active
    # to satisfy the requirement: "ensure these sanitizers do not run on the Unit Test suite targets"
    if(SANITIZER_FLAGS)
        add_library(EnigmaCore_NoSan_OBJ OBJECT ${CORE_SOURCES})
        target_include_directories(EnigmaCore_NoSan_OBJ PUBLIC ${INCLUDE_DIRS})
        target_link_libraries(EnigmaCore_NoSan_OBJ PUBLIC toml11::toml11)
        set_target_properties(EnigmaCore_NoSan_OBJ PROPERTIES POSITION_INDEPENDENT_CODE ON)
        target_compile_definitions(EnigmaCore_NoSan_OBJ PRIVATE
            ENIGMA_INSTALL_ASSETS_PATH="${CMAKE_INSTALL_FULL_DATADIR}/EnigmaMachineCore/assets/"
        )
        if(MSVC)
            target_compile_options(EnigmaCore_NoSan_OBJ PRIVATE /W4)
        else()
            target_compile_options(EnigmaCore_NoSan_OBJ PRIVATE -Wall -Wextra -Wpedantic)
        endif()
    endif()

    add_subdirectory(tests)

    if(ENIGMA_ENABLE_COVERAGE)
        add_custom_target(enigma_coverage
            COMMAND ${LCOV_EXE} --capture --directory . --output-file coverage_raw.info --include 'EnigmaMachineCore/*' --ignore-errors gcov,source,inconsistent,mismatch || true
            COMMAND ${LCOV_EXE} --remove coverage_raw.info '*/tests/*' '*/_deps/*' '*/external/*' --output-file coverage.info --ignore-errors inconsistent || true
            COMMAND ${GENHTML_EXE} coverage.info --output-directory coverage_html --branch-coverage --legend
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            DEPENDS EnigmaTests
            COMMENT "Generating lcov coverage report"
        )

        add_custom_target(enigma_core_coverage
            COMMAND ${LCOV_EXE} --capture --directory . --output-file coverage_core_raw.info --include 'EnigmaMachineCore/*' --ignore-errors gcov,source,inconsistent,mismatch || true
            COMMAND ${LCOV_EXE} --remove coverage_core_raw.info '*/tests/*' '*/_deps/*' '*/external/*' --output-file coverage_core.info --ignore-errors inconsistent || true
            COMMAND ${LCOV_EXE} --extract coverage_core.info '*/RotorBox/src/*' '*/PlugBoard/src/*' '*/EnigmaMachine/src/EnigmaMachine.cpp' --output-file coverage_core_only.info --ignore-errors mismatch || true
            COMMAND ${GENHTML_EXE} coverage_core_only.info --output-directory coverage_core_html --branch-coverage --legend
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            DEPENDS EnigmaTests
            COMMENT "Generating core crypto-only lcov coverage report"
        )
    endif()
endif()

# --- Benchmarking (Google Benchmark) ---
if(ENIGMA_BUILD_BENCHMARKS)
    include(FetchContent)
    FetchContent_Declare(
        benchmark
        URL https://github.com/google/benchmark/archive/refs/tags/v1.8.3.zip
    )
    # Disable building benchmark tests and examples to speed up build
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
    set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
    set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(benchmark)

    add_subdirectory(benchmarks)
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
    list(APPEND ALL_SOURCES "app/main.cpp")

    add_custom_target(enigma_format
        COMMAND ${CLANG_FORMAT_EXE} -i -style=file ${ALL_SOURCES}
        COMMENT "Formatting source code with clang-format"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
else()
    message(WARNING "clang-format not found. The 'enigma_format' target will not be available.")
endif()

# --- Installation Rules ---

# 1. Install EnigmaCore library
install(TARGETS EnigmaCore
    EXPORT EnigmaMachineCoreTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# 2. Install public headers
set(PUBLIC_HEADERS
    include/EnigmaCore.hpp
    include/EnigmaTypes.hpp
    include/EnigmaConfig.hpp
    include/EnigmaData.hpp
    include/EnigmaError.hpp
    EnigmaMachine/include/EnigmaMachine.hpp
    EnigmaMachine/include/IEnigmaObserver.hpp
    EnigmaMachine/include/IAssetProvider.hpp
    PlugBoard/include/PlugBoardPair.hpp
)

install(FILES ${PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/EnigmaMachineCore
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/generated/EnigmaCore_EXPORT.hpp
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/EnigmaMachineCore
)

# 3. Install assets
install(DIRECTORY assets/
    DESTINATION ${CMAKE_INSTALL_DATADIR}/EnigmaMachineCore/assets
)

# 4. Install CLI executable
if(ENIGMA_BUILD_CLI)
    install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()

# --- Package Export (find_package support) ---

# 1. Install the export set
install(EXPORT EnigmaMachineCoreTargets
    FILE EnigmaMachineCoreTargets.cmake
    NAMESPACE EnigmaMachineCore::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/EnigmaMachineCore
)

# 2. Generate and install the Config and Version files
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/EnigmaMachineCoreConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/EnigmaMachineCoreConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/EnigmaMachineCoreConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/EnigmaMachineCore
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/EnigmaMachineCoreConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/EnigmaMachineCoreConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/EnigmaMachineCore
)
