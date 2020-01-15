#include <DX12LibPCH.h>

#include <ByteAddressBuffer.h>
#include <Device.h>

ByteAddressBuffer::ByteAddressBuffer(const std::wstring& name)
    : Buffer(name)
	, m_BufferSize(0)
{
    auto& device = Device::Get();
    m_SRV = device.AllocateDescriptors( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );
    m_UAV = device.AllocateDescriptors( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );
}

ByteAddressBuffer::ByteAddressBuffer(
    const D3D12_RESOURCE_DESC& resDesc,
	size_t numElements, size_t elementSize,
	const std::wstring& name)
	: Buffer(resDesc, numElements, elementSize, name)
	, m_BufferSize(numElements * elementSize)
{}

void ByteAddressBuffer::CreateViews( size_t numElements, size_t elementSize )
{
    // Make sure buffer size is aligned to 4 bytes.
    m_BufferSize = Math::AlignUp( numElements * elementSize, 4 );

    auto d3d12Device = Device::Get().GetD3D12Device();

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Buffer.NumElements = (UINT)m_BufferSize / 4;
    srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;

    d3d12Device->CreateShaderResourceView( m_d3d12Resource.Get(), &srvDesc, m_SRV.GetDescriptorHandle() );

    auto desc = m_d3d12Resource->GetDesc();

    if (desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS)
    {
        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
        uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
        uavDesc.Buffer.NumElements = (UINT)m_BufferSize / 4;
        uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;

        d3d12Device->CreateUnorderedAccessView(m_d3d12Resource.Get(), nullptr, &uavDesc, m_UAV.GetDescriptorHandle());
    }
}
