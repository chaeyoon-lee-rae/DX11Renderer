#include "../shared/Renderer.h"
#include "GenGeo.h"

#include <tuple>
#include <vector>

using namespace std;

Renderer::Renderer(int screenWidth, int screenHeight) : WinApp(screenWidth, screenHeight) {}

bool Renderer::Initialize() {

    if (!WinApp::Initialize())
        return false;

    D3DUtils::CreateTexture("free_texture2.jpg", m_texture, m_textureResourceView, m_device);
    D3DUtils::CreateSamplerState(m_samplerState, m_device);

    const int numSplit = 5;
    m_numSplit = numSplit;

    vector<MeshInfo> v_mesh;
    v_mesh.resize(numSplit);

    for (int i = 0; i < numSplit; ++i)
        v_mesh[i] = GenGeo::MakeBox(m_boxWidth, m_boxHeight, numSplit, i);

    for (size_t i = 0; i < v_mesh.size(); ++i) {
        // MESH
        auto &meshData = v_mesh[i];

        auto new_mesh = std::make_shared<Mesh>();
        auto vertexConstantBufferCPU = std::make_shared<vertexConstantBuffer>();
        auto pixelConstantBufferCPU = std::make_shared<pixelConstantBuffer>();

        D3DUtils::CreateVertexBuffer(meshData.vertices, new_mesh->m_vertexBuffer, m_device);
        D3DUtils::CreateIndexBuffer(meshData.indices, new_mesh->m_indexBuffer, m_device);
        new_mesh->m_indexCount = UINT(meshData.indices.size());

        vertexConstantBufferCPU->model = Matrix();
        vertexConstantBufferCPU->view = Matrix();
        vertexConstantBufferCPU->projection = Matrix();

        D3DUtils::CreateConstantBuffer(*vertexConstantBufferCPU, new_mesh->m_vertexConstantBuffer,
                                       m_device);
        D3DUtils::CreateConstantBuffer(*pixelConstantBufferCPU, new_mesh->m_pixelConstantBuffer,
                                       m_device);

        m_mesh.push_back(new_mesh);
        m_vertexConstantBufferCPU.push_back(vertexConstantBufferCPU);
        m_pixelConstantBufferCPU.push_back(pixelConstantBufferCPU);

        // NORMALS
        auto new_normal = std::make_shared<Mesh>();
        auto normalVertexConstantBufferCPU = std::make_shared<normalVertexConstantBuffer>();

        std::vector<Vertex> normalVertices;
        std::vector<uint16_t> normalIndices;
        for (size_t i = 0; i < meshData.vertices.size(); i++) {
            auto v = meshData.vertices[i];

            v.uv.x = 0.0f; // meaning a starting point for the normal shader
            normalVertices.push_back(v);

            v.uv.x = 1.0f; // meaning an end point for the normal shader
            normalVertices.push_back(v);

            normalIndices.push_back(uint16_t(2 * i));
            normalIndices.push_back(uint16_t(2 * i + 1));
        }

        D3DUtils::CreateVertexBuffer(normalVertices, new_normal->m_vertexBuffer, m_device);
        D3DUtils::CreateIndexBuffer(normalIndices, new_normal->m_indexBuffer, m_device);

        new_normal->m_indexCount = UINT(normalIndices.size());

        D3DUtils::CreateConstantBuffer(*normalVertexConstantBufferCPU,
                                       new_normal->m_vertexConstantBuffer, m_device);

        m_normalLines.push_back(new_normal);
        m_normalVertexConstantBufferCPU.push_back(normalVertexConstantBufferCPU);
    }

    vector<D3D11_INPUT_ELEMENT_DESC> inputElements = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 3 + 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    D3DUtils::CreateVertexShader(L"../shared/MeshVertexShader.hlsl", inputElements, m_meshVertexShader,
                                 m_meshInputLayout, m_shaderBlob, m_device);
    D3DUtils::CreateVertexShader(L"../shared/NormalVertexShader.hlsl", inputElements, m_normalVertexShader,
                                 m_meshInputLayout, m_shaderBlob, m_device);
    D3DUtils::CreateInputLayout(inputElements, m_meshInputLayout, m_shaderBlob, m_device);

    D3DUtils::CreatePixelShader(L"../shared/MeshPixelShader.hlsl", m_meshPixelShader, m_device);
    D3DUtils::CreatePixelShader(L"../shared/NormalPixelShader.hlsl", m_normalPixelShader, m_device);


    return true;
}

