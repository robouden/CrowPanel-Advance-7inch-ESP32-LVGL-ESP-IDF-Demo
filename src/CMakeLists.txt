# This file was automatically generated for projects
# without default 'CMakeLists.txt' file.

FILE(GLOB ui_sources 
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/*.c
)

FILE(GLOB display_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/display/*.c
)

idf_component_register(
    SRCS 
        "main.c"
        ${ui_sources}
        ${display_sources}
    INCLUDE_DIRS 
        "."
        "ui"
        "display"
)

# Optional: Set component properties if needed
# set_component_property(${COMPONENT_LIB} PROPERTY ...)
