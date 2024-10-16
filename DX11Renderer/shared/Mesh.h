#pragma once

#include <d3d11.h>
#include <windows.h>
#include <wrl.h>

#include "Vertex.h"

using Microsoft::WRL::ComPtr;

struct Mesh {
    ComPtr<ID3D11Buffer> m_vertexBuffer;
    ComPtr<ID3D11Buffer> m_indexBuffer;
    ComPtr<ID3D11Buffer> m_vertexConstantBuffer;
    ComPtr<ID3D11Buffer> m_pixelConstantBuffer;

    UINT m_indexCount = 0;
};
