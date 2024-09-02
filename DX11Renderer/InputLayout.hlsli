#ifndef INPUT_LAYOUT_HLSLI
#define INPUT_LAYOUT_HLSLI

// Vertex Shader Input Structure
struct VertexShaderInput
{
    float3 posModel     : POSITION;
    float3 normal       : NORMAL;
    float2 texCoord     : TEXCOORD0;
};

// Pixel Shader Input Structure
struct PixelShaderInput
{
    float4 position     : SV_POSITION;
    float3 posWorld     : POSITION;
    float3 normal       : NORMAL;
    float2 texCoord     : TEXCOORD0;
};

// Normal Pixel Shader Input Structure
struct NormalPixelShaderInput
{
    float4 position : SV_POSITION;
    float3 color    : COLOR;
};

#endif // INPUT_LAYOUT_HLSLI
