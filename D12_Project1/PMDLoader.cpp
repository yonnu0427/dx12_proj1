#include "PMDLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <cstdio>

// PMDモデルを読み込む
PMDData LoadPMD(const std:: string& path) {
	PMDData ret;

	char signature[3];
	PMDHeader pmdheader = {};

	FILE* fp;

	if (fopen_s(&fp, path.c_str(), "rb") != 0 || fp == nullptr) {
		return ret;
	}

	fread(signature, sizeof(signature), 1, fp);
	fread(&pmdheader, sizeof(pmdheader), 1, fp);

	unsigned int vertNum;//頂点数
	fread(&vertNum, sizeof(vertNum), 1, fp);


	constexpr unsigned int pmdvertex_size = 38;//頂点1つあたりのサイズ
	std::vector<PMD_VERTEX> vertices(vertNum);//バッファ確保
	for (auto i = 0; i < vertNum; i++)
	{
		fread(&vertices[i], pmdvertex_size, 1, fp);
	}

	unsigned int indicesNum;//インデックス数
	fread(&indicesNum, sizeof(indicesNum), 1, fp);//

	std::vector<unsigned short> indices(indicesNum);
	fread(indices.data(), indices.size() * sizeof(indices[0]), 1, fp);//一気に読み込み

	unsigned int materialNum;//マテリアル数
	fread(&materialNum, sizeof(materialNum), 1, fp);
	std::vector<Material> materials(materialNum);

	std::vector<PMDMaterial> pmdMaterials(materialNum);
	fread(pmdMaterials.data(), pmdMaterials.size() * sizeof(PMDMaterial), 1, fp);

	for (int i = 0; i < pmdMaterials.size(); ++i) {
		materials[i].indicesNum = pmdMaterials[i].indicesNum;
		materials[i].material.diffuse = pmdMaterials[i].diffuse;
		materials[i].material.alpha = pmdMaterials[i].alpha;
		materials[i].material.specular = pmdMaterials[i].specular;
		materials[i].material.specularity = pmdMaterials[i].specularity;
		materials[i].material.ambient = pmdMaterials[i].ambient;

		materials[i].additional.toonIdx = pmdMaterials[i].toonIdx;
		materials[i].additional.edgeFlg = pmdMaterials[i].edgeFlg;
		materials[i].additional.texPath = pmdMaterials[i].texFilePath;
	}

	uint16_t boneCount = 0;

	fread(&boneCount, sizeof(boneCount), 1, fp);

	ret.bones.resize(boneCount);

	fread(ret.bones.data(), sizeof(PMDBone), boneCount, fp);


	fclose(fp);

	ret.header = pmdheader;
	ret.vertices = vertices;
	ret.indices = indices;
	ret.materials = materials;

	return ret;
}

