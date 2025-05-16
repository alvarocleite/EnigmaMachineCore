
cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
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
)

target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/RotorBox/src/RotorBox.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RotorBox/Transformer/src/Transformer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RotorBox/Transformer/Reflector/src/Reflector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RotorBox/Transformer/Rotor/src/Rotor.cpp
)
