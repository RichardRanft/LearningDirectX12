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
 *  @file Window.h
 *  @date October 24, 2018
 *  @author Jeremiah van Oosten
 *
 *  @brief A window for our application.
 */

#include <Events.h>
#include <HighResolutionClock.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <memory>
#include <string>

class Game;
class Texture;

class Window : public std::enable_shared_from_this<Window>
{
public:
    /**
    * Get a handle to this window's instance.
    * @returns The handle to the window instance or nullptr if this is not a valid window.
    */
    HWND GetWindowHandle() const;

	/**
	 * Get the current DPI scaling for this window.
	 */
	float GetDPIScaling() const;

    /**
     * Initialize the window.
     */
    void Initialize();

    /**
    * Destroy this window.
    */
    void Destroy();

    const std::wstring& GetWindowName() const;

    int GetClientWidth() const;
    int GetClientHeight() const;

    /**
    * Is this a windowed window or full-screen?
    */
    bool IsFullScreen() const;

    // Set the fullscreen state of the window.
    void SetFullscreen(bool fullscreen);
    void ToggleFullscreen();

    /**
     * Show this window.
     */
    void Show();

    /**
     * Hide the window.
     */
    void Hide();

protected:
    // The Window procedure needs to call protected methods of this class.
    friend LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

    // Only the application can create a window.
    friend class Application;
    // The Game class needs to register itself with a window.
    friend class Game;

    Window() = delete;
    Window(HWND hWnd, const std::wstring& windowName, int clientWidth, int clientHeight);
    virtual ~Window();

    // Register a Game with this window. This allows
    // the window to callback functions in the Game class.
    void RegisterCallbacks( std::shared_ptr<Game> pGame );

    // Update and Draw can only be called by the application.
    virtual void OnUpdate(UpdateEventArgs& e);
    virtual void OnRender(RenderEventArgs& e);

    // A keyboard key was pressed
    virtual void OnKeyPressed(KeyEventArgs& e);
    // A keyboard key was released
    virtual void OnKeyReleased(KeyEventArgs& e);

    // The mouse was moved
    virtual void OnMouseMoved(MouseMotionEventArgs& e);
    // A button on the mouse was pressed
    virtual void OnMouseButtonPressed(MouseButtonEventArgs& e);
    // A button on the mouse was released
    virtual void OnMouseButtonReleased(MouseButtonEventArgs& e);
    // The mouse wheel was moved.
    virtual void OnMouseWheel(MouseWheelEventArgs& e);

    // The window was resized.
    virtual void OnResize(ResizeEventArgs& e);

	// The DPI scaling has changed.
	virtual void OnDPIScaleChanged(DPIScaleEventArgs& e);

	// Update DPI scaling (can only be called from WndProc)
	void SetDPIScaling(float dpiScaling);

private:
    // Windows should not be copied.
    Window(const Window& copy) = delete;
    Window& operator=(const Window& other) = delete;

    HWND m_hWnd;

    std::wstring m_WindowName;
    
    int m_ClientWidth;
    int m_ClientHeight;
    bool m_Fullscreen;

    HighResolutionClock m_UpdateClock;
    HighResolutionClock m_RenderClock;

    std::weak_ptr<Game> m_pGame;

    RECT m_WindowRect;

    int m_PreviousMouseX;
    int m_PreviousMouseY;

    // Per-window DPI scaling.
    float m_DPIScaling;
};