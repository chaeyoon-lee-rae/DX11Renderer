#include "D3DUtils.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bool D3DUtils::CreateDevice(UINT &numQualityLevels, ComPtr<ID3D11Device> &device,
                            ComPtr<ID3D11DeviceContext> &context) {
    HRESULT hrRet;

    UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    ComPtr<ID3D11Device> device_;
    ComPtr<ID3D11DeviceContext> context_;

    const D3D_FEATURE_LEVEL featureLevels[2] = {D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_9_3};
    D3D_FEATURE_LEVEL featureLevel;

    hrRet = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags,
                              featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION,
                              device_.GetAddressOf(), &featureLevel, context_.GetAddressOf());
    if (CheckFailed(hrRet, "D3D11CreateDevice failed."))
        return false;

    if (featureLevel != D3D_FEATURE_LEVEL_11_0) {
        cerr << "D3D Feature Level 11 unsupported." << endl;
        return false;
    }

    device_->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &numQualityLevels);
    if (numQualityLevels <= 0) {
        cout << "MSAA not supported." << endl;
    }

    hrRet = device_.As(&device);
    if (CheckFailed(hrRet, "Converting to device failed."))
        return false;

    hrRet = context_.As(&context);
    if (CheckFailed(hrRet, "context.As() failed."))
        return false;

    return true;
}

bool D3DUtils::CreateDeviceAndSwapChain(const int width, const int height,
                                        const UINT numQualityLevels, const HWND &window,
                                        ComPtr<IDXGISwapChain> &swapChain,
                                        ComPtr<ID3D11Device> &device,
                                        ComPtr<ID3D11DeviceContext> &context) {
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferCount = 2;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = window;
    sd.Windowed = TRUE;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    if (numQualityLevels > 0) {
        sd.SampleDesc.Count = 4;
        sd.SampleDesc.Quality = numQualityLevels - 1;
    } else {
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
    }

    D3D_FEATURE_LEVEL FeatureLevelsRequested = D3D_FEATURE_LEVEL_11_0;
    UINT numLevelsRequested = 1;
    D3D_FEATURE_LEVEL FeatureLevelsSupported;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &FeatureLevelsRequested, numLevelsRequested,
        D3D11_SDK_VERSION, &sd, swapChain.GetAddressOf(), device.GetAddressOf(),
        &FeatureLevelsSupported, context.GetAddressOf());

    if (CheckFailed(hr, "D3D11CreateDeviceAndSwapChain() failed"))
        return false;

    return true;
}

bool D3DUtils::CreateRenderTargetView(ComPtr<IDXGISwapChain> &swapChain,
                                      ComPtr<ID3D11Device> &device,
                                      ComPtr<ID3D11RenderTargetView> &renderTargetView) {
    ComPtr<ID3D11Texture2D> backBuffer;
    swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
    if (backBuffer)
        device->CreateRenderTargetView(backBuffer.Get(), NULL, renderTargetView.GetAddressOf());
    else {
        cerr << "CreateRenderTargetView() failed." << endl;
        return false;
    }

    return true;
}

bool D3DUtils::SetViewPort(const int guiWidth, const int width, const int height,
                           D3D11_VIEWPORT &viewPort, ComPtr<ID3D11DeviceContext> &context) {
    static int previousGuiWidth = guiWidth;

    if (previousGuiWidth != guiWidth) {
        ZeroMemory(&viewPort, sizeof(D3D11_VIEWPORT));
        viewPort.TopLeftX = 0;
        viewPort.TopLeftY = 0;
        viewPort.Width = float(width - guiWidth);
        viewPort.Height = float(height);
        viewPort.MinDepth = 0.0f;
        viewPort.MaxDepth = 1.0f; // z-buffer

        context->RSSetViewports(1, &viewPort);

        previousGuiWidth = guiWidth;
    }

    return true;
}

bool D3DUtils::CreateRasterizerState(ComPtr<ID3D11RasterizerState> &solidRasterizerState,
                                     ComPtr<ID3D11RasterizerState> &wireRasterizerState,
                                     ComPtr<ID3D11Device> &device) {
    HRESULT hrRet;

    D3D11_RASTERIZER_DESC rastDesc;
    ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC));
    rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
    rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
    rastDesc.FrontCounterClockwise = false;
    rastDesc.DepthClipEnable = true;

    hrRet = device->CreateRasterizerState(&rastDesc, solidRasterizerState.GetAddressOf());
    if (CheckFailed(hrRet, "CreateRasterizerState_Solid() failed"))
        return false;

    rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
    hrRet = device->CreateRasterizerState(&rastDesc, wireRasterizerState.GetAddressOf());
    if (CheckFailed(hrRet, "CreateRasterizerState_Solid() failed"))
        return false;

    return true;
}

bool D3DUtils::CreateDepthBuffer(const int width, const int height, const UINT &numQualityLevels,
                                 ComPtr<ID3D11Device> &device,
                                 ComPtr<ID3D11Texture2D> &depthStencilBuffer,
                                 ComPtr<ID3D11DepthStencilView> &depthStencilView) {
    HRESULT hrRet;

    D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
    depthStencilBufferDesc.Width = width;
    depthStencilBufferDesc.Height = height;
    depthStencilBufferDesc.MipLevels = 1;
    depthStencilBufferDesc.ArraySize = 1;
    depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    if (numQualityLevels > 0) {
        depthStencilBufferDesc.SampleDesc.Count = 4;
        depthStencilBufferDesc.SampleDesc.Quality = numQualityLevels - 1;
    } else {
        depthStencilBufferDesc.SampleDesc.Count = 1;
        depthStencilBufferDesc.SampleDesc.Quality = 0;
    }
    depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilBufferDesc.CPUAccessFlags = 0;
    depthStencilBufferDesc.MiscFlags = 0;

    hrRet = device->CreateTexture2D(&depthStencilBufferDesc, 0, depthStencilBuffer.GetAddressOf());
    if (CheckFailed(hrRet, "CreateTexture2D() failed."))
        return false;

    hrRet = device->CreateDepthStencilView(depthStencilBuffer.Get(), 0, &depthStencilView);
    if (CheckFailed(hrRet, "CreateDepthStencilView() failed."))
        return false;

    return true;
}

