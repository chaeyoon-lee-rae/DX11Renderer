#pragma once

#include <algorithm>
#include <iostream>
#include <memory>

#include "WinApp.h"
#include "Mesh.h"
#include "ConstantBuffers.h"


class Renderer : public WinApp {
public:
    Renderer(int screenWidth, int screenHeight);

    virtual bool Initialize() override;
    virtual void UpdateGUI() override;
    virtual void Update() override;
    virtual void Render() override;

protected:
    // Mesh Shader Variables
    ComPtr<ID3D11VertexShader> m_meshVertexShader;
    ComPtr<ID3D11PixelShader> m_meshPixelShader;
    ComPtr<ID3D11InputLayout> m_meshInputLayout;
    ComPtr<ID3DBlob> m_shaderBlob;

    vector<shared_ptr<Mesh>> m_mesh; // Vector to store Mesh objects for rendering

    // Texture Variables
    ComPtr<ID3D11Texture2D> m_texture;
    ComPtr<ID3D11ShaderResourceView> m_textureResourceView;
    ComPtr<ID3D11SamplerState> m_samplerState;

    // Constant Buffer Variables
    vector<shared_ptr<vertexConstantBuffer>> m_vertexConstantBufferCPU;
    vector<shared_ptr<pixelConstantBuffer>> m_pixelConstantBufferCPU;

    // Rendering Setting Variables
    bool m_usePerspectiveProjection = true; // Whether to use perspective projection
    Vector3 m_modelTranslation = Vector3(0.0f);
    Vector3 m_modelRotation = Vector3(-DirectX::XM_PI / 6.0f, DirectX::XM_PI / 4.0f, 0.0f);
    Vector3 m_modelScaling = Vector3(1.0f);
    float m_shininess = 5.0f;
    bool m_shininessDirtyFlag = true; // whether the shininess value has changed

    bool m_useTexture = false;
    bool m_useTextureDirtyFlag = true; // whether the texture usage has changed

    float m_viewRot = 0.0f;

    float m_projFovAngleY = 70.0f;
    float m_nearZ = 0.01f;
    float m_farZ = 100.0f;

    int m_numSplit = 0; // Number of boxes
    float m_boxWidth = 1.2f;
    float m_boxHeight = 1.2f;

    int m_lightType = 1;
    Light m_lightFromGUI;   // Lighting information set from the GUI
    float m_materialDiffuse = 0.7f;
    float m_materialSpecular = 0.2f;

    // Normal Shader Variables
    ComPtr<ID3D11VertexShader> m_normalVertexShader;
    ComPtr<ID3D11PixelShader> m_normalPixelShader;

    vector<shared_ptr<Mesh>> m_normalLines;
    vector<shared_ptr<normalVertexConstantBuffer>> m_normalVertexConstantBufferCPU;
    float m_normalScale;
    bool m_drawNormals = false;
    bool m_drawNormalsDirtyFlag = false; // Flag indicating whether the drawing of normals has changed
};
