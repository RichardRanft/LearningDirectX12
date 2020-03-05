cmake_minimum_required( VERSION 3.17.0 ) # Latest version of CMake when this file was created.

set( HEADER_FILES 
    inc/Connection.h
    inc/ConnectionBody.h
    inc/Events.h
    inc/GarbageCollectingLock.h
    inc/KeyCodes.h
    inc/NonCopyable.h
)

set( SOURCE_FILES
    src/Events.cpp
)

add_library( Events STATIC
    ${HEADER_FILES}
    ${SOURCE_FILES}
)

target_include_directories( Events
    PUBLIC inc
)
