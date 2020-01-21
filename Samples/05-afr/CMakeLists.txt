cmake_minimum_required( VERSION 3.12.1 ) # Latest version of CMake when this file was created.

set( SRC_FILES
    src/main.cpp
)

set( HEADER_FILES
    inc/Helpers.h
    resource.h
)

set( RESOURCE_FILES
    Resources/Icon/app_icon.ico
)

set( VERTEX_SHADER_FILES
    Resources/Shaders/VertexShader.hlsl
)

set( PIXEL_SHADER_FILES
    Resources/Shaders/PixelShader.hlsl
)

set( SHADER_FILES
    ${VERTEX_SHADER_FILES}
    ${PIXEL_SHADER_FILES}
)

source_group( "Resource Files" FILES 05-afr.rc resource.h )
source_group( "Resource Files\\Icon" FILES Resources/Icon/app_icon.ico )
source_group( "Resource Files\\Shaders" FILES ${SHADER_FILES} )

set_source_files_properties( ${SHADER_FILES} 
    PROPERTIES
        VS_SHADER_OBJECT_FILE_NAME "${CMAKE_SOURCE_DIR}/Samples/Data/Shaders/05-afr/%(Filename).cso"
        VS_SHADER_MODEL 5.1
        VS_SHADER_DISABLE_OPTIMIZATIONS $<$<CONFIG:Debug>:ON>
        VS_SHADER_ENABLE_DEBUG $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:ON>
)

set_source_files_properties( ${VERTEX_SHADER_FILES}
    PROPERTIES 
        VS_SHADER_TYPE Vertex
)

set_source_files_properties( ${PIXEL_SHADER_FILES}
    PROPERTIES 
        VS_SHADER_TYPE Pixel
)

add_executable( 05-afr WIN32
    ${HEADER_FILES}
    ${SRC_FILES}
    ${SHADER_FILES}
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
    D3DX12AffinityLayer
    D3DCompiler
)

# Set Local Debugger Settings (Command Arguments and Environment Variables)
set( COMMAND_ARGUMENTS "-wd \"${CMAKE_SOURCE_DIR}\"" )
configure_file( 05-afr.vcxproj.user.in ${CMAKE_CURRENT_BINARY_DIR}/05-afr.vcxproj.user @ONLY )
