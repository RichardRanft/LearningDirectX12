cmake_minimum_required( VERSION 3.17.0 ) # Latest version of CMake when this file was created.

set( HEADER_FILES 
    inc/PlatformPCH.h
    inc/IWindow.h
)

set( SOURCE_FILES
    src/PlatformPCH.cpp
)

add_library( Platform STATIC
    ${HEADER_FILES}
    ${SOURCE_FILES}
)

target_include_directories( Platform
    PUBLIC inc
)
