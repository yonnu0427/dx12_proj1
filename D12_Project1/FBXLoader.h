#pragma once

#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>

void ReadNode(aiNode* aiNode, int parentIndex, ModelData& modelData);
ModelData LoadFBX(const std::string& filePath);
int FindNodeIndex(const ModelData& modelData, const std::string& name);
void AddBoneWeight(ModelVertex& vertex, uint32_t boneIndex, float weight);

// AssimpÇ∆DirectXMathÇ≈çsóÒÇÃàµÇ¢Ç™à·Ç§ÇÃÇ≈ì]íu
DirectX::XMFLOAT4X4 ConvertAssimpMatrix(const aiMatrix4x4& m);