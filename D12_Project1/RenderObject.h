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

    std::vector<DirectX::XMFLOAT4X4> skinMatrices;

    // 後でフレームごとのバッファに統合する
    ID3D12Resource* skinMatBuff = nullptr;
    DirectX::XMFLOAT4X4* mappedSkinMatrices = nullptr; // CPUからskinMatBuffへ書き込むためのポインタ

    ID3D12Resource* objectConstBuff = nullptr;
    ObjectData* mapScene = nullptr;
};

bool InitializeRenderObject(RenderObject& object, Model* model);
void UpdateNodeMatrices(RenderObject& object);
void UpdateRenderObjectMatrices(RenderObject& object);
void UpdateSkinMatrices(RenderObject& object);