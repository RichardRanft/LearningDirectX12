cmake_minimum_required( VERSION 3.12.1 ) # Latest version of CMake when this file was created.

set( HEADER_FILES
    inc/Camera.h
    inc/Tutorial5.h
)

set( SRC_FILES
    src/Camera.cpp
    src/Tutorial5.cpp
    src/main.cpp
)

add_executable( Tutorial5 WIN32
    ${HEADER_FILES} 
    ${SRC_FILES}
)

target_include_directories( Tutorial5
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries( Tutorial5
    GameFramework
    DX12Lib
)

# Set Local Debugger Settings (Command Arguments and Environment Variables)
set( COMMAND_ARGUMENTS "-wd \"${CMAKE_SOURCE_DIR}\"" )
configure_file( Tutorial5.vcxproj.user.in ${CMAKE_CURRENT_BINARY_DIR}/Tutorial5.vcxproj.user @ONLY )
