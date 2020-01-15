#include <DX12LibPCH.h>

#include <Buffer.h>

Buffer::Buffer()
: Resource()
{}

Buffer::Buffer(const std::wstring& name)
    : Resource(name)
{}

Buffer::Buffer(const D3D12_RESOURCE_DESC& desc,
    size_t numElements, size_t elementSize,
    const std::wstring& name )
    : Resource(desc, nullptr, name)
{}
