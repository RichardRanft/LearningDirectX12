cmake_minimum_required( VERSION 3.17.0 ) # Latest version of CMake when this file was created.

set( HEADER_FILES
    inc/IRenderer.h
    inc/RendererPCH.h
)

set( SOURCE_FILES
    src/RendererPCH.cpp
)

add_library( Renderer STATIC
    ${HEADER_FILES}
    ${SOURCE_FILES}
)

target_include_directories( Renderer
    PUBLIC inc
)

target_link_libraries( Renderer 
)

# Enable precompiled header files.
set_source_files_properties( ${SOURCE_FILES} 
    PROPERTIES 
        COMPILE_FLAGS /Yu"RendererPCH.h"
)

set_source_files_properties( src/RendererPCH.cpp
    PROPERTIES
        COMPILE_FLAGS /Yc"RendererPCH.h"
)

