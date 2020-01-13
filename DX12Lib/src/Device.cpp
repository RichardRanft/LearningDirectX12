#include <DX12LibPCH.h>

#include <Device.h>

#include <CommandQueue.h>
#include <DescriptorAllocator.h>

std::atomic_uint64_t Device::ms_FrameCounter = 0ull;

// Allow std::make_shared access to constructor.
struct DeviceCtor : public Device {
    DeviceCtor(uint32_t nodeMask)
        : Device(nodeMask)
    {}
};

// Allow std::make_shared access to constructor.
struct DescriptorAllocatorCtor : public DescriptorAllocator
{
    DescriptorAllocatorCtor(std::shared_ptr<Device> device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptorsPerHeap = 256)
        : DescriptorAllocator(device, type, numDescriptorsPerHeap)
    {}
};

// Allow std::make_shared access to constructor.
struct CommandQueueCtor : public CommandQueue
{
    CommandQueueCtor(std::shared_ptr<Device> device, D3D12_COMMAND_LIST_TYPE type)
        : CommandQueue(device, type)
    {}
};



Device::Device(uint32_t nodeMask)
    : m_NodeCount(0)
    , m_NodeMask(nodeMask)
    , m_RootSignatureFeatureData({ D3D_ROOT_SIGNATURE_VERSION_1_1 })
{
    // Check for DirectX Math library support.
    if (!DirectX::XMVerifyCPUSupport())
    {
        MessageBoxA(NULL, "Failed to verify DirectX Math library support.", "Error", MB_OK | MB_ICONERROR);
        exit(1);
    }

#if defined(_DEBUG)
    // Always enable the debug layer before doing anything DX12 related
    // so all possible errors generated while creating DX12 objects
    // are caught by the debug layer.
    ComPtr<ID3D12Debug1> debugInterface;
    ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)));
    debugInterface->EnableDebugLayer();
    // Enable these if you want full validation (will slow down rendering a lot).
    //debugInterface->SetEnableGPUBasedValidation(TRUE);
    //debugInterface->SetEnableSynchronizedCommandQueueValidation(TRUE);
#endif

    auto dxgiAdapter = GetAdapter(false);
    if (!dxgiAdapter)
    {
        // If no supporting DX12 adapters exist, fall back to WARP
        dxgiAdapter = GetAdapter(true);
    }

    if (dxgiAdapter)
    {
        m_d3d12Device = CreateDX12Device(dxgiAdapter);
        if (m_d3d12Device)
        {
            m_NodeCount = std::min(m_d3d12Device->GetNodeCount(), MaxNodeCount);
        }
        else
        {
            throw std::exception("Failed to create D3D12 Device.");
        }
    }
    else
    {
        throw std::exception("DXGI adapter enumeration failed.");
    }

    if (FAILED(m_d3d12Device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &m_RootSignatureFeatureData, sizeof(m_RootSignatureFeatureData))))
    {
        m_RootSignatureFeatureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
    }
}

void Device::Init()
{
    m_DirectCommandQueue = std::make_shared<CommandQueueCtor>(shared_from_this(), D3D12_COMMAND_LIST_TYPE_DIRECT);
    m_ComputeCommandQueue = std::make_shared<CommandQueueCtor>(shared_from_this(), D3D12_COMMAND_LIST_TYPE_COMPUTE);
    m_CopyCommandQueue = std::make_shared<CommandQueueCtor>(shared_from_this(), D3D12_COMMAND_LIST_TYPE_COPY);

    // Create descriptor allocators
    for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
    {
        m_DescriptorAllocators[i] = std::make_unique<DescriptorAllocatorCtor>(shared_from_this(), static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(i));
    }
}

std::shared_ptr<Device> Device::CreateDevice(uint32_t nodeMask)
{
    std::shared_ptr<Device> device = std::make_shared<DeviceCtor>(nodeMask);
    device->Init();

    return device;
}

Microsoft::WRL::ComPtr<IDXGIAdapter4> Device::GetAdapter(bool bUseWarp)
{
    ComPtr<IDXGIFactory6> dxgiFactory;
    UINT createFactoryFlags = 0;
#if defined(_DEBUG)
    createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

    ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory)));

    ComPtr<IDXGIAdapter1> dxgiAdapter1;
    ComPtr<IDXGIAdapter4> dxgiAdapter4;

    if (bUseWarp)
    {
        ThrowIfFailed(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&dxgiAdapter1)));
        ThrowIfFailed(dxgiAdapter1.As(&dxgiAdapter4));
    }
    else
    {
        for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&dxgiAdapter1)) != DXGI_ERROR_NOT_FOUND; ++i)
        {
            DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
            dxgiAdapter1->GetDesc1(&dxgiAdapterDesc1);

            // Check to see if the adapter can create a D3D12 device without actually 
            // creating it.
            if ((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
                SUCCEEDED(D3D12CreateDevice(dxgiAdapter1.Get(),
                    D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)))
            {
                ThrowIfFailed(dxgiAdapter1.As(&dxgiAdapter4));
            }
        }
    }

    return dxgiAdapter4;
}

