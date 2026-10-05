#include "FBXLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <unordered_map>
#include <filesystem>

ModelData LoadFBX(const std::string& path) {
	ModelData ret;

    Assimp::Importer importer;

    // Assimpのオプション
    unsigned int flag = 0;
    flag |= aiProcess_Triangulate;
    flag |= aiProcess_JoinIdenticalVertices;
    flag |= aiProcess_GenSmoothNormals;
    flag |= aiProcess_ConvertToLeftHanded;

    const aiScene* scene = importer.ReadFile(path, flag);

    if (scene == nullptr || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || scene->mRootNode == nullptr)
    {
        const char* error = importer.GetErrorString();
        return ret;
    }

    std::filesystem::path modelPath(path);
    std::filesystem::path modelDirectory = modelPath.parent_path();

    ReadNode(scene->mRootNode, -1, ret);

    for (unsigned int materialIdx = 0; materialIdx < scene->mNumMaterials; ++materialIdx) {
        aiMaterial* aiMat = scene->mMaterials[materialIdx];

        ModelMaterial material = {};

        aiColor3D diffuse;
        if (aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse) == AI_SUCCESS) {
            material.diffuse = {
                diffuse.r,
                diffuse.g,
                diffuse.b
            };
        }

        aiColor3D specular;
        if (aiMat->Get(AI_MATKEY_COLOR_SPECULAR, specular) == AI_SUCCESS) {
            material.specular = {
                specular.r,
                specular.g,
                specular.b
            };
        }

        aiColor3D ambient;
        if (aiMat->Get(AI_MATKEY_COLOR_AMBIENT, ambient) == AI_SUCCESS) {
            material.ambient = {
                ambient.r,
                ambient.g,
                ambient.b
            };
        }

        float opacity = 1.0f;
        if (aiMat->Get(AI_MATKEY_OPACITY, opacity) == AI_SUCCESS) {
            material.alpha = opacity;
        }

        float shininess = 0.0f;
        if (aiMat->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS) {
            material.specularity = shininess;
        }

        aiString texPath;

        if (aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS)
        {
            std::filesystem::path texturePath = std::filesystem::path(texPath.C_Str());

            if (texturePath.is_relative())
            {
                texturePath = modelDirectory / texturePath;
            }

            material.texPath = texturePath.lexically_normal().string();

        }

        ret.materials.push_back(material);
    }

    std::unordered_map<std::string, uint32_t> boneMap;

    for (unsigned int meshIdx = 0; meshIdx < scene->mNumMeshes; ++meshIdx) {
        aiMesh* mesh = scene->mMeshes[meshIdx];

        ModelMesh modelMesh = {};

        modelMesh.indexOffset = static_cast<uint32_t>(ret.indices.size());

        modelMesh.materialIndex = mesh->mMaterialIndex;

        uint32_t vertexOffset = static_cast<uint32_t>(ret.vertices.size());
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
            ModelVertex vertex = {};

            vertex.position = {
                mesh->mVertices[i].x,
                mesh->mVertices[i].y,
                mesh->mVertices[i].z
            };

            if (mesh->HasNormals()) {
                vertex.normal = {
                    mesh->mNormals[i].x,
                    mesh->mNormals[i].y,
                    mesh->mNormals[i].z
                };
            }

            if (mesh->HasTextureCoords(0)) {
                vertex.uv = {
                    mesh->mTextureCoords[0][i].x,
                    mesh->mTextureCoords[0][i].y,
                };
            }

            vertex.boneIndices[0] = 0;
            vertex.boneIndices[1] = 0;

            ret.vertices.push_back(vertex);
        }


        for (unsigned int boneIdx = 0; boneIdx < mesh->mNumBones; ++boneIdx)
        {
            aiBone* aiBone = mesh->mBones[boneIdx];

            std::string boneName = aiBone->mName.C_Str();

            uint32_t modelBoneIndex = FindNodeIndex(ret, boneName);

            auto boneIndex = boneMap.find(boneName);

            if (boneIndex == boneMap.end())
            {
                int nodeIndex = FindNodeIndex(ret, boneName);

                if (nodeIndex < 0) continue;
            
                ModelBone bone = {};

                bone.nodeIndex = static_cast<uint32_t>(nodeIndex);

                bone.inverseBindMatrix = ConvertAssimpMatrix(aiBone->mOffsetMatrix);

                modelBoneIndex = static_cast<uint32_t>(ret.skeleton.bones.size());

                ret.skeleton.bones.push_back(bone);

                boneMap[boneName] = modelBoneIndex;
            }
            else
            {
                modelBoneIndex = boneIndex->second;
            }

            for (unsigned int weightIdx = 0; weightIdx < aiBone->mNumWeights; ++weightIdx) {
                const aiVertexWeight& weight = aiBone->mWeights[weightIdx];

                uint32_t vertexIndex = vertexOffset + weight.mVertexId;

                AddBoneWeight(ret.vertices[vertexIndex], modelBoneIndex, weight.mWeight);
            }
        }

        for (unsigned int faceIdx = 0; faceIdx < mesh->mNumFaces; ++faceIdx)
        {
            aiFace face = mesh->mFaces[faceIdx];
            for (unsigned int indiceIdx = 0; indiceIdx < face.mNumIndices; ++indiceIdx) {
                ret.indices.push_back(vertexOffset + face.mIndices[indiceIdx]);
            }
        }

        modelMesh.indexCount = static_cast<uint32_t>(ret.indices.size()) - modelMesh.indexOffset;

        ret.meshes.push_back(modelMesh);
    }

	return ret;
}

