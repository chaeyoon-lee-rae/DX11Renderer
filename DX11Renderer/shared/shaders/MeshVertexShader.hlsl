#include "InputLayout.hlsli"

// Constant Buffer for Vertex Shader
cbuffer MeshVertexConstantBuffer : register(b0)
{
    float4x4 model;
    float4x4 view;
    float4x4 projection;
    float4x4 normalM;   // Normal matrix (transpose(inverse(model)))
};

// Vertex Shader Main Function
PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput output;

    float4 posWorld = mul(float4(input.posModel, 1.0f), model);
    output.posWorld = posWorld.xyz;

    float4 posProj = mul(posWorld, view);
    posProj = mul(posProj, projection);
    output.position = posProj;

    output.texCoord = input.texCoord;

    float3 normalWorld = normalize(mul(input.normal, (float3x3) normalM));
    output.normal = normalWorld;

    return output;
}