ModelData ConvertPMDToModelData(const PMDData& pmdData) {
	ModelData ret;

	ret.vertices.reserve(pmdData.vertices.size());

	for (const auto& pmdVertex : pmdData.vertices)
	{
		ModelVertex vertex = {};
		vertex.position = pmdVertex.pos;
		vertex.normal = pmdVertex.normal;
		vertex.uv = pmdVertex.uv;

		vertex.boneIndices[0] = pmdVertex.bone_no[0];
		vertex.boneIndices[1] = pmdVertex.bone_no[1];
		vertex.boneIndices[2] = 0;
		vertex.boneIndices[3] = 0;

		float weight0 = static_cast<float>(pmdVertex.weight) / 100.0f; // PMDには0~100で書かれているので、0~1に圧縮する

		vertex.boneWeights[0] = weight0;
		vertex.boneWeights[1] = 1.0f - weight0;

		ret.vertices.push_back(vertex);
	}

	for (auto& index : pmdData.indices)
	{
		ret.indices.push_back(index);
	}


	uint32_t indexOffset = 0;
	for (uint32_t i = 0; i < pmdData.materials.size(); ++i)
	{
		const auto& pmdMaterial = pmdData.materials[i];

		ModelMaterial material = {};

		material.diffuse = pmdMaterial.material.diffuse;
		material.alpha = pmdMaterial.material.alpha;
		material.specular = pmdMaterial.material.specular;
		material.specularity = pmdMaterial.material.specularity;
		material.ambient = pmdMaterial.material.ambient;

		material.edgeFlg = pmdMaterial.additional.edgeFlg;
		material.texPath = pmdMaterial.additional.texPath;

		char toonFileName[16];

		sprintf_s(
			toonFileName,
			sizeof(toonFileName),
			"toon%02d.bmp",
			pmdMaterial.additional.toonIdx + 1
		);

		material.toonTexPath = "toon/" + std::string(toonFileName);

		ret.materials.push_back(material);

		ModelMesh mesh = {};

		mesh.indexOffset = indexOffset;
		mesh.indexCount = pmdMaterial.indicesNum;
		mesh.materialIndex = i;

		ret.meshes.push_back(mesh);

		indexOffset += pmdMaterial.indicesNum;
	}

	// PMDには描画用NodeがないのでRootNodeを作る
	ModelNode rootNode = {};

	rootNode.name = "RootNode";
	rootNode.parentIndex = -1;
	rootNode.position = { 0.0f, 0.0f, 0.0f };
	rootNode.rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
	rootNode.scale = { 1.0f, 1.0f, 1.0f };

	for (uint32_t i = 0; i < ret.meshes.size(); i++) {
		rootNode.meshIndices.push_back(i);
	}

	ret.nodes.push_back(rootNode);

	const uint32_t boneNodeStart = static_cast<uint32_t>(ret.nodes.size());

	// PMDのボーン情報からNodeを作成
	for (uint32_t i = 0; i < pmdData.bones.size(); ++i)
	{
		const PMDBone& pmdBone = pmdData.bones[i];

		ModelNode node = {};

		node.name = pmdBone.name;
		node.rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
		node.scale = { 1.0f, 1.0f, 1.0f };

		if (pmdBone.parentBoneIndex == UINT16_MAX)
		{
			// 親がなければRootNodeの子に
			node.parentIndex = 0;
			node.position = pmdBone.position;
		}
		else
		{
			node.parentIndex = static_cast<int>(boneNodeStart + pmdBone.parentBoneIndex);
			const auto& parent = pmdData.bones[pmdBone.parentBoneIndex];

			// PMDのBone座標はモデル空間座標なので親からの相対座標に変換
			node.position = {
				pmdBone.position.x - parent.position.x,
				pmdBone.position.y - parent.position.y,
				pmdBone.position.z - parent.position.z,
			};
		}

		ret.nodes.push_back(node);
	}

	for (uint32_t i = 0; i < pmdData.bones.size(); ++i) 
	{
		const PMDBone& pmdBone = pmdData.bones[i];

		ModelBone bone = {};

		// nodes[0]はRootNodeなので+1
		bone.nodeIndex = boneNodeStart + i;

		// PMDのBoneの座標はモデル空間上のBind座標
		DirectX::XMMATRIX inverseBind = DirectX::XMMatrixTranslation(
			-pmdBone.position.x,
			-pmdBone.position.y,
			-pmdBone.position.z
		);

		DirectX::XMStoreFloat4x4(&bone.inverseBindMatrix, inverseBind);

		const ModelNode& boneNode = ret.nodes[bone.nodeIndex];

		DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(
			boneNode.scale.x,
			boneNode.scale.y,
			boneNode.scale.z
		);

		DirectX::XMMATRIX rotationMatrix(DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&boneNode.rotation)));

		DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation(
			boneNode.position.x,
			boneNode.position.y,
			boneNode.position.z
		);

		DirectX::XMMATRIX bindLocal = scaleMatrix * rotationMatrix * translationMatrix;

		DirectX::XMStoreFloat4x4(&bone.bindLocalMatrix, bindLocal);



		ret.skeleton.bones.push_back(bone);
	}


	return ret;
}