void Renderer::Update() {
    using namespace DirectX;

    static const float startT = -1.0f * (m_numSplit / 5.0f);
    static const float dt = 1.0f / 50.0f;
    static float t = startT;
    t += 2.0f * dt;

    static UINT flags = (1 << m_mesh.size()) - 1;

    static const float delay = 0.4f;

    static const float offset = float(m_boxHeight / m_numSplit);
    Vector3 startHeight;
    static const int div = m_numSplit / 2;
    if (div % 2) {
        startHeight = Vector3(0.0f, offset * div, 0.0f);
    } else {
        startHeight = Vector3(0.0f, offset * div + offset / 2.0f, 0.0f);
    }

    for (size_t i = 0; i < m_mesh.size(); ++i) {
        auto &mesh = m_mesh[i];
        auto &vcb = m_vertexConstantBufferCPU[i];
        auto &pcb = m_pixelConstantBufferCPU[i];

        auto &normal = m_normalLines[i];
        auto &normalVcb = m_normalVertexConstantBufferCPU[i];

        float rotY = 0.0f;
        auto real_t = max(t - float(i) * delay, 0.0f);
        if (flags & (1<<i))
            rotY = (real_t - sin(real_t));

        auto trans = m_modelTranslation + startHeight - float(i) * Vector3(0.0f, offset, 0.0f);

        vcb->model = Matrix::CreateScale(m_modelScaling) *
                     Matrix::CreateRotationY(m_modelRotation.y + rotY) *
                     Matrix::CreateTranslation(trans * m_modelScaling) *
                     Matrix::CreateRotationX(m_modelRotation.x) *
                     Matrix::CreateRotationZ(m_modelRotation.z);
        vcb->model = vcb->model.Transpose();

        if (abs(rotY - XM_2PI) < 1e-3)
            flags &= ~(1 << i);
        if (flags == 0) {
            t = startT;
            flags = (1 << m_mesh.size()) - 1;
        }

        vcb->normalM = vcb->model;
        vcb->normalM.Translation(Vector3(0.0f));
        vcb->normalM = vcb->normalM.Transpose().Invert();


        vcb->view =
            Matrix::CreateRotationY(m_viewRot) * Matrix::CreateTranslation(0.0f, 0.0f, 2.0f);
        pcb->camera = Vector3::Transform(Vector3(0.0f), vcb->view.Invert());
        vcb->view = vcb->view.Transpose();


        const float aspect = WinApp::GetAspectRatio();
        if (m_usePerspectiveProjection) {
            vcb->projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(m_projFovAngleY), aspect,
                                                       m_nearZ, m_farZ);
        } else {
            vcb->projection =
                XMMatrixOrthographicOffCenterLH(-aspect, aspect, -1.0f, 1.0f, m_nearZ, m_farZ);
        }
        vcb->projection = vcb->projection.Transpose();


        pcb->material.diff = Vector3(m_materialDiffuse);
        pcb->material.spec = Vector3(m_materialSpecular);

        for (int i = 0; i < 3; i++) {
            if (i != m_lightType) {
                pcb->lights[i].strength *= 0.0f;
            } else {
                pcb->lights[i] = m_lightFromGUI;
            }
        }

        if (m_useTexture)
            pcb->useTexture = 1.0f;
        else
            pcb->useTexture = 0.0f;

        D3DUtils::UpdateBuffer(*vcb, mesh->m_vertexConstantBuffer, m_context);
        D3DUtils::UpdateBuffer(*pcb, mesh->m_pixelConstantBuffer, m_context);

        if (m_drawNormals && m_drawNormalsDirtyFlag) {
            normalVcb->scale = m_normalScale;
            D3DUtils::UpdateBuffer(*normalVcb, normal->m_vertexConstantBuffer, m_context);
            m_drawNormalsDirtyFlag = false;
        }
    }

}

