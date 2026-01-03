add_executable(EnigmaTests
    EnigmaMachine/TestEnigmaMachine.cpp
    PlugBoard/TestPlugBoard.cpp
    RotorBox/TestRotorBox.cpp
    RotorBox/Transformer/TestTransformer.cpp
    RotorBox/Transformer/Reflector/TestReflector.cpp
    RotorBox/Transformer/Rotor/TestRotor.cpp
)

# Link against the Core library and GTest
# GTest::gtest_main includes the main() entry point for the test runner
target_link_libraries(EnigmaTests PRIVATE EnigmaCore GTest::gtest_main)

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
