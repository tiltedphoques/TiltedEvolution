#include <TiltedOnlinePCH.h>

#include <Services/ImguiService.h>

#include <OverlayApp.hpp>
#include <Systems/RenderSystemD3D11.h>
#include <d3d11.h>

#include <imgui.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/ImguiFont.inl>

static void SetTiltedImStyle()
{
    // cherry colors, 3 intensities
#define HI(v) ImVec4(0.502f, 0.075f, 0.256f, v)
#define MED(v) ImVec4(0.455f, 0.198f, 0.301f, v)
#define LOW(v) ImVec4(0.232f, 0.201f, 0.271f, v)
// backgrounds (@todo: complete with BG_MED, BG_LOW)
#define BG(v) ImVec4(0.200f, 0.220f, 0.270f, v)
// text
#define TEXT(v) ImVec4(0.860f, 0.930f, 0.890f, v)

    auto& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Text] = TEXT(0.78f);
    style.Colors[ImGuiCol_TextDisabled] = TEXT(0.28f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_ChildBg] = BG(0.58f);
    style.Colors[ImGuiCol_PopupBg] = BG(0.9f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.31f, 0.31f, 1.00f, 0.00f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg] = BG(1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = MED(0.78f);
    style.Colors[ImGuiCol_FrameBgActive] = MED(1.00f);
    style.Colors[ImGuiCol_TitleBg] = LOW(1.00f);
    style.Colors[ImGuiCol_TitleBgActive] = HI(1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = BG(0.75f);
    style.Colors[ImGuiCol_MenuBarBg] = BG(0.47f);
    style.Colors[ImGuiCol_ScrollbarBg] = BG(1.00f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = MED(0.78f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = MED(1.00f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
    style.Colors[ImGuiCol_ButtonHovered] = MED(0.86f);
    style.Colors[ImGuiCol_ButtonActive] = MED(1.00f);
    style.Colors[ImGuiCol_Header] = MED(0.76f);
    style.Colors[ImGuiCol_HeaderHovered] = MED(0.86f);
    style.Colors[ImGuiCol_HeaderActive] = HI(1.00f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.47f, 0.77f, 0.83f, 0.04f);
    style.Colors[ImGuiCol_ResizeGripHovered] = MED(0.78f);
    style.Colors[ImGuiCol_ResizeGripActive] = MED(1.00f);
    style.Colors[ImGuiCol_PlotLines] = TEXT(0.63f);
    style.Colors[ImGuiCol_PlotLinesHovered] = MED(1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = TEXT(0.63f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = MED(1.00f);
    style.Colors[ImGuiCol_TextSelectedBg] = MED(0.43f);
    style.Colors[ImGuiCol_ModalWindowDimBg] = BG(0.73f);

    style.WindowPadding = ImVec2(6, 4);
    style.WindowRounding = 0.0f;
    style.FramePadding = ImVec2(5, 2);
    style.FrameRounding = 3.0f;
    style.ItemSpacing = ImVec2(7, 1);
    style.ItemInnerSpacing = ImVec2(1, 1);
    style.TouchExtraPadding = ImVec2(0, 0);
    style.IndentSpacing = 6.0f;
    style.ScrollbarSize = 12.0f;
    style.ScrollbarRounding = 16.0f;
    style.GrabMinSize = 20.0f;
    style.GrabRounding = 2.0f;

    style.WindowTitleAlign.x = 0.50f;

    style.Colors[ImGuiCol_Border] = ImVec4(0.539f, 0.479f, 0.255f, 0.162f);
    style.FrameBorderSize = 0.0f;
    style.WindowBorderSize = 1.0f;
}

ImguiService::ImguiService() : OnDraw(m_drawSignal)
{
}

ImguiService::~ImguiService() noexcept
{
}

void ImguiService::Create(RenderSystemD3D11* apRenderSystem, HWND aHwnd)
{
    ID3D11Device* d3dDevice = nullptr;
    ID3D11DeviceContext* d3dContext = nullptr;

    // create imgui
    ImGui::CreateContext();

    auto& io = ImGui::GetIO();
    io.Fonts->AddFontFromMemoryCompressedBase85TTF(Roboto_compressed_data_base85, 30.f);
    // io.IniFilename = nullptr;

    auto& st = ImGui::GetStyle();
    st.FrameBorderSize = 1.0f;
    st.FramePadding = ImVec2(4.0f, 2.0f);
    st.ItemSpacing = ImVec2(8.0f, 2.0f);
    st.WindowBorderSize = 1.0f;
    st.TabBorderSize = 1.0f;

    // make everything have smooth edges
    st.WindowRounding = 2.0f;
    st.ChildRounding = 2.0f;
    st.FrameRounding = 3.0f;
    st.ScrollbarRounding = 3.0f;
    st.GrabRounding = 2.f;
    st.TabRounding = 1.0f;
    SetTiltedImStyle();

    // TODO: should hard fail here.

    // init platform
    if (!ImGui_ImplWin32_Init(aHwnd))
        spdlog::error("Failed to initialize Imgui-Win32");

    apRenderSystem->GetSwapChain()->GetDevice(__uuidof(d3dDevice), reinterpret_cast<void**>(&d3dDevice));
    d3dDevice->GetImmediateContext(&d3dContext);

    ImGui_ImplDX11_Init(d3dDevice, d3dContext);
}

void ImguiService::Render() const
{
    ImGui_ImplDX11_NewFrame();

    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    m_drawSignal.publish();
    ImGui::Render();

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImguiService::Reset() const
{
    // TODO: idk how imgui handles this
}

static bool ImGui_ImplWin32_UpdateMouseCursor()
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
        return false;

    ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
    if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
    {
        // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
        ::SetCursor(NULL);
    }
    else
    {
        // Show OS mouse cursor
        LPTSTR win32_cursor = IDC_ARROW;
        switch (imgui_cursor)
        {
        case ImGuiMouseCursor_Arrow:
            win32_cursor = IDC_ARROW;
            break;
        case ImGuiMouseCursor_TextInput:
            win32_cursor = IDC_IBEAM;
            break;
        case ImGuiMouseCursor_ResizeAll:
            win32_cursor = IDC_SIZEALL;
            break;
        case ImGuiMouseCursor_ResizeEW:
            win32_cursor = IDC_SIZEWE;
            break;
        case ImGuiMouseCursor_ResizeNS:
            win32_cursor = IDC_SIZENS;
            break;
        case ImGuiMouseCursor_ResizeNESW:
            win32_cursor = IDC_SIZENESW;
            break;
        case ImGuiMouseCursor_ResizeNWSE:
            win32_cursor = IDC_SIZENWSE;
            break;
        case ImGuiMouseCursor_Hand:
            win32_cursor = IDC_HAND;
            break;
        case ImGuiMouseCursor_NotAllowed:
            win32_cursor = IDC_NO;
            break;
        }
        ::SetCursor(::LoadCursor(NULL, win32_cursor));
    }
    return true;
}

LRESULT ImguiService::WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui::GetCurrentContext() == NULL)
        return 0;

    ImGuiIO& io = ImGui::GetIO();
    switch (msg)
    {
    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONDBLCLK:
    case WM_XBUTTONDOWN:
    case WM_XBUTTONDBLCLK: {
        int button = 0;
        if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK)
        {
            button = 0;
        }
        if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK)
        {
            button = 1;
        }
        if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK)
        {
            button = 2;
        }
        if (msg == WM_XBUTTONDOWN || msg == WM_XBUTTONDBLCLK)
        {
            button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? 3 : 4;
        }
        if (!ImGui::IsAnyMouseDown() && ::GetCapture() == NULL)
            ::SetCapture(hwnd);
        io.MouseDown[button] = true;
        return 0;
    }
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
    case WM_XBUTTONUP: {
        int button = 0;
        if (msg == WM_LBUTTONUP)
        {
            button = 0;
        }
        if (msg == WM_RBUTTONUP)
        {
            button = 1;
        }
        if (msg == WM_MBUTTONUP)
        {
            button = 2;
        }
        if (msg == WM_XBUTTONUP)
        {
            button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? 3 : 4;
        }
        io.MouseDown[button] = false;
        if (!ImGui::IsAnyMouseDown() && ::GetCapture() == hwnd)
            ::ReleaseCapture();
        return 0;
    }
    case WM_MOUSEWHEEL:
        io.MouseWheel += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
        return 0;
    case WM_MOUSEHWHEEL:
        io.MouseWheelH += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
        return 0;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        if (wParam < 256)
            io.KeysDown[wParam] = 1;
        return 0;
    case WM_KEYUP:
    case WM_SYSKEYUP:
        if (wParam < 256)
            io.KeysDown[wParam] = 0;
        return 0;
    case WM_CHAR:
        // You can also use ToAscii()+GetKeyboardState() to retrieve characters.
        io.AddInputCharacter((unsigned int)wParam);
        return 0;
    case WM_SETCURSOR:
        if (LOWORD(lParam) == HTCLIENT && ImGui_ImplWin32_UpdateMouseCursor())
            return 1;
        return 0;
    case WM_DEVICECHANGE:
        return 0;
    }
    return 0;
}

void ImguiService::RawInputHandler(RAWINPUT& aRawinput)
{
    if (ImGui::GetCurrentContext() == NULL)
        return;

    ImGuiIO& io = ImGui::GetIO();
    if (aRawinput.header.dwType == RIM_TYPEMOUSE)
    {
        const auto mouse = aRawinput.data.mouse;

        if (mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)
        {
            io.MouseDown[0] = true;
        }

        if (mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP)
        {
            io.MouseDown[0] = false;
        }

        if (mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)
        {
            io.MouseDown[1] = true;
        }

        if (mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP)
        {
            io.MouseDown[1] = false;
        }

        if (mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
        {
            io.MouseDown[2] = true;
        }

        if (mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP)
        {
            io.MouseDown[2] = false;
        }
    }
}