Microsoft::WRL::ComPtr<CD3DX12AffinityDevice> Device::CreateDX12Device(Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter)
{
    ComPtr<ID3D12Device6> d3d12Device6;
    ComPtr<CD3DX12AffinityDevice> affinityDevice;
    ThrowIfFailed(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3d12Device6)));
    ThrowIfFailed(D3DX12AffinityCreateLDADevice(d3d12Device6.Get(), &affinityDevice));
    //    NAME_D3D12_OBJECT(d3d12Device2);

        // Enable debug messages in debug mode.
#if defined(_DEBUG)
    ComPtr<ID3D12InfoQueue> pInfoQueue;
    if (SUCCEEDED(d3d12Device6.As(&pInfoQueue)))
    {
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

        // Suppress whole categories of messages
        //D3D12_MESSAGE_CATEGORY Categories[] = {};

        // Suppress messages based on their severity level
        D3D12_MESSAGE_SEVERITY Severities[] =
        {
            D3D12_MESSAGE_SEVERITY_INFO
        };

        // Suppress individual messages by their ID
        D3D12_MESSAGE_ID DenyIds[] = {
            D3D12_MESSAGE_ID_COPY_DESCRIPTORS_INVALID_RANGES,               // This started happening after updating to an RTX 2080 Ti. I believe this to be an error in the validation layer itself.
            D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,   // I'm really not sure how to avoid this message.
            D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,                         // This warning occurs when using capture frame while graphics debugging.
            D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,                       // This warning occurs when using capture frame while graphics debugging.
        };

        D3D12_INFO_QUEUE_FILTER NewFilter = {};
        //NewFilter.DenyList.NumCategories = _countof(Categories);
        //NewFilter.DenyList.pCategoryList = Categories;
        NewFilter.DenyList.NumSeverities = _countof(Severities);
        NewFilter.DenyList.pSeverityList = Severities;
        NewFilter.DenyList.NumIDs = _countof(DenyIds);
        NewFilter.DenyList.pIDList = DenyIds;

        ThrowIfFailed(pInfoQueue->PushStorageFilter(&NewFilter));
    }
#endif

    return affinityDevice;
}

DXGI_SAMPLE_DESC Device::GetMultisampleQualityLevels(DXGI_FORMAT format, UINT numSamples, D3D12_MULTISAMPLE_QUALITY_LEVEL_FLAGS flags) const
{
    DXGI_SAMPLE_DESC sampleDesc = { 1, 0 };

    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS qualityLevels;
    qualityLevels.Format = format;
    qualityLevels.SampleCount = 1;
    qualityLevels.Flags = flags;
    qualityLevels.NumQualityLevels = 0;

    while (qualityLevels.SampleCount <= numSamples && SUCCEEDED(m_d3d12Device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &qualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS))) && qualityLevels.NumQualityLevels > 0)
    {
        // That works...
        sampleDesc.Count = qualityLevels.SampleCount;
        sampleDesc.Quality = qualityLevels.NumQualityLevels - 1;

        // But can we do better?
        qualityLevels.SampleCount *= 2;
    }

    return sampleDesc;
}

Microsoft::WRL::ComPtr<CD3DX12AffinityDevice> Device::GetD3D12Device() const
{
    return m_d3d12Device;
}

std::shared_ptr<CommandQueue> Device::GetCommandQueue(D3D12_COMMAND_LIST_TYPE type) const
{
    std::shared_ptr<CommandQueue> commandQueue;
    switch (type)
    {
    case D3D12_COMMAND_LIST_TYPE_DIRECT:
        commandQueue = m_DirectCommandQueue;
        break;
    case D3D12_COMMAND_LIST_TYPE_COMPUTE:
        commandQueue = m_ComputeCommandQueue;
        break;
    case D3D12_COMMAND_LIST_TYPE_COPY:
        commandQueue = m_CopyCommandQueue;
        break;
    default:
        assert(false && "Invalid command queue type.");
    }

    return commandQueue;
}

void Device::Flush()
{
    m_CopyCommandQueue->Flush();
    m_ComputeCommandQueue->Flush();
    m_DirectCommandQueue->Flush();
}

DescriptorAllocation Device::AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors)
{
    return m_DescriptorAllocators[type]->Allocate(numDescriptors);
}

void Device::ReleaseStaleDescriptors(uint64_t finishedFrame)
{
    for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
    {
        m_DescriptorAllocators[i]->ReleaseStaleDescriptors(finishedFrame);
    }
}

UINT Device::GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE type) const
{
    return m_d3d12Device->GetDescriptorHandleIncrementSize(type);
}

SwapChain Device::CreateSwapChain(HWND hWnd)
{
    return SwapChain(shared_from_this(), hWnd);
}

RootSignature Device::CreateRootSignature(const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDesc)
{
    return RootSignature(shared_from_this(), rootSignatureDesc, m_RootSignatureFeatureData.HighestVersion);
}

Texture Device::CreateTexture(const D3D12_RESOURCE_DESC& desc,
    const D3D12_CLEAR_VALUE* clearValue,
    TextureUsage textureUsage,
    const std::wstring& name)
{
    return Texture(shared_from_this(), desc, clearValue, textureUsage, name);
}

Texture Device::CreateTexture(Microsoft::WRL::ComPtr<CD3DX12AffinityResource> resource,
    TextureUsage textureUsage,
    const std::wstring& name)
{
    return Texture(shared_from_this(), resource, textureUsage, name);
}
