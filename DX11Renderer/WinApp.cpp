#include "WinApp.h"

using std::cerr;
using std::endl;

// ImGui 라이브러리에서 제공하는 Win32 메시지 처리 함수, 전방 선언 필요
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam,
                                                             LPARAM lParam);
WinApp *g_appBase = nullptr;

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    return g_appBase->MsgProc(hWnd, msg, wParam, lParam);
}

WinApp::WinApp(int screenWidth, int screenHeight)
    : m_screenWidth(screenWidth), m_screenHeight(screenHeight), m_mainWindow(0),
      m_viewport(D3D11_VIEWPORT()) {

    g_appBase = this;
}

WinApp::~WinApp() {
    g_appBase = nullptr;

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    DestroyWindow(m_mainWindow);
}

float WinApp::GetAspectRatio() const {
    return float(m_screenWidth - m_guiWidth) / m_screenHeight;
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

            m_guiWidth = int(ImGui::GetWindowWidth());

            ImGui::SetWindowPos(ImVec2(float(m_screenWidth - m_guiWidth), 0.0f));
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

bool WinApp::Initialize() {
    if (!InitMainWindow())
        return false;

    if (!InitDirect3D())
        return false;

    if (!InitGUI())
        return false;

    return true;
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
            m_swapChain->ResizeBuffers(0,
                                       (UINT)LOWORD(lParam),
                                       (UINT)HIWORD(lParam),
                                       DXGI_FORMAT_UNKNOWN,
                                       0);
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
                     NULL,
                     NULL,
                     NULL,
                     NULL,
                     L"Renderer",
                     NULL};

    if (!RegisterClassEx(&wc)) {
        cerr << "RegisterClassEx() failed." << endl;
        return false;
    }

    RECT wr = {0, 0, m_screenWidth, m_screenHeight};

    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, false);

    m_mainWindow = CreateWindow(wc.lpszClassName, L"Renderer",
                                WS_OVERLAPPEDWINDOW,
                                100,
                                100,
                                wr.right - wr.left,
                                wr.bottom - wr.top,
                                NULL, NULL, wc.hInstance, NULL);

    if (!m_mainWindow) {
        cerr << "CreateWindow() failed." << endl;
        return false;
    }

    ShowWindow(m_mainWindow, SW_SHOWDEFAULT);
    UpdateWindow(m_mainWindow);

    return true;
}

bool WinApp::InitDirect3D() {
    bool result = 0;
    result |= !D3DUtils::CreateDevice(m_numQualityLevels, m_device, m_context);
    result |= !D3DUtils::CreateDeviceAndSwapChain(m_screenWidth, m_screenHeight, m_numQualityLevels,
                                                  m_mainWindow, m_swapChain, m_device, m_context);
    result |= !D3DUtils::CreateRenderTargetView(m_swapChain, m_device, m_renderTargetView);
    result |= !D3DUtils::CreateRasterizerState(m_solidRasterizerState, m_wireRasterizerState, 
                                               m_device);
    result |= !D3DUtils::CreateDepthBuffer(m_screenWidth, m_screenHeight, m_numQualityLevels,
                                           m_device, m_depthStencilBuffer, m_depthStencilView);
    result |= !D3DUtils::CreateDepthStencilState(m_depthStencilState, m_device);
    result |= !D3DUtils::SetViewPort(m_guiWidth, m_screenWidth, m_screenHeight, m_viewport, m_context);

    return !result;
}

bool WinApp::InitGUI() {

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.DisplaySize = ImVec2(float(m_screenWidth), float(m_screenHeight));
    ImGui::StyleColorsLight();

    if (!ImGui_ImplDX11_Init(m_device.Get(), m_context.Get())) {
        return false;
    }

    if (!ImGui_ImplWin32_Init(m_mainWindow)) {
        return false;
    }

    return true;
}