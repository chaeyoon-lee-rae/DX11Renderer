#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi1_3.h>
#include <windows.h>
#include <wrl.h>

#include <iostream>
#include <vector>

#include "CheckHr.h"
#include "Vertex.h"

using Microsoft::WRL::ComPtr;
using std::shared_ptr;
using std::vector;
using std::wstring;
using std::cout;
using std::cerr;
using std::endl;

class D3DUtils {
public:
    static bool CreateDevice(UINT &numQualityLevels, ComPtr<ID3D11Device> &device,
                         ComPtr<ID3D11DeviceContext> &context);

    static bool CreateDeviceAndSwapChain(const int width, const int height,
                                         const UINT numQualityLevels, const HWND &window,
                                         ComPtr<IDXGISwapChain> &swapChain,
                                         ComPtr<ID3D11Device> &device,
                                         ComPtr<ID3D11DeviceContext> &context);

    static bool CreateRenderTargetView(ComPtr<IDXGISwapChain> &swapChain,
                                       ComPtr<ID3D11Device> &device,
                                       ComPtr<ID3D11RenderTargetView> &renderTargetView);

    static bool SetViewPort(const int guiWidth, const int width, const int height,
                            D3D11_VIEWPORT &viewPort, ComPtr<ID3D11DeviceContext> &context);

    static bool CreateRasterizerState(ComPtr<ID3D11RasterizerState> &solidRasterizerState,
                                      ComPtr<ID3D11RasterizerState> &wireRasterizerState,
                                      ComPtr<ID3D11Device> &device);

    static bool CreateDepthBuffer(const int width, const int height, const UINT &numQualityLevels,
                                  ComPtr<ID3D11Device> &device,
                                  ComPtr<ID3D11Texture2D> &depthStencilBuffer,
                                  ComPtr<ID3D11DepthStencilView> &depthStencilView);

    static bool CreateDepthStencilState(ComPtr<ID3D11DepthStencilState> depthStencilState,
                                        ComPtr<ID3D11Device> device);

    static void CreateVertexShader(const wstring &filename,
                                   const vector<D3D11_INPUT_ELEMENT_DESC> &inputElements,
                                   ComPtr<ID3D11VertexShader> &vertexShader,
                                   ComPtr<ID3D11InputLayout> &inputLayout,
                                   ComPtr<ID3DBlob> &shaderBlob, ComPtr<ID3D11Device> &device);

    static void CreateInputLayout(const vector<D3D11_INPUT_ELEMENT_DESC> &inputElements,
                                  ComPtr<ID3D11InputLayout> &inputLayout,
                                  ComPtr<ID3DBlob> &shaderBlob, ComPtr<ID3D11Device> &device);

    static void CreatePixelShader(const wstring &filename, ComPtr<ID3D11PixelShader> &pixelShader,
                                  ComPtr<ID3D11Device> &device);

    static void CreateTexture(const std::string &filename, ComPtr<ID3D11Texture2D> &texture,
                            ComPtr<ID3D11ShaderResourceView> &textureResourceView,
                            ComPtr<ID3D11Device> &device);

    static void CreateSamplerState(ComPtr<ID3D11SamplerState> &samplerState,
                                   ComPtr<ID3D11Device> &device);

    static void CreateVertexBuffer(const vector<Vertex>& vertices,
                                   ComPtr<ID3D11Buffer> &vertexBuffer,
                                   ComPtr<ID3D11Device> &device);

    static void CreateIndexBuffer(const std::vector<uint16_t> &indices,
                                  ComPtr<ID3D11Buffer> &m_indexBuffer,
                                  ComPtr<ID3D11Device> &device);

    template <typename T_ConstantBuffer>
    static void CreateConstantBuffer(const T_ConstantBuffer &constantBufferCPU,
                                     ComPtr<ID3D11Buffer> &constantBuffer,
                                     ComPtr<ID3D11Device> &device);

    template <typename T_ConstantBuffer>
    static void UpdateBuffer(const T_ConstantBuffer &bufferCPU, ComPtr<ID3D11Buffer> &buffer,
                      ComPtr<ID3D11DeviceContext> &context);
};

template <typename T_ConstantBuffer>
void D3DUtils::CreateConstantBuffer(const T_ConstantBuffer &constantBufferCPU,
                                    ComPtr<ID3D11Buffer> &constantBuffer, ComPtr<ID3D11Device> &device) {
    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.ByteWidth = (sizeof(T_ConstantBuffer) + 15) / 16 * 16; // 16바이트에 맞춤
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = &constantBufferCPU;

    HRESULT hr = device->CreateBuffer(&cbDesc, &initData, constantBuffer.GetAddressOf());
    CheckFailed(hr, "Failed to create constant buffer.");
}

template <typename T_ConstantBuffer>
void D3DUtils::UpdateBuffer(const T_ConstantBuffer &bufferCPU, ComPtr<ID3D11Buffer> &buffer,
                            ComPtr<ID3D11DeviceContext> &context) {
    D3D11_MAPPED_SUBRESOURCE ms;
    HRESULT hr = context->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
    if (!CheckFailed(hr, "Failed to map buffer for update")) {
        memcpy(ms.pData, &bufferCPU, sizeof(bufferCPU));
        context->Unmap(buffer.Get(), 0);
    }
}
