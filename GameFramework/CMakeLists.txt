cmake_minimum_required( VERSION 3.12.1 ) # Latest version of CMake when this file was created.

set( HEADER_FILES 
    inc/GameFrameworkPCH.h
    inc/Application.h
    inc/Events.h
    inc/Game.h
    inc/HighResolutionClock.h
    inc/KeyCodes.h
    inc/Window.h
)

set( SOURCE_FILES
    src/GameFrameworkPCH.cpp
    src/Application.cpp
    src/Game.cpp
    src/HighResolutionClock.cpp
    src/Window.cpp
)

set( RESOURCE_FILES 
    GameFramework.rc
    Resources/Icon/app_icon.ico
    ${SHADER_FILES}
)

source_group( "Resources" FILES GameFramework.rc resource.h )
source_group( "Resources\\Icon" FILES Resources/Icon/app_icon.ico )

add_library( GameFramework STATIC
    ${HEADER_FILES}
    ${SOURCE_FILES}
    ${RESOURCE_FILES}
)

target_include_directories( GameFramework
    PUBLIC inc
)



