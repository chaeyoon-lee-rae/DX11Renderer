# DX11 Renderer

This project is a DirectX 3D 11 renderer built with modularity and expandability in mind. 
The structure is designed to allow easy addition of new visual studio projects and sharing of common resources. 
The renderer demonstrates the use of **DirectX 11** for Graphics API, **DXGI** for a swapchain, **ImGui** for graphical user interfaces, and **HLSL** for shader programming.


## Project Structure

```
DX11Renderer/
│
├── MatrixAnimation/
│   ├── assets/
│   │   └── textures/
│   ├── src/
│   │   ├── include/
│   │   │   └── GenGeo.h
│   │   ├── GenGeo.cpp
│   │   ├── Main.cpp
│   │   └── Renderer.cpp
│   ├── MatrixAnimation.filters
│   ├── MatrixAnimation.user
│   ├── MatrixAnimation.vcxproj
│   ├── MatrixAnimation.vcxproj.filters
│   └── imgui.ini
│
├── shared/
│   ├── config/
│   │   └── .clang-format
│   ├── shaders/
│   │   ├── InputLayout.hlsli
│   │   ├── MeshPixelShader.hlsl
│   │   ├── MeshVertexShader.hlsl
│   │   ├── NormalPixelShader.hlsl
│   │   ├── NormalVertexShader.hlsl
│   │   └── PhongConstants.hlsli
│   └── src/
│       ├── include/
│       │   ├── CheckHr.h
│       │   ├── ConstantBuffers.h
│       │   ├── D3DUtils.h
│       │   ├── Mesh.h
│       │   ├── Renderer.h
│       │   ├── Vertex.h
│       │   └── WinApp.h
│       ├── D3DUtils.cpp
│       └── WinApp.cpp
│
└── DX11Renderer.sln
```
## MatrixAnimation Project
<img alt="image" src="https://github.com/user-attachments/assets/73148c98-158a-4a2d-a595-273516b152fa">

The MatrixAnimation project contains the main renderer implementation, which includes:
* Main.cpp: Sets up the main window and starts the rendering loop.
* Renderer.cpp: Handles initialization of Direct3D 11 and ImGui, creating and rendering meshes.
* GenGeo.cpp: Generates geometry for rendering.

## Shared Resources
The shared directory contains reusable resources:

* Shaders: A collection of HLSL shaders used for rendering.
* Utilities: D3D utility functions to assist with device creation, buffer management, etc.

## Features
* Blinn-Phong Lighting: Implements directional, point, and spotlights using the Blinn-Phong shading model in MeshPixelShader.hlsl.
* Mesh Generation: The GenGeo utility allows for creating procedural 3D geometry.
* Texture Support: Loads and applies textures to models.
* Normal Rendering: Visualizes vertex normals for debugging and educational purposes.
* ImGui Integration: Provides a GUI for scene control and real-time parameter tweaking.

## Usage
[Demonstration](https://drive.google.com/file/d/1xgxHAesuQAQ7b6cVwlYy3Bjw5E_C550p/view?usp=drive_link)
* The application launches a window with a D3D11 renderer.
* Use the ImGui control panel to modify rendering parameters such as transformations, material properties, lighting options, etc.
* The project supports wireframe rendering, normal visualization, and texture mapping.

### Key Controls
<img width="341" alt="image" src="https://github.com/user-attachments/assets/4035d121-3758-46d4-aa79-3fe46ecd620f">

* Use Texture: Toggle between applying texture or using only material colors.
* Wireframe Mode: Enable wireframe rendering for debugging.
* Normal Scale: Adjust the length of normal vectors when visualizing normals.
* Material Properties: Modify the diffuse, specular, and shininess of the material using sliders.
* Light Settings: Adjust the position, falloff, and intensity of the directional, point, and spotlights.

## Build Instructions
### Requirements
* Visual Studio 2019 or later
* Windows SDK
* DirectX 11
* Dear ImGui

### Steps to Build
1. Clone the repository.
2. Open the DX11Renderer.sln solution file in Visual Studio.
3. Set MatrixAnimation as the startup project.
4. Build and run the project.
