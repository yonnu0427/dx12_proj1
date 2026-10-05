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

// FBXモデルにおける各ノードの情報を表す構造体
struct NodePose
{
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT4 rotation;
    DirectX::XMFLOAT3 scale;

    DirectX::XMFLOAT4X4 globalMatrix;
};

struct NodeRenderData
{
    ID3D12Resource* constBuff = nullptr;
    ObjectData* mappedData = nullptr;
};

struct RenderObject
{
    Model* model = nullptr;
    Transform transform;

    std::vector<NodePose> nodePoses;
    std::vector<NodeRenderData> nodeRenderDatas;

    ID3D12Resource* objectConstBuff = nullptr;
    ObjectData* mapScene = nullptr;
};

bool InitializeRenderObject(RenderObject& object, Model* model);
void UpdateNodeMatrices(RenderObject& object);
void UpdateRenderObjectMatrices(RenderObject& object);