#pragma once

/*
 *  Copyright(c) 2018 Jeremiah van Oosten
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
 *  @file GUI.h
 *  @date October 24, 2018
 *  @author Jeremiah van Oosten
 *
 *  @brief Wrapper for ImGui. This class is used internally by the Window class.
 */

#include "imgui.h"

#include <RootSignature.h>
#include <Texture.h>

#include <d3dx12.h>
#include <wrl.h>

class CommandList;
class Device;
class RenderTarget;

class GUI
{
public:
    GUI(HWND hWnd = NULL);
    virtual ~GUI();
        
    // Moves are allowed.
    GUI(GUI&& copy);
    GUI& operator=(GUI&& copy) noexcept;

    // Copying is not allowed (since the GUI owns the ImGuiContext)
    GUI(const GUI& copy) = delete;
    GUI& operator=(const GUI& copy) = delete;

    // Initialize the ImGui context.
    virtual bool Initialize( HWND window );

    // Begin a new frame.
    virtual void NewFrame();
    virtual void Render( std::shared_ptr<CommandList> commandList, const RenderTarget& renderTarget );

    // Destroy the ImGui context.
    virtual void Destroy();

	// Set the scaling for this ImGuiContext.
	void SetScaling(float scale);

private:
    HWND m_hWnd;
    ImGuiContext* m_pImGuiCtx;
    Texture m_FontTexture;
    RootSignature m_RootSignature;
    Microsoft::WRL::ComPtr<CD3DX12AffinityPipelineState> m_PipelineState;
};