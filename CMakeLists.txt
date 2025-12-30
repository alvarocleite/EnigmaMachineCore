
cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(Enigma-Machine-Core VERSION 0.0.1)
add_executable(${PROJECT_NAME}
    main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/RotorBox/include
    ${CMAKE_CURRENT_SOURCE_DIR}/RotorBox/Transformer/include
    ${CMAKE_CURRENT_SOURCE_DIR}/RotorBox/Transformer/Reflector/include
    ${CMAKE_CURRENT_SOURCE_DIR}/RotorBox/Transformer/Rotor/include
    ${CMAKE_CURRENT_SOURCE_DIR}/PlugBoard/include
    ${CMAKE_CURRENT_SOURCE_DIR}/EnigmaMachine/include
    ${CMAKE_CURRENT_SOURCE_DIR}/config/
)

target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/RotorBox/src/RotorBox.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RotorBox/Transformer/src/Transformer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RotorBox/Transformer/Reflector/src/Reflector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RotorBox/Transformer/Rotor/src/Rotor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PlugBoard/src/PlugBoard.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/EnigmaMachine/src/EnigmaMachine.cpp
)

add_subdirectory(external/toml11)
target_link_libraries(${PROJECT_NAME} PRIVATE toml11::toml11)

# Documentation
find_package(Doxygen)
find_program(PLANTUML_PATH NAMES plantuml plantuml.jar PATHS /usr/bin /usr/share/plantuml /usr/local/bin)

if (DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(doxygen
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)
    
    # Target to run everything (Build + Docs)
    add_custom_target(full_build)
    add_dependencies(full_build ${PROJECT_NAME} doxygen)
else()
    message(WARNING "Doxygen not found in the system. Documentation will not be generated.")
endif()
