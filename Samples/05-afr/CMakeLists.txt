cmake_minimum_required( VERSION 3.12.1 ) # Latest version of CMake when this file was created.

set( SRC_FILES
    src/main.cpp
)

set( HEADER_FILES
    resource.h
)

set( RESOURCE_FILES
    Resources/Icon/app_icon.ico
)

source_group( "Resource Files\\Icon" FILES Resources/Icon/app_icon.ico )

add_executable( 05-afr WIN32
    ${HEADER_FILES}
    ${SRC_FILES}
    ${RESOURCE_FILES}
    05-afr.rc
)

target_include_directories( 05-afr
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_compile_definitions( 05-afr
    PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX
)

target_link_libraries( 05-afr
    PUBLIC D3DX12AffinityLayer
)

# Set Local Debugger Settings (Command Arguments and Environment Variables)
set( COMMAND_ARGUMENTS "-wd \"${CMAKE_SOURCE_DIR}\"" )
configure_file( 05-afr.vcxproj.user.in ${CMAKE_CURRENT_BINARY_DIR}/05-afr.vcxproj.user @ONLY )
