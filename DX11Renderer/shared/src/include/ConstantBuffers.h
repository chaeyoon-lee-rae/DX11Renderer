#pragma once

#include <vector>
#include <directxtk/SimpleMath.h>

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector2;
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Vector4;

// Phong Shader Constants
struct Material {
    Vector3 amb = Vector3(0.1f);    // 12 bytes
    float dummy1;                   // 4
    Vector3 diff = Vector3(0.5f);   // 12
    float dummy2;                   // 4
    Vector3 spec = Vector3(0.5f);   // 12
    float shininess = 1.0f;         // 4
};

struct Light {
    Vector3 direction = Vector3(0.0f, 0.0f, 1.0f);  // 12
    float fallOffStart = 0.0f;                      // 4
    Vector3 position = Vector3(0.0f, 0.0f, -2.0f);  // 12
    float fallOffEnd = 10.0f;                       // 4
    Vector3 strength = Vector3(1.0f);               // 12
    float spotPower = 1.0f;                         // 4
};

// Constant Buffers
// Mesh
struct vertexConstantBuffer {
    Matrix model;       // 64
    Matrix view;        // 64
    Matrix projection;  // 64
    Matrix normalM;     // 64
};

struct pixelConstantBuffer {
    Material material;  // 48
    Light lights[3];    // 48
    Vector3 camera;     // 12
    float useTexture;   // 4
};

// Normal
struct normalVertexConstantBuffer {
    float scale = 0.1f; // 4
    float dummy[3];     // 12
};


// Check size
static_assert((sizeof(Material) % 16) == 0, 
               "Material % 16 != 0");
static_assert((sizeof(Light) % 16) == 0, 
               "Light % 16 != 0");
static_assert((sizeof(vertexConstantBuffer) % 16) == 0, 
               "vertexConstantBuffer % 16 != 0");
static_assert((sizeof(pixelConstantBuffer) % 16) == 0, 
               "pixelConstantBuffer % 16 != 0");
static_assert((sizeof(normalVertexConstantBuffer) % 16) == 0, 
               "normalVertexConstantBuffer % 16 != 0");