#include <Tutorial5.h>

#include <Application.h>
#include <Window.h>

#include <wrl.h>
using namespace Microsoft::WRL;

using namespace DirectX;

#include <algorithm> // For std::min and std::max.
#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif

// Clamp a value between a min and max range.
template<typename T>
constexpr const T& clamp(const T& val, const T& min = T(0), const T& max = T(1))
{
    return val < min ? min : val > max ? max : val;
}

// Builds a look-at (world) matrix from a point, up and direction vectors.
XMMATRIX XM_CALLCONV LookAtMatrix(FXMVECTOR Position, FXMVECTOR Direction, FXMVECTOR Up)
{
    assert(!XMVector3Equal(Direction, XMVectorZero()));
    assert(!XMVector3IsInfinite(Direction));
    assert(!XMVector3Equal(Up, XMVectorZero()));
    assert(!XMVector3IsInfinite(Up));

    XMVECTOR R2 = XMVector3Normalize(Direction);

    XMVECTOR R0 = XMVector3Cross(Up, R2);
    R0 = XMVector3Normalize(R0);

    XMVECTOR R1 = XMVector3Cross(R2, R0);

    XMMATRIX M(R0, R1, R2, Position);

    return M;
}

Tutorial5::Tutorial5(const std::wstring& name, int width, int height, bool vSync)
    : super(name, width, height, vSync)
    , m_Forward(0)
    , m_Backward(0)
    , m_Left(0)
    , m_Right(0)
    , m_Up(0)
    , m_Down(0)
    , m_Pitch(0)
    , m_Yaw(0)
    , m_AnimateLights(false)
    , m_Shift(false)
    , m_Width(0)
    , m_Height(0)
    , m_RenderScale(1.0f)
{

    XMVECTOR cameraPos = XMVectorSet(0, 5, -20, 1);
    XMVECTOR cameraTarget = XMVectorSet(0, 5, 0, 1);
    XMVECTOR cameraUp = XMVectorSet(0, 1, 0, 0);

    m_Camera.set_LookAt(cameraPos, cameraTarget, cameraUp);
    m_Camera.set_Projection(45.0f, width / (float)height, 0.1f, 100.0f);

    m_pAlignedCameraData = (CameraData*)_aligned_malloc(sizeof(CameraData), 16);

    m_pAlignedCameraData->m_InitialCamPos = m_Camera.get_Translation();
    m_pAlignedCameraData->m_InitialCamRot = m_Camera.get_Rotation();
    m_pAlignedCameraData->m_InitialFov = m_Camera.get_FoV();
}

Tutorial5::~Tutorial5()
{
    _aligned_free(m_pAlignedCameraData);
}

bool Tutorial5::LoadContent()
{

    return true;
}

void Tutorial5::OnResize(ResizeEventArgs& e)
{
    super::OnResize(e);

    if (m_Width != e.Width || m_Height != e.Height)
    {
        m_Width = std::max(1, e.Width);
        m_Height = std::max(1, e.Height);

        float fov = m_Camera.get_FoV();
        float aspectRatio = m_Width / (float)m_Height;
        m_Camera.set_Projection(fov, aspectRatio, 0.1f, 100.0f);
    }
}

void Tutorial5::OnDPIScaleChanged(DPIScaleEventArgs& e)
{
//  ImGui::GetIO().FontGlobalScale = e.DPIScale;
}

void Tutorial5::UnloadContent()
{
}

static double g_FPS = 0.0;

