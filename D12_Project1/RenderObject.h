#pragma once

#include "Model.h"
#include "Transform.h"

#include <d3d12.h>
#include <DirectXMath.h>
#include <string>

struct SceneData
{
    DirectX::XMMATRIX world;
    DirectX::XMMATRIX view;
    DirectX::XMMATRIX proj;
    DirectX::XMFLOAT3 eye;
};

struct RenderObject
{
    Model model;
    Transform transform;

    ID3D12Resource* sceneConstBuff = nullptr;
    SceneData* mapScene = nullptr;
};

bool InitializeRenderObject(
    RenderObject& object,
    const std::string&modelPath
);