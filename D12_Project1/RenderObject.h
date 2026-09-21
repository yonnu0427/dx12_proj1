#pragma once

#include "Model.h"
#include "Transform.h"

#include <d3d12.h>
#include <DirectXMath.h>
#include <string>

struct ObjectData
{
    DirectX::XMMATRIX world;
};

struct RenderObject
{
    Model* model = nullptr;
    Transform transform;

    ID3D12Resource* objectConstBuff = nullptr;
    ObjectData* mapScene = nullptr;
};

bool InitializeRenderObject(RenderObject& object, Model* model);