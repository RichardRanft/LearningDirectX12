#include <DX12LibPCH.h>

#include <Buffer.h>

Buffer::Buffer(Device& device, const std::wstring& name)
    : Resource(device, name)
{}

Buffer::Buffer(Device& device, const D3D12_RESOURCE_DESC& resDesc,
    size_t numElements, size_t elementSize,
    const std::wstring& name )
    : Resource(device, resDesc, nullptr, name)
{}
