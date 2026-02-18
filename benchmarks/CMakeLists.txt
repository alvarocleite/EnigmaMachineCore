# Add benchmark target
add_executable(EnigmaBenchmark EnigmaBenchmark.cpp)

# Link benchmark library and EnigmaCore
target_link_libraries(EnigmaBenchmark PRIVATE 
    benchmark::benchmark 
    EnigmaMachineCore::EnigmaCore
)

# --- Asset Handling for Benchmarks ---
add_custom_command(TARGET EnigmaBenchmark POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/../assets
        ${CMAKE_CURRENT_BINARY_DIR}/assets
    COMMENT "Copying assets to benchmarks build directory"
)

# Disable unnecessary warnings for benchmark executable if needed
if(NOT MSVC)
    target_compile_options(EnigmaBenchmark PRIVATE -Wno-unused-parameter)
endif()
