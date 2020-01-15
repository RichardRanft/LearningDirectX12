#include <GameFrameworkPCH.h>

#include <Application.h>
#include <Game.h>
#include <Window.h>

Game::Game( const std::wstring& name, int width, int height)
    : m_Name( name )
    , m_Width( width )
    , m_Height( height )
{
}

Game::~Game()
{
    Destroy();
}

HWND Game::GetWindowHandle() const
{
    HWND hWnd = NULL;
    if (m_pWindow)
    {
        hWnd = m_pWindow->GetWindowHandle();
    }

    return hWnd;
}

bool Game::Initialize()
{
    m_pWindow = Application::Get().CreateGameWindow(m_Name, m_Width, m_Height);
    m_pWindow->RegisterCallbacks(shared_from_this());

    return true;
}

bool Game::LoadContent()
{
    return true;
}

/**
 *  Unload demo specific content that was loaded in LoadContent.
 */
void Game::UnloadContent()
{

}

void Game::Destroy()
{
    if ( m_pWindow )
    {
        Application::Get().DestroyWindow(m_pWindow);
        m_pWindow.reset();
    }
}

void Game::Show()
{
    if (m_pWindow)
    {
        m_pWindow->Show();
    }
}

void Game::Hide()
{
    if (m_pWindow)
    {
        m_pWindow->Hide();
    }
}

void Game::ToggleFullscreen()
{
    if (m_pWindow)
    {
        m_pWindow->ToggleFullscreen();
    }
}


void Game::OnUpdate(UpdateEventArgs& e)
{
}

void Game::OnRender(RenderEventArgs& e)
{

}

void Game::OnKeyPressed(KeyEventArgs& e)
{
    // By default, do nothing.
}

void Game::OnKeyReleased(KeyEventArgs& e)
{
    // By default, do nothing.
}

void Game::OnMouseMoved(class MouseMotionEventArgs& e)
{
    // By default, do nothing.
}

void Game::OnMouseButtonPressed(MouseButtonEventArgs& e)
{
    // By default, do nothing.
}

void Game::OnMouseButtonReleased(MouseButtonEventArgs& e)
{
    // By default, do nothing.
}

void Game::OnMouseWheel(MouseWheelEventArgs& e)
{
    // By default, do nothing.
}

void Game::OnResize(ResizeEventArgs& e)
{
    m_Width = e.Width;
    m_Height = e.Height;
}

void Game::OnDPIScaleChanged(DPIScaleEventArgs& e)
{
	// By default, do nothing.
}

void Game::OnWindowDestroy()
{
    // If the Window which we are registered to is 
    // destroyed, then any resources which are associated 
    // to the window must be released.
    UnloadContent();
}