bool D3DUtils::CreateDepthStencilState(ComPtr<ID3D11DepthStencilState> depthStencilState,
                                       ComPtr<ID3D11Device> device) {
    HRESULT hrRet;

    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

    hrRet = device->CreateDepthStencilState(&depthStencilDesc, depthStencilState.GetAddressOf());

    if (CheckFailed(hrRet, "CreateDepthStencilState() failed"))
        return false;

    return true;
}

void D3DUtils::CreateVertexShader(const wstring &filename,
                                  const vector<D3D11_INPUT_ELEMENT_DESC> &inputElements,
                                  ComPtr<ID3D11VertexShader> &vertexShader,
                                  ComPtr<ID3D11InputLayout> &inputLayout,
                                  ComPtr<ID3DBlob> &shaderBlob, ComPtr<ID3D11Device> &device) {

    ComPtr<ID3DBlob> errorBlob;

    UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    HRESULT hr = D3DCompileFromFile(filename.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main",
                                    "vs_5_0", compileFlags, 0, &shaderBlob, &errorBlob);

    CheckFailed(hr, "D3DCompileFromFile failed.");

    device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL,
                               &vertexShader);
}

void D3DUtils::CreateInputLayout(const vector<D3D11_INPUT_ELEMENT_DESC> &inputElements,
                                 ComPtr<ID3D11InputLayout> &inputLayout,
                                 ComPtr<ID3DBlob> &shaderBlob, ComPtr<ID3D11Device> &device) {

    HRESULT hr = device->CreateInputLayout(inputElements.data(), UINT(inputElements.size()),
                                           shaderBlob->GetBufferPointer(),
                                           shaderBlob->GetBufferSize(), &inputLayout);

    CheckFailed(hr, "CreateInputLayout failed.");
}

void D3DUtils::CreatePixelShader(const wstring &filename, ComPtr<ID3D11PixelShader> &pixelShader,
                                 ComPtr<ID3D11Device> &device) {
    ComPtr<ID3DBlob> shaderBlob;
    ComPtr<ID3DBlob> errorBlob;

    UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    HRESULT hr = D3DCompileFromFile(filename.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main",
                                    "ps_5_0", compileFlags, 0, &shaderBlob, &errorBlob);
    CheckFailed(hr, "D3DCompileFromFile() failed.");

    device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL,
                              &pixelShader);
}

void D3DUtils::CreateTexture(const std::string &filename, ComPtr<ID3D11Texture2D> &texture,
                             ComPtr<ID3D11ShaderResourceView> &textureResourceView,
                             ComPtr<ID3D11Device> &device) {

    int width, height, channels;

    unsigned char *img = stbi_load(filename.c_str(), &width, &height, &channels, 0);
    std::vector<uint8_t> image;
    image.resize(width * height * 4);
    for (size_t i = 0; i < width * height; i++) {
        for (size_t c = 0; c < 3; c++) {
            image[4 * i + c] = img[i * channels + c];
        }
        image[4 * i + 3] = 255;
    }

    D3D11_TEXTURE2D_DESC txtDesc = {};
    txtDesc.Width = width;
    txtDesc.Height = height;
    txtDesc.MipLevels = txtDesc.ArraySize = 1;
    txtDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    txtDesc.SampleDesc.Count = 1;
    txtDesc.Usage = D3D11_USAGE_IMMUTABLE;
    txtDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = image.data();
    InitData.SysMemPitch = txtDesc.Width * sizeof(uint8_t) * 4;
    InitData.SysMemSlicePitch = 0;

    device->CreateTexture2D(&txtDesc, &InitData, texture.GetAddressOf());
    device->CreateShaderResourceView(texture.Get(), nullptr, textureResourceView.GetAddressOf());
}

void D3DUtils::CreateSamplerState(ComPtr<ID3D11SamplerState> &samplerState,
                                  ComPtr<ID3D11Device> &device) {
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    device->CreateSamplerState(&sampDesc, samplerState.GetAddressOf());
}

void D3DUtils::CreateVertexBuffer(const vector<Vertex> &vertices,
                                  ComPtr<ID3D11Buffer> &vertexBuffer,
                                  ComPtr<ID3D11Device> &device) {
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * vertices.size());
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA vertexBufferData = {};
    vertexBufferData.pSysMem = vertices.data();

    HRESULT hr = device->CreateBuffer(&bufferDesc, &vertexBufferData, vertexBuffer.GetAddressOf());
    CheckFailed(hr, "CreateBuffer failed.");
}

void D3DUtils::CreateIndexBuffer(const std::vector<uint16_t> &indices,
                                 ComPtr<ID3D11Buffer> &m_indexBuffer,
                                 ComPtr<ID3D11Device> &device) {
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    bufferDesc.ByteWidth = UINT(sizeof(uint16_t) * indices.size());
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.StructureByteStride = sizeof(uint16_t);

    D3D11_SUBRESOURCE_DATA indexBufferData = {0};
    indexBufferData.pSysMem = indices.data();
    indexBufferData.SysMemPitch = 0;
    indexBufferData.SysMemSlicePitch = 0;

    device->CreateBuffer(&bufferDesc, &indexBufferData, m_indexBuffer.GetAddressOf());
}
