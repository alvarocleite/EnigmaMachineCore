# Generation of Documentation
find_package(Doxygen)
find_program(PLANTUML_PATH NAMES plantuml plantuml.jar PATHS /usr/bin /usr/share/plantuml /usr/local/bin)

if (PLANTUML_PATH)
    message(STATUS "Found PlantUML: ${PLANTUML_PATH}")
else()
    message(STATUS "PlantUML not found. Diagrams in documentation might fail to generate.")
endif()

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
