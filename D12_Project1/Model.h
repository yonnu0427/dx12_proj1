#pragma once

#include "PMD.h"

#include <d3d12.h>
#include <vector>

struct ModelVertex
{
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 normal;
    DirectX::XMFLOAT2 uv;

    uint32_t boneIndices[4]; // 1頂点にボーン4つを影響させる設定が多いらしいので
    float boneWeights[4];    // 各ボーンがこの頂点に与える重み
};

struct ModelMaterial
{
    DirectX::XMFLOAT3 diffuse = { 1.0f, 1.0f, 1.0f };
    float alpha = 1.0f;

    DirectX::XMFLOAT3 specular = { 0.0f, 0.0f, 0.0f };
    float specularity = 0.0f;

    DirectX::XMFLOAT3 ambient = { 0.0f, 0.0f, 0.0f };

    std::string texPath;

    // PMDでのみ使用する
    std::string sphereTexturePath;
    std::string sphereAddTexturePath;
    std::string toonTexPath;

    bool edgeFlg = false;
};

struct ModelMesh {
    uint32_t indexOffset = 0;
    uint32_t indexCount = 0;

    uint32_t materialIndex = 0;
};

struct ModelNode {
    std::string name;

    int parentIndex;

    // 親ノードから見たTransform
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT4 rotation;
    DirectX::XMFLOAT3 scale;

    std::vector<uint32_t> meshIndices;
};

struct ModelBone {
    uint32_t nodeIndex;
    DirectX::XMFLOAT4X4 inverseBindMatrix;
};

struct ModelSkeleton {
    std::vector<ModelBone> bones;
};

// PMD, FBXをLoadしてこの構造体にする
struct ModelData
{
    std::vector<ModelVertex> vertices; // モデルを構成する頂点の集合
    std::vector<uint32_t> indices;     // 三角形を作るための頂点の順序を示す
    std::vector<ModelMesh> meshes;     // モデルを構成するメッシュの集合
    std::vector<ModelMaterial> materials;
    std::vector<ModelNode> nodes;      // 

    ModelSkeleton skeleton;
};

// モデルとそのメモリを合わせて管理する構造体
struct Model
{
    ModelData data;

    ID3D12Resource* vertBuff = nullptr;
    D3D12_VERTEX_BUFFER_VIEW vbView = {};

    ID3D12Resource* idxBuff = nullptr;
    D3D12_INDEX_BUFFER_VIEW ibView = {};

    ID3D12Resource* materialBuff = nullptr;
    ID3D12DescriptorHeap* materialDescHeap = nullptr;

    std::vector<ID3D12Resource*> textureResources;
    std::vector<ID3D12Resource*> sphResources;
    std::vector<ID3D12Resource*> spaResources;
    std::vector<ID3D12Resource*> toonResources;
};