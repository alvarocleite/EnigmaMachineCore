add_executable(EnigmaTests
    AssetProvider/TestFileAssetProvider.cpp
    EnigmaMachine/TestEnigmaMachine.cpp
    EnigmaMachine/TestEnigmaConfigLoader.cpp
    EnigmaMachine/TestEnigmaObserver.cpp
    EnigmaMachine/TestHistoricalVectors.cpp
    PlugBoard/TestPlugBoard.cpp
    RotorBox/TestRotorBox.cpp
    RotorBox/TestTransformer.cpp
    RotorBox/TestReflector.cpp
    RotorBox/TestRotor.cpp
)

# Link against the Core Logic (Object Library) and GTest
# Linking against the OBJ library allows access to hidden internal symbols
if(TARGET EnigmaCore_NoSan_OBJ)
    target_link_libraries(EnigmaTests PRIVATE EnigmaCore_NoSan_OBJ GTest::gtest_main)
else()
    target_link_libraries(EnigmaTests PRIVATE EnigmaCore_OBJ GTest::gtest_main)
endif()

# Apply coverage flags if enabled
if(ENIGMA_ENABLE_COVERAGE)
    target_compile_options(EnigmaTests PRIVATE ${COVERAGE_FLAGS})
    target_link_options(EnigmaTests PRIVATE ${COVERAGE_FLAGS})
endif()

# Register tests so CTest can find them
include(GoogleTest)
gtest_discover_tests(EnigmaTests)

# --- Property-Based Testing (RapidCheck) ---
option(ENIGMA_BUILD_PROPERTY_TESTS "Build property-based tests with RapidCheck" OFF)

if(ENIGMA_BUILD_PROPERTY_TESTS)
    # Fetch RapidCheck if not already available
    include(FetchContent)
    if(NOT TARGET RapidCheck::rapidcheck)
        FetchContent_Declare(
            rapidcheck
            GIT_REPOSITORY https://github.com/emil-e/rapidcheck.git
            GIT_TAG        master
        )
        FetchContent_MakeAvailable(rapidcheck)
    endif()

    # Find RapidCheck headers and libraries
    find_package(RapidCheck QUIET)
    if(NOT RapidCheck_FOUND AND NOT TARGET rapidcheck)
        message(WARNING "RapidCheck not found. Property tests will not be built.")
        return()
    endif()

    add_executable(EnigmaPropertyTests
        RotorBox/TestRotorProperties.cpp
        RotorBox/TestReflectorProperties.cpp
        PlugBoard/TestPlugBoardProperties.cpp
        RotorBox/TestRotorBoxProperties.cpp
        EnigmaMachine/TestEnigmaMachineProperties.cpp
    )

    # Link against the Core Logic, GTest, and RapidCheck
    if(TARGET EnigmaCore_NoSan_OBJ)
        target_link_libraries(EnigmaPropertyTests PRIVATE EnigmaCore_NoSan_OBJ GTest::gtest GTest::gtest_main)
    else()
        target_link_libraries(EnigmaPropertyTests PRIVATE EnigmaCore_OBJ GTest::gtest GTest::gtest_main)
    endif()

    # Add RapidCheck include directories
    target_include_directories(EnigmaPropertyTests PRIVATE
        ${CMAKE_BINARY_DIR}/_deps/rapidcheck-src/extras/gtest/include
        ${CMAKE_BINARY_DIR}/_deps/rapidcheck-src/include
    )

    # Link RapidCheck if available
    if(TARGET RapidCheck::rapidcheck)
        target_link_libraries(EnigmaPropertyTests PRIVATE RapidCheck::rapidcheck)
    elseif(TARGET rapidcheck)
        target_link_libraries(EnigmaPropertyTests PRIVATE rapidcheck)
    elseif(RapidCheck_FOUND)
        target_include_directories(EnigmaPropertyTests PRIVATE ${RAPIDCHECK_INCLUDE_DIRS})
        target_link_libraries(EnigmaPropertyTests PRIVATE ${RAPIDCHECK_LIBRARIES})
    endif()

    # Apply coverage flags if enabled
    if(ENIGMA_ENABLE_COVERAGE)
        target_compile_options(EnigmaPropertyTests PRIVATE ${COVERAGE_FLAGS})
        target_link_options(EnigmaPropertyTests PRIVATE ${COVERAGE_FLAGS})
    endif()

    # Register property tests
    gtest_discover_tests(EnigmaPropertyTests)

    # Copy assets for property tests
    add_custom_command(TARGET EnigmaPropertyTests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
            ${CMAKE_BINARY_DIR}/tests/assets
        COMMENT "Copying assets to property test build directory"
    )
endif()

# Copy assets to the test binary directory so tests can access configuration files
add_custom_command(TARGET EnigmaTests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        ${CMAKE_BINARY_DIR}/tests/assets
    COMMENT "Copying assets to test build directory"
)

# --- Valgrind Memory Profiling ---
find_program(VALGRIND_EXE valgrind)
if(VALGRIND_EXE)
    add_custom_target(Enigma_valgrind
        COMMAND ${VALGRIND_EXE} --leak-check=full --error-exitcode=1 ./EnigmaTests
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS EnigmaTests
        COMMENT "Running test suite through Valgrind for memory leak detection"
    )
else()
    message(STATUS "Valgrind not found. The 'Enigma_valgrind' target will not be available.")
endif()