void Tutorial5::OnUpdate(UpdateEventArgs& e)
{
    static uint64_t frameCount = 0;
    static double totalTime = 0.0;

    super::OnUpdate(e);

    totalTime += e.ElapsedTime;
    frameCount++;

    if (totalTime > 1.0)
    {
        g_FPS = frameCount / totalTime;

        char buffer[512];
        sprintf_s(buffer, "FPS: %f\n", g_FPS);
        OutputDebugStringA(buffer);

        frameCount = 0;
        totalTime = 0.0;
    }

    // Update the camera.
    float speedMultipler = (m_Shift ? 16.0f : 4.0f);

    XMVECTOR cameraTranslate = XMVectorSet(m_Right - m_Left, 0.0f, m_Forward - m_Backward, 1.0f) * speedMultipler * static_cast<float>(e.ElapsedTime);
    XMVECTOR cameraPan = XMVectorSet(0.0f, m_Up - m_Down, 0.0f, 1.0f) * speedMultipler * static_cast<float>(e.ElapsedTime);
    m_Camera.Translate(cameraTranslate, Space::Local);
    m_Camera.Translate(cameraPan, Space::Local);

    XMVECTOR cameraRotation = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(m_Pitch), XMConvertToRadians(m_Yaw), 0.0f);
    m_Camera.set_Rotation(cameraRotation);

}

