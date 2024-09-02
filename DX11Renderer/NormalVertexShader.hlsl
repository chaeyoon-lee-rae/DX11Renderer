#include "InputLayout.hlsli"

// Constant Buffer for Vertex Shader
cbuffer MeshVertexConstantBuffer : register(b0)
{
    float4x4 model;     // Model matrix
    float4x4 view;      // View matrix
    float4x4 projection; // Projection matrix
    float4x4 normalM;   // Normal matrix (transpose(inverse(model)))
};

// Constant Buffer for Normal Vertex Shader
cbuffer NormalVertexConstantBuffer : register(b1)
{
    float scale;    // Scale factor for normal visualization
    float3 padding; // Padding for 16-byte alignment
};

// Normal Vertex Shader Main Function
NormalPixelShaderInput main(VertexShaderInput input)
{
    NormalPixelShaderInput output;

    float4 posWorld = mul(float4(input.posModel, 1.0f), model);
    float3 normalWorld = normalize(mul(input.normal, (float3x3) normalM));

    float t = input.texCoord.x; // Using u-coordinate as a scaling factor
    posWorld.xyz += normalWorld * t * scale;

    float4 posProj = mul(posWorld, view);
    posProj = mul(posProj, projection);
    output.position = posProj;

    // Interpolate color between green and red based on 't'
    float3 startColor = float3(0.0f, 1.0f, 0.0f); // Green
    float3 endColor = float3(1.0f, 0.0f, 0.0f);   // Red
    output.color = lerp(startColor, endColor, t);

    return output;
}
