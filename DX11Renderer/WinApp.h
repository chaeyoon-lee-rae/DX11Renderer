#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi1_3.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <windows.h>
#include <wrl.h>

#include <iostream>
#include <vector>

#include "CheckHr.h"
#include "Vertex.h"
#include "D3DUtils.h"

using Microsoft::WRL::ComPtr;
using std::shared_ptr;
using std::vector;
using std::wstring;

class WinApp {
public:
    WinApp(int screenWidth, int screenHeight);
    virtual ~WinApp();
    int Run();

    virtual bool Initialize();
    virtual void Update() = 0;
    virtual void Render() = 0;
    virtual void UpdateGUI() = 0;

    virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    float GetAspectRatio() const;

protected:
    bool InitMainWindow();
    bool InitDirect3D();
    bool InitGUI();

protected:
    // Window Setting Variables
    int m_screenWidth;
    int m_screenHeight;
    float m_guiWidth;
    HWND m_mainWindow;
    UINT m_numQualityLevels; // MSAA quality level

    // Direct3D Device Variables
    ComPtr<ID3D11Device> m_device;
    ComPtr<ID3D11DeviceContext> m_context;
    ComPtr<ID3D11RenderTargetView> m_renderTargetView;
    ComPtr<IDXGISwapChain> m_swapChain;

    // Rasterization State Variables
    ComPtr<ID3D11RasterizerState> m_solidRasterizerState;
    ComPtr<ID3D11RasterizerState> m_wireRasterizerState;
    bool m_drawAsWire; // Whether to render in wireframe mode

    // Depth-Stencil Buffer and State Variables
    ComPtr<ID3D11Texture2D> m_depthStencilBuffer;
    ComPtr<ID3D11DepthStencilView> m_depthStencilView;
    ComPtr<ID3D11DepthStencilState> m_depthStencilState;

    // Viewport Setting Variables
    D3D11_VIEWPORT m_viewport;
};