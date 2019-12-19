#pragma once

/*
 *  Copyright(c) 2019 Jeremiah van Oosten
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files(the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions :
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 */

 /**
  *  @file Device.h
  *  @date December 9, 2019
  *  @author Jeremiah van Oosten
  *
  *  @brief The Device class is used abstract the functionality of the D3D12Devcie class.
  */

#include <DescriptorAllocation.h>
#include <memory>
#include <d3dx12.h>
#include <dxgi1_6.h>
#include <wrl.h>

class CommandQueue;
class DescriptorAllocator;

class Device : public std::enable_shared_from_this<Device>
{
public:
    /**
     * Create the Graphics device object. This is used to create all device dependent resources.
     */
    static std::shared_ptr<Device> CreateDevice();
    
    /**
     * Get the number of GPU nodes in the SLI configuration.
     * @see https://docs.microsoft.com/en-us/windows/win32/direct3d12/multi-engine
     */
    inline uint32_t GetNodeCount()
    {
        return m_NodeCount;
    }

    /**
     * Check if the requested multisample quality is supported for the given format.
     */
    DXGI_SAMPLE_DESC GetMultisampleQualityLevels(DXGI_FORMAT format, UINT numSamples, D3D12_MULTISAMPLE_QUALITY_LEVEL_FLAGS flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE) const;

    /**
     * Get the Direct3D 12 device
     */
    Microsoft::WRL::ComPtr<ID3D12Device6> GetD3D12Device() const;

    /**
     * Get a command queue. Valid types are:
     * - D3D12_COMMAND_LIST_TYPE_DIRECT : Can be used for draw, dispatch, or copy commands.
     * - D3D12_COMMAND_LIST_TYPE_COMPUTE: Can be used for dispatch or copy commands.
     * - D3D12_COMMAND_LIST_TYPE_COPY   : Can be used for copy commands.
     */
    std::shared_ptr<CommandQueue> GetCommandQueue(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT, uint32_t nodeIndex = 0) const;

    /**
     * Flush all command queues.
     */
    void Flush();

    /**
     * Allocate a number of CPU visible descriptors.
     */
    DescriptorAllocation AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors = 1);

    /**
     * Release stale descriptors. This should only be called with a completed frame counter.
     */
    void ReleaseStaleDescriptors(uint64_t finishedFrame);

    UINT GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE type) const;

    /**
     * Increment the frame counter and return the previous frame count.
     */
    static uint64_t IncrementFrameCounter()
    {
        return ms_FrameCounter++;
    }

    /**
     * Get the current frame counter value.
     */
    static uint64_t GetFrameCounter()
    {
        return ms_FrameCounter;
    }

    /**
     * Reset the frame counter to 0.
     */
    static void ResetFrameCounter()
    {
        ms_FrameCounter = 0ull;
    }

protected:
    Device();

    void Init();

    Microsoft::WRL::ComPtr<IDXGIAdapter4> GetAdapter(bool bUseWarp);
    Microsoft::WRL::ComPtr<ID3D12Device6> CreateDevice(Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter);
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(UINT numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t nodeIndex );

private:
    static const uint32_t MaxNodeCount = 2;

    Microsoft::WRL::ComPtr<ID3D12Device6> m_d3d12Device;

    std::shared_ptr<CommandQueue> m_DirectCommandQueue[MaxNodeCount];
    std::shared_ptr<CommandQueue> m_ComputeCommandQueue[MaxNodeCount];
    std::shared_ptr<CommandQueue> m_CopyCommandQueue[MaxNodeCount];

    std::unique_ptr<DescriptorAllocator> m_DescriptorAllocators[MaxNodeCount][D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

    uint32_t m_NodeCount;

    // The frame counter is used for safely releasing dynamic descriptors.
    static std::atomic_uint64_t ms_FrameCounter;
};
