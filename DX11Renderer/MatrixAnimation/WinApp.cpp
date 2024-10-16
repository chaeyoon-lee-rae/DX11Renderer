#include "WinApp.h"

WinApp *g_appBase = nullptr;

WinApp::WinApp(int screenWidth, int screenHeight)
    : m_screenWidth(screenWidth), 
      m_screenHeight(screenHeight), 
      m_mainWindow(0),
      m_viewport(D3D11_VIEWPORT()), 
      m_drawAsWire(false), 
      m_guiWidth(0.0f), 
      m_numQualityLevels(0)
{
    g_appBase = this;
}

WinApp::~WinApp() {
    g_appBase = nullptr;

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    DestroyWindow(m_mainWindow);
}

bool WinApp::Initialize() {
    if (!InitMainWindow())
        return false;

    if (!InitDirect3D())
        return false;

    if (!InitGUI())
        return false;

    return true;
}

float WinApp::GetAspectRatio() const { return float(m_screenWidth - m_guiWidth) / m_screenHeight; }

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam,
                                                             LPARAM lParam);

// 메세지 콜백 함수, 멤버 함수를 RegisterClassEx에 직접 등록할 수 없는 관계로 간접적으로 등록
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    return g_appBase->MsgProc(hWnd, msg, wParam, lParam);
}

LRESULT WinApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
        return true;

    switch (msg) {
    case WM_SIZE:
        if (m_swapChain) {
            m_screenWidth = int(LOWORD(lParam));
            m_screenHeight = int(HIWORD(lParam));
            m_guiWidth = 0;

            m_renderTargetView.Reset();
            m_swapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam),
                                       DXGI_FORMAT_UNKNOWN, 0);
            D3DUtils::CreateRenderTargetView(m_swapChain, m_device, m_renderTargetView);
            D3DUtils::SetViewPort(m_guiWidth, m_screenWidth, m_screenHeight, m_viewport, m_context);
            D3DUtils::CreateDepthBuffer(m_screenWidth, m_screenHeight, m_numQualityLevels, m_device,
                                        m_depthStencilBuffer, m_depthStencilView);
        }
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }

    return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

bool WinApp::InitMainWindow() {
    WNDCLASSEX wc = {sizeof(WNDCLASSEX),
                     CS_CLASSDC,
                     WndProc,
                     0L,
                     0L,
                     GetModuleHandle(NULL),
                     LoadIcon(NULL, IDI_APPLICATION), // TODO: custom icon
                     LoadCursor(NULL, IDC_ARROW), // TODO: custom cursor
                     (HBRUSH)(COLOR_WINDOW + 1),
                     NULL,
                     L"Dx11Renderer",
                     LoadIcon(NULL, IDI_APPLICATION)}; // TODO: custom icon

    if (!RegisterClassEx(&wc)) // TODO: exception for error handling
        return false;

    RECT wr = {0, 0, m_screenWidth, m_screenHeight};
    AdjustWindowRectEx(&wr, WS_OVERLAPPEDWINDOW, false, 0);

    m_mainWindow = CreateWindowEx(0,
                                  wc.lpszClassName, 
                                  L"Dx11Renderer Example",
                                  WS_OVERLAPPEDWINDOW,
                                  CW_USEDEFAULT, 
                                  CW_USEDEFAULT,
                                  wr.right - wr.left,
                                  wr.bottom - wr.top,
                                  NULL, NULL, wc.hInstance, NULL);

    if (!m_mainWindow) // TODO: exception for error handling
        return false;

    ShowWindow(m_mainWindow, SW_SHOWDEFAULT);

    return true;
}

bool WinApp::InitDirect3D() {
    bool result = 0;
    result |= !D3DUtils::CreateDevice(m_numQualityLevels, m_device, m_context);
    result |= !D3DUtils::CreateDeviceAndSwapChain(m_screenWidth, m_screenHeight, m_numQualityLevels,
                                                  m_mainWindow, m_swapChain, m_device, m_context);
    result |= !D3DUtils::CreateRenderTargetView(m_swapChain, m_device, m_renderTargetView);
    result |= !D3DUtils::CreateRasterizerState(m_solidRasterizerState, 
                                               m_wireRasterizerState, 
                                               m_device);
    result |= !D3DUtils::CreateDepthBuffer(m_screenWidth, m_screenHeight, m_numQualityLevels,
                                           m_device, m_depthStencilBuffer, m_depthStencilView);
    result |= !D3DUtils::CreateDepthStencilState(m_depthStencilState, m_device);
    result |= !D3DUtils::SetViewPort(m_guiWidth, m_screenWidth, m_screenHeight, 
                                     m_viewport, m_context);

    return !result;
}

bool WinApp::InitGUI() {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    if (!ImGui_ImplWin32_Init(m_mainWindow)) // TODO: error handle
        return false;
    if (!ImGui_ImplDX11_Init(m_device.Get(), m_context.Get())) // TODO: error handle
        return false;

    return true;
}

int WinApp::Run() {
    MSG msg = {0};
    while (WM_QUIT != msg.message) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();

            ImGui::NewFrame();
            ImGui::Begin("Scene Control");
            this->UpdateGUI();

            m_guiWidth = ImGui::GetWindowWidth();
            ImGui::SetWindowPos(ImVec2(float(m_screenWidth) - m_guiWidth, 0.0f));
            ImGui::SetWindowSize(ImVec2(float(m_guiWidth), float(m_screenHeight)));

            ImGui::End();
            ImGui::Render();

            this->Update();
            this->Render();

            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
            m_swapChain->Present(1, 0);
        }
    }

    return 0;
}

