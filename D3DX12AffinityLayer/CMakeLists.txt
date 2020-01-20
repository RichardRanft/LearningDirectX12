cmake_minimum_required( VERSION 3.16.1 ) # Latest version of CMake when this file was created.

set( HEADER_FILES 
    inc/CD3DX12AffinityCommandAllocator.h
    inc/CD3DX12AffinityCommandList.h
    inc/CD3DX12AffinityCommandQueue.h
    inc/CD3DX12AffinityCommandSignature.h
    inc/CD3DX12AffinityDescriptorHeap.h
    inc/CD3DX12AffinityDevice.h
    inc/CD3DX12AffinityDeviceChild.h
    inc/CD3DX12AffinityFence.h
    inc/CD3DX12AffinityGraphicsCommandList.h
    inc/CD3DX12AffinityHeap.h
    inc/CD3DX12AffinityObject.h
    inc/CD3DX12AffinityPageable.h
    inc/CD3DX12AffinityPipelineState.h
    inc/CD3DX12AffinityQueryHeap.h
    inc/CD3DX12AffinityResource.h
    inc/CD3DX12AffinityRootSignature.h
    inc/CDXGIAffinitySwapChain.h
    inc/d3dx12.h
    inc/d3dx12affinity.h
    inc/d3dx12affinity_d3dx12.h
    inc/d3dx12affinity_structs.h
    inc/Utils.h
)

set( SOURCE_FILES
    src/CD3DX12AffinityCommandAllocator.cpp
    src/CD3DX12AffinityCommandList.cpp
    src/CD3DX12AffinityCommandQueue.cpp
    src/CD3DX12AffinityCommandSignature.cpp
    src/CD3DX12AffinityDescriptorHeap.cpp
    src/CD3DX12AffinityDevice.cpp
    src/CD3DX12AffinityDeviceChild.cpp
    src/CD3DX12AffinityFence.cpp
    src/CD3DX12AffinityGraphicsCommandList.cpp
    src/CD3DX12AffinityHeap.cpp
    src/CD3DX12AffinityObject.cpp
    src/CD3DX12AffinityPageable.cpp
    src/CD3DX12AffinityPipelineState.cpp
    src/CD3DX12AffinityQueryHeap.cpp
    src/CD3DX12AffinityResource.cpp
    src/CD3DX12AffinityRootSignature.cpp
    src/CDXGIAffinitySwapChain.cpp
    src/d3dx12affinity.cpp
    src/D3DX12AffinityCreateMultiDevice.cpp
    src/DXGIXAffinityCreateLDASwapChain.cpp
    src/DXGIXAffinityCreateSingleWindowSwapChain.cpp
    src/Utils.cpp
)

add_library( D3DX12AffinityLayer STATIC
    ${HEADER_FILES}
    ${SOURCE_FILES}
)

target_include_directories( D3DX12AffinityLayer
    PUBLIC inc
)

target_link_libraries( D3DX12AffinityLayer
    PUBLIC d3d12.lib
    PUBLIC dxgi.lib
    PUBLIC dxguid.lib
)

