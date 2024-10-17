#ifndef PHONG_CONSTANTS_HLSLI
#define PHONG_CONSTANTS_HLSLI

// Maximum number of lights supported
#define NUM_LIGHTS       3
#define DIR_LIGHTS       1
#define POINT_LIGHTS     1
#define SPOT_LIGHTS      1

// Material Structure
struct Material
{
    float3 ambient;  // Ambient color
    float padding1;  // Padding for 16-byte alignment
    float3 diffuse;  // Diffuse color
    float padding2;  // Padding for 16-byte alignment
    float3 specular; // Specular color
    float shininess; // Shininess factor
};

// Light Structure
struct Light
{
    float3 direction;    // Direction for directional lights
    float  falloffStart; // Start distance for attenuation
    float3 position;     // Position for point/spot lights
    float  falloffEnd;   // End distance for attenuation
    float3 intensity;    // Light intensity (color)
    float  spotPower;     // Spotlight power factor
};

#endif // PHONG_CONSTANTS_HLSLI
