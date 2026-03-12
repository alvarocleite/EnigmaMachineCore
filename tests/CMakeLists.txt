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

# Register tests so CTest can find them
include(GoogleTest)
gtest_discover_tests(EnigmaTests)

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