void Renderer::Render() {
    D3DUtils::SetViewPort(m_guiWidth, m_screenWidth, m_screenHeight, m_viewport, m_context);

    float clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    m_context->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);
    m_context->ClearDepthStencilView(m_depthStencilView.Get(),
                                        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
                                        1.0f, 0);
    m_context->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(),
                                    m_depthStencilView.Get());
    m_context->OMSetDepthStencilState(m_depthStencilState.Get(), 0);

    for (size_t i = 0; i < m_mesh.size(); ++i) {
        auto &mesh = m_mesh[i];
        auto &normal = m_normalLines[i];

        m_context->VSSetShader(m_meshVertexShader.Get(), 0, 0);
        m_context->VSSetConstantBuffers(
            0, 1, mesh->m_vertexConstantBuffer.GetAddressOf());

        m_context->PSSetShaderResources(0, 1, m_textureResourceView.GetAddressOf());
        m_context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());

        m_context->PSSetConstantBuffers(
            0, 1, mesh->m_pixelConstantBuffer.GetAddressOf());
        m_context->PSSetShader(m_meshPixelShader.Get(), 0, 0);

        if (m_drawAsWire) {
            m_context->RSSetState(m_wireRasterizerState.Get());
        } else {
            m_context->RSSetState(m_solidRasterizerState.Get());
        }

        UINT stride = sizeof(Vertex);
        UINT offset = 0;
        m_context->IASetInputLayout(m_meshInputLayout.Get());
        m_context->IASetVertexBuffers(0, 1, mesh->m_vertexBuffer.GetAddressOf(),
                                      &stride, &offset);
        m_context->IASetIndexBuffer(mesh->m_indexBuffer.Get(),
                                    DXGI_FORMAT_R16_UINT, 0);
        m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        m_context->DrawIndexed(mesh->m_indexCount, 0, 0);

        // draw normal vectors
        if (m_drawNormals) {
            m_context->VSSetShader(m_normalVertexShader.Get(), 0, 0);

            ID3D11Buffer *pptr[2] = {mesh->m_vertexConstantBuffer.Get(),
                                     normal->m_vertexConstantBuffer.Get()};
            m_context->VSSetConstantBuffers(0, 2, pptr);

            m_context->PSSetShader(m_normalPixelShader.Get(), 0, 0);
            m_context->IASetVertexBuffers(
                0, 1, normal->m_vertexBuffer.GetAddressOf(), &stride,
                &offset);
            m_context->IASetIndexBuffer(normal->m_indexBuffer.Get(),
                                        DXGI_FORMAT_R16_UINT, 0);
            m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
            m_context->DrawIndexed(normal->m_indexCount, 0, 0);
        }
    }
}

void Renderer::UpdateGUI() {
    ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);

    ImGuiIO &io = ImGui::GetIO();
    if (ImGui::IsMousePosValid())
        ImGui::Text("Mouse pos: (%g, %g)", io.MousePos.x, io.MousePos.y);
    else
        ImGui::Text("Mouse pos: <INVALID>");
    ImGui::Text("Mouse delta: (%g, %g)", io.MouseDelta.x, io.MouseDelta.y);

    if (ImGui::CollapsingHeader("Render Options", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::Checkbox("Use Texture", &m_useTexture)) {
            m_useTextureDirtyFlag = true;
        }
        ImGui::Checkbox("Wireframe", &m_drawAsWire);
        ImGui::Checkbox("Draw Normals", &m_drawNormals);
        if (ImGui::SliderFloat("Normal scale", &m_normalScale, 0.0f, 1.0f)) {
            m_drawNormalsDirtyFlag = true;
        }
    }

    if (ImGui::CollapsingHeader("Transformations", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::SliderFloat3("m_modelTranslation", &m_modelTranslation.x, -2.0f, 2.0f);
        ImGui::SliderFloat3("m_modelRotation", &m_modelRotation.x, -3.14f, 3.14f);
        ImGui::SliderFloat3("m_modelScaling", &m_modelScaling.x, 0.1f, 2.0f);    
        ImGui::SliderFloat("m_viewRot", &m_viewRot, -3.14f, 3.14f);
    }

    if (ImGui::CollapsingHeader("Shading Options", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::SliderFloat("Material Diffuse", &m_materialDiffuse, 0.0f, 1.0f);
        ImGui::SliderFloat("Material Specular", &m_materialSpecular, 0.0f, 1.0f);
        if (ImGui::SliderFloat("Material Shininess", &m_shininess, 1.0f, 256.0f)) {
            m_shininessDirtyFlag = true;
        }

        if (ImGui::RadioButton("Directional Light", m_lightType == 0)) {
            m_lightType = 0;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Point Light", m_lightType == 1)) {
            m_lightType = 1;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Spot Light", m_lightType == 2)) {
            m_lightType = 2;
        }

        ImGui::SliderFloat3("Light Position", &m_lightFromGUI.position.x, -5.0f, 5.0f);
        ImGui::SliderFloat("Light fallOffStart", &m_lightFromGUI.fallOffStart, 0.0f, 5.0f);
        ImGui::SliderFloat("Light fallOffEnd", &m_lightFromGUI.fallOffEnd, 0.0f, 10.0f);
        ImGui::SliderFloat("Light spotPower", &m_lightFromGUI.spotPower, 1.0f, 512.0f);
    }
}