void ReadNode(aiNode* aiNode, int parentIndex, ModelData& modelData) {
    ModelNode node = {};

    node.name = aiNode->mName.C_Str();
    node.parentIndex = parentIndex;

    aiVector3D scale;
    aiQuaternion rotation;
    aiVector3D position;

    aiNode->mTransformation.Decompose(scale, rotation, position);

    node.position = {
        position.x,
        position.y,
        position.z
    };

    node.rotation = {
        rotation.x,
        rotation.y,
        rotation.z,
        rotation.w
    };

    node.scale = {
        scale.x,
        scale.y,
        scale.z
    };

    for (unsigned int i = 0; i < aiNode->mNumMeshes; ++i) {
        node.meshIndices.push_back(aiNode->mMeshes[i]);
    }

    int nodeIndex = static_cast<int>(modelData.nodes.size());
    modelData.nodes.push_back(node);

    for (unsigned int i = 0; i < aiNode->mNumChildren; ++i) {
        ReadNode(aiNode->mChildren[i], nodeIndex, modelData);
    }
}

int FindNodeIndex(const ModelData& modelData, const std::string& name)
{
    for (int i = 0; i < modelData.nodes.size(); ++i)
    {
        if (modelData.nodes[i].name == name)
        {
            return i;
        }
    }

    return -1;
}

void AddBoneWeight(ModelVertex& vertex, uint32_t boneIndex, float weight)
{
    // 重い順に最大4つ残す
    for (int i = 0; i < 4; ++i) {
        if (weight > vertex.boneWeights[i])
        {
            for (int j = 3; j > i; --j) {
                vertex.boneWeights[j] = vertex.boneWeights[j - 1];
                vertex.boneIndices[j] = vertex.boneIndices[j - 1];
            }

            vertex.boneWeights[i] = weight;
            vertex.boneIndices[i] = boneIndex;

            return;
        }
    }
}

DirectX::XMFLOAT4X4 ConvertAssimpMatrix(const aiMatrix4x4& m)
{
    return DirectX::XMFLOAT4X4(
        m.a1, m.b1, m.c1, m.d1,
        m.a2, m.b2, m.c2, m.d2,
        m.a3, m.b3, m.c3, m.d3,
        m.a4, m.b4, m.c4, m.d4
    );
}