void Tutorial5::OnGUI()
{
    //static bool showDemoWindow = false;
    //static bool showOptions = true;

    //if (ImGui::BeginMainMenuBar())
    //{
    //    if (ImGui::BeginMenu("File"))
    //    {
    //        if (ImGui::MenuItem("Exit", "Esc"))
    //        {
    //            Application::Get().Quit();
    //        }
    //        ImGui::EndMenu();
    //    }

    //    if (ImGui::BeginMenu("View"))
    //    {
    //        ImGui::MenuItem("ImGui Demo", nullptr, &showDemoWindow);
    //        ImGui::MenuItem("Tonemapping", nullptr, &showOptions);

    //        ImGui::EndMenu();
    //    }

    //    if (ImGui::BeginMenu("Options") )
    //    {
    //        bool vSync = m_pWindow->IsVSync();
    //        if (ImGui::MenuItem("V-Sync", "V", &vSync))
    //        {
    //            m_pWindow->SetVSync(vSync);
    //        }

    //        bool fullscreen = m_pWindow->IsFullScreen();
    //        if (ImGui::MenuItem("Full screen", "Alt+Enter", &fullscreen) )
    //        {
    //            m_pWindow->SetFullscreen(fullscreen);
    //        }

    //        ImGui::EndMenu();
    //    }

    //    char buffer[256];

    //    {
    //        // Output a slider to scale the resolution of the HDR render target.
    //        float renderScale = m_RenderScale;
    //        ImGui::PushItemWidth(300.0f);
    //        ImGui::SliderFloat("Resolution Scale", &renderScale, 0.1f, 2.0f);
    //        // Using Ctrl+Click on the slider, the user can set values outside of the 
    //        // specified range. Make sure to clamp to a sane range to avoid creating
    //        // giant render targets.
    //        renderScale = clamp(renderScale, 0.0f, 2.0f);

    //        // Output current resolution of render target.
    //        auto size = m_HDRRenderTarget.GetSize();
    //        ImGui::SameLine();
    //        sprintf_s(buffer, _countof(buffer), "(%ux%u)", size.x, size.y);
    //        ImGui::Text(buffer);

    //        // Resize HDR render target if the scale changed.
    //        if (renderScale != m_RenderScale)
    //        {
    //            m_RenderScale = renderScale;
    //            RescaleHDRRenderTarget(m_RenderScale);
    //        }
    //    }

    //    {
    //        sprintf_s(buffer, _countof(buffer), "FPS: %.2f (%.2f ms)  ", g_FPS, 1.0 / g_FPS * 1000.0);
    //        auto fpsTextSize = ImGui::CalcTextSize(buffer);
    //        ImGui::SameLine(ImGui::GetWindowWidth() - fpsTextSize.x);
    //        ImGui::Text(buffer);
    //    }

    //    ImGui::EndMainMenuBar();
    //}

    //if (showDemoWindow)
    //{
    //    ImGui::ShowDemoWindow(&showDemoWindow);
    //}

    //if (showOptions)
    //{
    //    ImGui::Begin("Tonemapping", &showOptions);
    //    {
    //        ImGui::TextWrapped("Use the Exposure slider to adjust the overall exposure of the HDR scene.");
    //        ImGui::SliderFloat("Exposure", &g_TonemapParameters.Exposure, -10.0f, 10.0f);
    //        ImGui::SameLine(); ShowHelpMarker("Adjust the overall exposure of the HDR scene.");
    //        ImGui::SliderFloat("Gamma", &g_TonemapParameters.Gamma, 0.01f, 5.0f);
    //        ImGui::SameLine(); ShowHelpMarker("Adjust the Gamma of the output image.");

    //        const char* toneMappingMethods[] = {
    //            "Linear",
    //            "Reinhard",
    //            "Reinhard Squared",
    //            "ACES Filmic"
    //        };

    //        ImGui::Combo("Tonemapping Methods", (int*)(&g_TonemapParameters.TonemapMethod), toneMappingMethods, 4);

    //        switch (g_TonemapParameters.TonemapMethod)
    //        {
    //        case TonemapMethod::TM_Linear:
    //            ImGui::PlotLines("Linear Tonemapping", &LinearTonemappingPlot, nullptr, VALUES_COUNT, 0, nullptr, 0.0f, 1.0f, ImVec2(0, 250));
    //            ImGui::SliderFloat("Max Brightness", &g_TonemapParameters.MaxLuminance, 1.0f, HDR_MAX );
    //            ImGui::SameLine(); ShowHelpMarker("Linearly scale the HDR image by the maximum brightness.");
    //            break;
    //        case TonemapMethod::TM_Reinhard:
    //            ImGui::PlotLines("Reinhard Tonemapping", &ReinhardTonemappingPlot, nullptr, VALUES_COUNT, 0, nullptr, 0.0f, 1.0f, ImVec2(0, 250));
    //            ImGui::SliderFloat("Reinhard Constant", &g_TonemapParameters.K, 0.01f, 10.0f);
    //            ImGui::SameLine(); ShowHelpMarker("The Reinhard constant is used in the denominator.");
    //            break;
    //        case TonemapMethod::TM_ReinhardSq:
    //            ImGui::PlotLines("Reinhard Squared Tonemapping", &ReinhardSqrTonemappingPlot, nullptr, VALUES_COUNT, 0, nullptr, 0.0f, 1.0f, ImVec2(0, 250));
    //            ImGui::SliderFloat("Reinhard Constant", &g_TonemapParameters.K, 0.01f, 10.0f);
    //            ImGui::SameLine(); ShowHelpMarker("The Reinhard constant is used in the denominator.");
    //            break;
    //        case TonemapMethod::TM_ACESFilmic:
    //            ImGui::PlotLines("ACES Filmic Tonemapping", &ACESFilmicTonemappingPlot, nullptr, VALUES_COUNT, 0, nullptr, 0.0f, 1.0f, ImVec2(0, 250));
    //            ImGui::SliderFloat("Shoulder Strength", &g_TonemapParameters.A, 0.01f, 5.0f);
    //            ImGui::SliderFloat("Linear Strength", &g_TonemapParameters.B, 0.0f, 100.0f);
    //            ImGui::SliderFloat("Linear Angle", &g_TonemapParameters.C, 0.0f, 1.0f);
    //            ImGui::SliderFloat("Toe Strength", &g_TonemapParameters.D, 0.01f, 1.0f);
    //            ImGui::SliderFloat("Toe Numerator", &g_TonemapParameters.E, 0.0f, 10.0f);
    //            ImGui::SliderFloat("Toe Denominator", &g_TonemapParameters.F, 1.0f, 10.0f);
    //            ImGui::SliderFloat("Linear White", &g_TonemapParameters.LinearWhite, 1.0f, 120.0f);
    //            break;
    //        default:
    //            break;
    //        }
    //    }

    //    if (ImGui::Button("Reset to Defaults"))
    //    {
    //        TonemapMethod method = g_TonemapParameters.TonemapMethod;
    //        g_TonemapParameters = TonemapParameters();
    //        g_TonemapParameters.TonemapMethod = method;
    //    }

    //    ImGui::End();

    //}
}

void Tutorial5::OnRender(RenderEventArgs& e)
{
    super::OnRender(e);

    // Render GUI.
    OnGUI();

    // Present
    // TODO: Present is done through the swapchain now.
//    m_pWindow->Present();
}

