#include "InputLayout.hlsli"

float4 main(NormalPixelShaderInput input) : SV_TARGET
{
    return float4(input.color, 1.0f);
}
