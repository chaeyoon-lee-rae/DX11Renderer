#include "InputLayout.hlsli"
#include "PhongConstants.hlsli"

// Texture and Sampler
Texture2D g_texture0 : register(t0);
SamplerState g_sampler0 : register(s0);

// Constant Buffer for Pixel Shader
cbuffer BasicPixelConstantBuffer : register(b0)
{
    Material material; // Material properties
    Light light[NUM_LIGHTS]; // Array of lights
    float3 cameraPos;
    float useTexture;
};

// Blinn-Phong Lighting Model
float3 BlinnPhong(float3 lightIntensity, float3 lightDir, float3 normal,
                 float3 viewDir, Material mat)
{
    float3 halfVec = normalize(viewDir + lightDir);
    
    float spec = 0.0f;
    if (length(mat.diffuse) > 0.0f)
    {
        spec = pow(max(dot(halfVec, normal), 0.0f), mat.shininess);
    }
    float3 specular = mat.specular * spec;

    return mat.ambient + (mat.diffuse + specular) * lightIntensity;
}

// Compute Directional Light
float3 DirectionalLight(Light L, Material mat, float3 normal,
                               float3 viewDir)
{
    float3 lightDir = -normalize(L.direction);

    float ndotl = max(dot(lightDir, normal), 0.0f);
    float3 lightIntensity = L.intensity * ndotl;

    return BlinnPhong(lightIntensity, lightDir, normal, viewDir, mat);
}

// Calculate Attenuation based on distance
float Attenuation(float distance, float falloffStart, float falloffEnd)
{
    return saturate((falloffEnd - distance) / (falloffEnd - falloffStart));
}

// Compute Point Light
float3 PointLight(Light L, Material mat, float3 pos, float3 normal,
                         float3 viewDir)
{
    float3 lightDir = L.position - pos;
    float distance = length(lightDir);

    float3 blinnPhongRet = float3(0.0, 0.0, 0.0);
    
    if (distance <= L.falloffEnd)
    {
        lightDir = normalize(lightDir);
        float ndotl = max(dot(lightDir, normal), 0.0f);
        float3 lightIntensity = L.intensity * ndotl;

        lightIntensity *= Attenuation(distance, L.falloffStart, L.falloffEnd);

        blinnPhongRet = BlinnPhong(lightIntensity, lightDir, normal, viewDir, mat);
    }
    
    return blinnPhongRet;
}

// Compute Spot Light
float3 SpotLight(Light L, Material mat, float3 pos, float3 normal,
                        float3 viewDir)
{
    float3 lightDir = L.position - pos;
    float distance = length(lightDir);

    if (distance > L.falloffEnd)
    {
        return float3(0.0f, 0.0f, 0.0f);
    }
    else
    {
        lightDir = normalize(lightDir);
        float ndotl = max(dot(lightDir, normal), 0.0f);
        float3 lightIntensity = L.intensity * ndotl;

        float attenuation = Attenuation(distance, L.falloffStart, L.falloffEnd);
        lightIntensity *= attenuation;

        float spotFactor = pow(max(-dot(lightDir, normalize(L.direction)), 0.0f), L.spotPower);
        lightIntensity *= spotFactor;

        return BlinnPhong(lightIntensity, lightDir, normal, viewDir, mat);
    }
}

// Pixel Shader Main Function
float4 main(PixelShaderInput input) : SV_TARGET
{
    float3 viewDir = normalize(cameraPos - input.posWorld);
    
    //float3 color = 0.5f + 0.5f * cos(input.texCoord.xyx + float3(0.0f, 1.0f, 2.0f));
    float3 color = float3(0.0, 0.0, 0.0);

    // Instead of using if-else statements, set the contribution of the unchosen two light types to 0
    // Checkout Renderer::Update()
    [unroll]
    for (int i = 0; i < DIR_LIGHTS; ++i)
    {
        color += DirectionalLight(light[i], material, input.normal, viewDir);
    }
    
    [unroll]
    for (int i = DIR_LIGHTS; i < DIR_LIGHTS + POINT_LIGHTS; ++i)
    {
        color += PointLight(light[i], material, input.posWorld, input.normal, viewDir);
    }
    
    [unroll]
    for (int i = DIR_LIGHTS + POINT_LIGHTS; i < NUM_LIGHTS; ++i)
    {
        color += SpotLight(light[i], material, input.posWorld, input.normal, viewDir);
    }

    // Apply texture if enabled
    float4 textureColor = g_texture0.Sample(g_sampler0, input.texCoord);
    return useTexture > 0.5f ? float4(color, 1.0f) * textureColor : float4(color, 1.0f);
}
