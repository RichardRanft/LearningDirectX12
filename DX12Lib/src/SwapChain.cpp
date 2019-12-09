#include <DX12LibPCH.h>

#include <Device.h>

SwapChain::SwapChain(HWND hWnd, std::shared_ptr<Device> device )
{
    m_IsTearingSupported = device->IsTearingSupported();

    for (int i = 0; i < BufferCount; ++i)
    {
        m_BackBufferTextures[i].SetName(L"Backbuffer[" + std::to_wstring(i) + L"]");
    }

    m_dxgiSwapChain = CreateSwapChain();
    UpdateRenderTargetViews();

}