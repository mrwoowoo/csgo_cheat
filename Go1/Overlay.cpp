#include "Overlay.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool Overlay::Create()
{
    width = ::GetSystemMetrics(SM_CXSCREEN);
    height = ::GetSystemMetrics(SM_CYSCREEN);
    windowName = TEXT("TIM-退退退");
    hInstance = GetModuleHandle(nullptr);
    WNDCLASSEX wcex = {
         sizeof(WNDCLASSEX),
         0,
         ::DefWindowProc,
         0,
         0,
         hInstance,
         ::LoadIcon(nullptr, IDI_APPLICATION),
         ::LoadCursor(nullptr, IDC_ARROW),
         nullptr,
         nullptr,
         windowName,
         ::LoadIcon(nullptr, IDI_APPLICATION)
    };

    ::RegisterClassEx(&wcex);
    hwnd = ::CreateWindowEx(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW,
        windowName, windowName,
        WS_POPUP,
        0, 0, width-1, height-2,
        nullptr, nullptr, hInstance, nullptr);

    if (!hwnd)
    {
        ::wprintf_s(L"创建窗口失败\n");
        return false;
    }

    ::SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 255, LWA_ALPHA);
    MARGINS margin = { -1 };
    ::DwmExtendFrameIntoClientArea(hwnd, &margin);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        ::wprintf_s(L"创建D3d设备失败\n");
        CleanupDeviceD3D();
        ::UnregisterClass(windowName, hInstance);
        return false;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.IniFilename = nullptr;
    ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\Simhei.ttf", 15.f, NULL, io.Fonts->GetGlyphRangesChineseFull());
    IM_ASSERT(font != nullptr);

    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(pd3dDevice, pd3dDeviceContext);
}

bool Overlay::CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ::ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (::D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &pSwapChain, &pd3dDevice, &featureLevel, &pd3dDeviceContext) != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void Overlay::CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (pSwapChain) { pSwapChain->Release(); pSwapChain = NULL; }
    if (pd3dDeviceContext) { pd3dDeviceContext->Release(); pd3dDeviceContext = NULL; }
    if (pd3dDevice) { pd3dDevice->Release(); pd3dDevice = NULL; }
}

void Overlay::CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
    pBackBuffer->Release();
}

void Overlay::CleanupRenderTarget()
{
    if (mainRenderTargetView) { mainRenderTargetView->Release(); mainRenderTargetView = NULL; }
}

void Overlay::Clear()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(windowName, hInstance);
}

void Overlay::DrawFilledRect(float x1, float y1, float x2, float y2, ImColor color)
{
    ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(x1, y1), ImVec2(x2, y2), color);
}

void Overlay::DrawLine(float x1, float y1, float x2, float y2, ImColor color, float thinkness)
{
    ImGui::GetForegroundDrawList()->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), color, thinkness);
}

void Overlay::DrawRect(float x1, float y1, float x2, float y2, ImColor color, float thinkness)
{
    //DrawLine(x1, y1, x2, y1, color, thinkness);
    //DrawLine(x2, y1, x2, y2, color, thinkness);
    //DrawLine(x2, y2, x1, y2, color, thinkness);
    //DrawLine(x1, y2, x1, y1, color, thinkness);
    ImGui::GetForegroundDrawList()->AddRect(ImVec2(x1, y1), ImVec2(x2, y2), color, 0.f, 0, thinkness);
}

void Overlay::DrawTxt(float x, float y, ImColor color, const char* text)
{
    ImGui::GetForegroundDrawList()->AddText(ImVec2(x, y), color, text);
}