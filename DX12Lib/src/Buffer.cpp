#include <DX12LibPCH.h>

#include <Buffer.h>

Buffer::Buffer()
: Resource()
{}

Buffer::Buffer(std::shared_ptr<Device> device, const std::wstring& name)
    : Resource(device, name)
{}

Buffer::Buffer(std::shared_ptr<Device> device, const D3D12_RESOURCE_DESC& desc,
    size_t numElements, size_t elementSize,
    const std::wstring& name )
    : Resource(device, desc, nullptr, name)
{}
