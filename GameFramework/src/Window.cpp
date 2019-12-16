#include <GameFrameworkPCH.h>

#include <Window.h>

#include <Application.h>
#include <Game.h>

#include <algorithm>

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

Window::Window(HWND hWnd, const std::wstring& windowName, int clientWidth, int clientHeight)
    : m_hWnd(hWnd)
    , m_WindowName(windowName)
    , m_ClientWidth(clientWidth)
    , m_ClientHeight(clientHeight)
    , m_Fullscreen(false)
{
	m_DPIScaling = GetDpiForWindow(hWnd) / 96.0f;
}

Window::~Window()
{
    // Window should be destroyed with Application::DestroyWindow before
    // the window goes out of scope.
    assert(!m_hWnd && "Use Application::DestroyWindow before destruction.");
}

void Window::Initialize()
{}


HWND Window::GetWindowHandle() const
{
    return m_hWnd;
}

float Window::GetDPIScaling() const
{
	return m_DPIScaling;
}

const std::wstring& Window::GetWindowName() const
{
    return m_WindowName;
}

void Window::Show()
{
    ::ShowWindow(m_hWnd, SW_SHOW);
}

/**
* Hide the window.
*/
void Window::Hide()
{
    ::ShowWindow(m_hWnd, SW_HIDE);
}

void Window::Destroy()
{
    if (auto pGame = m_pGame.lock())
    {
        // Notify the registered game that the window is being destroyed.
        pGame->OnWindowDestroy();
    }

    if (m_hWnd)
    {
        DestroyWindow(m_hWnd);
        m_hWnd = nullptr;
    }
}

int Window::GetClientWidth() const
{
    return m_ClientWidth;
}

int Window::GetClientHeight() const
{
    return m_ClientHeight;
}

bool Window::IsFullScreen() const
{
    return m_Fullscreen;
}

// Set the fullscreen state of the window.
void Window::SetFullscreen(bool fullscreen)
{
    if (m_Fullscreen != fullscreen)
    {
        m_Fullscreen = fullscreen;

        if (m_Fullscreen) // Switching to fullscreen.
        {
            // Store the current window dimensions so they can be restored 
            // when switching out of fullscreen state.
            ::GetWindowRect(m_hWnd, &m_WindowRect);

            // Set the window style to a borderless window so the client area fills
            // the entire screen.
            UINT windowStyle = WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

            ::SetWindowLongW(m_hWnd, GWL_STYLE, windowStyle);

            // Query the name of the nearest display device for the window.
            // This is required to set the fullscreen dimensions of the window
            // when using a multi-monitor setup.
            HMONITOR hMonitor = ::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
            MONITORINFOEX monitorInfo = {};
            monitorInfo.cbSize = sizeof(MONITORINFOEX);
            ::GetMonitorInfo(hMonitor, &monitorInfo);

            ::SetWindowPos(m_hWnd, HWND_TOP,
                monitorInfo.rcMonitor.left,
                monitorInfo.rcMonitor.top,
                monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
                monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
                SWP_FRAMECHANGED | SWP_NOACTIVATE);

            ::ShowWindow(m_hWnd, SW_MAXIMIZE);
        }
        else
        {
            // Restore all the window decorators.
            ::SetWindowLong(m_hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);

            ::SetWindowPos(m_hWnd, HWND_NOTOPMOST,
                m_WindowRect.left,
                m_WindowRect.top,
                m_WindowRect.right - m_WindowRect.left,
                m_WindowRect.bottom - m_WindowRect.top,
                SWP_FRAMECHANGED | SWP_NOACTIVATE);

            ::ShowWindow(m_hWnd, SW_NORMAL);
        }
    }
}

void Window::ToggleFullscreen()
{
    SetFullscreen(!m_Fullscreen);
}


void Window::RegisterCallbacks(std::shared_ptr<Game> pGame)
{
    m_pGame = pGame;

    return;
}

void Window::OnUpdate(UpdateEventArgs& e)
{
    m_UpdateClock.Tick();

    if (auto pGame = m_pGame.lock())
    {
        UpdateEventArgs updateEventArgs(m_UpdateClock.GetDeltaSeconds(), m_UpdateClock.GetTotalSeconds(), e.FrameNumber);
        pGame->OnUpdate(updateEventArgs);
    }
}

void Window::OnRender(RenderEventArgs& e)
{
    m_RenderClock.Tick();

    if (auto pGame = m_pGame.lock())
    {
        RenderEventArgs renderEventArgs(m_RenderClock.GetDeltaSeconds(), m_RenderClock.GetTotalSeconds(), e.FrameNumber);
        pGame->OnRender(renderEventArgs);
    }
}

void Window::OnKeyPressed(KeyEventArgs& e)
{
    if (auto pGame = m_pGame.lock())
    {
        pGame->OnKeyPressed(e);
    }
}

void Window::OnKeyReleased(KeyEventArgs& e)
{
    if (auto pGame = m_pGame.lock())
    {
        pGame->OnKeyReleased(e);
    }
}

// The mouse was moved
void Window::OnMouseMoved(MouseMotionEventArgs& e)
{
    e.RelX = e.X - m_PreviousMouseX;
    e.RelY = e.Y - m_PreviousMouseY;

    m_PreviousMouseX = e.X;
    m_PreviousMouseY = e.Y;

    if (auto pGame = m_pGame.lock())
    {
        pGame->OnMouseMoved(e);
    }
}

// A button on the mouse was pressed
void Window::OnMouseButtonPressed(MouseButtonEventArgs& e)
{
    m_PreviousMouseX = e.X;
    m_PreviousMouseY = e.Y;

    if (auto pGame = m_pGame.lock())
    {
        pGame->OnMouseButtonPressed(e);
    }
}

// A button on the mouse was released
void Window::OnMouseButtonReleased(MouseButtonEventArgs& e)
{
    if (auto pGame = m_pGame.lock())
    {
        pGame->OnMouseButtonReleased(e);
    }
}

// The mouse wheel was moved.
void Window::OnMouseWheel(MouseWheelEventArgs& e)
{
    if (auto pGame = m_pGame.lock())
    {
        pGame->OnMouseWheel(e);
    }
}

void Window::OnResize(ResizeEventArgs& e)
{
    // Update the client size.
    if (m_ClientWidth != e.Width || m_ClientHeight != e.Height)
    {
        m_ClientWidth = std::max(1, e.Width);
        m_ClientHeight = std::max(1, e.Height);
    }

    if (auto pGame = m_pGame.lock())
    {
        pGame->OnResize(e);
    }
}

void Window::OnDPIScaleChanged(DPIScaleEventArgs& e)
{
	if (auto pGame = m_pGame.lock())
	{
		pGame->OnDPIScaleChanged(e);
	}
}

void Window::SetDPIScaling(float dpiScaling)
{
	m_DPIScaling = dpiScaling;
}

