# Generation of Documentation
find_package(Doxygen)
find_package(Java REQUIRED)

# Look for the JAR file specifically because Doxygen expects to run it with 'java -jar'
find_file(PLANTUML_PATH 
    NAMES plantuml.jar 
    PATHS 
    /usr/share/java/plantuml
    /usr/share/plantuml
    /usr/local/share/plantuml
    /usr/local/bin
    /opt/plantuml
    DOC "Path to plantuml.jar"
)

if (PLANTUML_PATH)
    message(STATUS "Found PlantUML JAR: ${PLANTUML_PATH}")

    # --- Automatic Diagram Generation ---
    # Find all .puml files in docs/diagrams
    file(GLOB PUML_FILES "${CMAKE_CURRENT_SOURCE_DIR}/docs/diagrams/*.puml")
    
    # Define the output directory for generated diagrams
    set(DIAGRAM_GEN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/docs/diagrams/output")

    # Create a custom target to generate SVGs
    # We use batch mode which automatically respects the filename defined in @startuml <name>
    add_custom_target(generate_diagrams
        COMMAND ${Java_JAVA_EXECUTABLE} -jar ${PLANTUML_PATH} -tsvg "${CMAKE_CURRENT_SOURCE_DIR}/docs/diagrams/*.puml" -o "${DIAGRAM_GEN_DIR}"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating SVG diagrams with PlantUML..."
        VERBATIM
    )

else()
    message(STATUS "PlantUML JAR not found. Diagrams in documentation might fail to generate.")
endif()

if (DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(doxygen
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)
    
    # Make Doxygen depend on the diagrams being generated first
    if (PLANTUML_PATH)
        add_dependencies(doxygen generate_diagrams)
    endif()
    
    # Target to run everything (Build + Docs)
    add_custom_target(full_build)
    add_dependencies(full_build ${PROJECT_NAME} doxygen)
else()
    message(WARNING "Doxygen not found in the system. Documentation will not be generated.")
endif()