static bool g_AllowFullscreenToggle = true;

void Tutorial5::OnKeyPressed(KeyEventArgs& e)
{
    super::OnKeyPressed(e);

//    if (!ImGui::GetIO().WantCaptureKeyboard)
    {
        switch (e.Key)
        {
        case KeyCode::Escape:
            Application::Get().Quit(0);
            break;
        case KeyCode::Enter:
            if (e.Alt)
            {
        case KeyCode::F11:
            if (g_AllowFullscreenToggle)
            {
                m_pWindow->ToggleFullscreen();
                g_AllowFullscreenToggle = false;
            }
            break;
            }
        case KeyCode::V:
        // TODO: VSync is done through the swap chain now.
//            m_pWindow->ToggleVSync();
            break;
        case KeyCode::R:
            // Reset camera transform
            m_Camera.set_Translation(m_pAlignedCameraData->m_InitialCamPos);
            m_Camera.set_Rotation(m_pAlignedCameraData->m_InitialCamRot);
            m_Camera.set_FoV(m_pAlignedCameraData->m_InitialFov);
            m_Pitch = 0.0f;
            m_Yaw = 0.0f;
            break;
        case KeyCode::Up:
        case KeyCode::W:
            m_Forward = 1.0f;
            break;
        case KeyCode::Left:
        case KeyCode::A:
            m_Left = 1.0f;
            break;
        case KeyCode::Down:
        case KeyCode::S:
            m_Backward = 1.0f;
            break;
        case KeyCode::Right:
        case KeyCode::D:
            m_Right = 1.0f;
            break;
        case KeyCode::Q:
            m_Down = 1.0f;
            break;
        case KeyCode::E:
            m_Up = 1.0f;
            break;
        case KeyCode::Space:
            m_AnimateLights = !m_AnimateLights;
            break;
        case KeyCode::ShiftKey:
            m_Shift = true;
            break;
        }
    }
}

void Tutorial5::OnKeyReleased(KeyEventArgs& e)
{
    super::OnKeyReleased(e);

    //if (!ImGui::GetIO().WantCaptureKeyboard)
    {
        switch (e.Key)
        {
        case KeyCode::Enter:
            if (e.Alt)
            {
        case KeyCode::F11:
            g_AllowFullscreenToggle = true;
            }
            break;
        case KeyCode::Up:
        case KeyCode::W:
            m_Forward = 0.0f;
            break;
        case KeyCode::Left:
        case KeyCode::A:
            m_Left = 0.0f;
            break;
        case KeyCode::Down:
        case KeyCode::S:
            m_Backward = 0.0f;
            break;
        case KeyCode::Right:
        case KeyCode::D:
            m_Right = 0.0f;
            break;
        case KeyCode::Q:
            m_Down = 0.0f;
            break;
        case KeyCode::E:
            m_Up = 0.0f;
            break;
        case KeyCode::ShiftKey:
            m_Shift = false;
            break;
        }
    }
}

void Tutorial5::OnMouseMoved(MouseMotionEventArgs& e)
{
    super::OnMouseMoved(e);

    const float mouseSpeed = 0.1f;
    //if (!ImGui::GetIO().WantCaptureMouse)
    {
        if (e.LeftButton)
        {
            m_Pitch -= e.RelY * mouseSpeed;

            m_Pitch = clamp(m_Pitch, -90.0f, 90.0f);

            m_Yaw -= e.RelX * mouseSpeed;
        }
    }
}


void Tutorial5::OnMouseWheel(MouseWheelEventArgs& e)
{
    //if (!ImGui::GetIO().WantCaptureMouse)
    {
        auto fov = m_Camera.get_FoV();

        fov -= e.WheelDelta;
        fov = clamp(fov, 12.0f, 90.0f);

        m_Camera.set_FoV(fov);

        char buffer[256];
        sprintf_s(buffer, "FoV: %f\n", fov);
        OutputDebugStringA(buffer);
    }
}
