#include "PMD.h"
#include "PMDLoader.h"

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


	fclose(fp);

	ret.header = pmdheader;
	ret.vertices = vertices;
	ret.indices = indices;
	ret.materials = materials;

	return ret;
}