#include "ModelLoader.h"
#include "PMDLoader.h"
#include "FBXLoader.h"
#include "D3D12Context.h"
#include "Texture.h"

#include <d3dx12.h>
#include <string>
#include <algorithm>
#include <filesystem>

static ID3D12Resource* whiteTex = nullptr;
static ID3D12Resource* blackTex = nullptr;
static ID3D12Resource* gradTex = nullptr;

bool LoadModel(Model& model, const std::string& path) {
	if (whiteTex == nullptr) {
		whiteTex = CreateWhiteTexture();
		blackTex = CreateBlackTexture();
		gradTex = CreateGrayGradationTexture();
	}

	std::filesystem::path p = path;

	std::string extension = p.extension().string();


	if (extension == ".pmd") {
		model.data = ConvertPMDToModelData(LoadPMD(path));
	}
	else if (extension == ".fbx" || extension == ".FBX") model.data = LoadFBX(path);
	else						  return false;


	auto& vertices = model.data.vertices;
	auto& indices = model.data.indices;
	auto& materials = model.data.materials;

	unsigned int materialNum = static_cast<unsigned int>(materials.size());

	model.textureResources.resize(materialNum);
	model.sphResources.resize(materialNum);
	model.spaResources.resize(materialNum);
	model.toonResources.resize(materialNum);

	model.vertBuff = nullptr;

	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(vertices.size() * sizeof(ModelVertex));

	auto result = _dev->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&model.vertBuff));

	ModelVertex* vertMap = nullptr;
	result = model.vertBuff->Map(0, nullptr, (void**)&vertMap);
	std::copy(vertices.begin(), vertices.end(), vertMap);
	model.vertBuff->Unmap(0, nullptr);

	model.vbView = {};
	model.vbView.BufferLocation = model.vertBuff->GetGPUVirtualAddress();//バッファの仮想アドレス
	model.vbView.SizeInBytes = static_cast<UINT>(vertices.size() * sizeof(ModelVertex));//全バイト数
	model.vbView.StrideInBytes = sizeof(ModelVertex);//1頂点あたりのバイト数

	{
		for (int i = 0; i < materials.size(); ++i) {
			//トゥーンリソースの読み込み
			if (!materials[i].toonTexPath.empty()) {
				model.toonResources[i] = LoadTextureFromFile(materials[i].toonTexPath);
			}
			else {
				model.toonResources[i] = nullptr;
			}

			if (materials[i].texPath.empty()) {
				model.textureResources[i] = nullptr;
				continue;
			}

			std::string texFileName = materials[i].texPath;
			std::string sphFileName = "";
			std::string spaFileName = "";

			if (std::count(texFileName.begin(), texFileName.end(), '*') > 0) {//スプリッタがある
				auto namepair = SplitFileName(texFileName);
				if (GetExtension(namepair.first) == "sph") {
					texFileName = namepair.second;
					sphFileName = namepair.first;
				}
				else if (GetExtension(namepair.first) == "spa") {
					texFileName = namepair.second;
					spaFileName = namepair.first;
				}
				else {
					texFileName = namepair.first;
					if (GetExtension(namepair.second) == "sph") {
						sphFileName = namepair.second;
					}
					else if (GetExtension(namepair.second) == "spa") {
						spaFileName = namepair.second;
					}
				}
			}
			else {
				if (GetExtension(materials[i].texPath) == "sph") {
					sphFileName = materials[i].texPath;
					texFileName = "";
				}
				else if (GetExtension(materials[i].texPath) == "spa") {
					spaFileName = materials[i].texPath;
					texFileName = "";
				}
				else {
					texFileName = materials[i].texPath;
				}
			}
			//モデルとテクスチャパスからアプリケーションからのテクスチャパスを得る
			if (texFileName != "") {
				auto texFilePath = GetTexturePathFromModelAndTexPath(path, texFileName.c_str());
				model.textureResources[i] = LoadTextureFromFile(texFilePath);
			}
			if (sphFileName != "") {
				auto sphFilePath = GetTexturePathFromModelAndTexPath(path, sphFileName.c_str());
				model.sphResources[i] = LoadTextureFromFile(sphFilePath);
			}
			if (spaFileName != "") {
				auto spaFilePath = GetTexturePathFromModelAndTexPath(path, spaFileName.c_str());
				model.spaResources[i] = LoadTextureFromFile(spaFilePath);
			}
		}

	}




	model.idxBuff = nullptr;
	//設定は、バッファのサイズ以外頂点バッファの設定を使いまわして
	//OKだと思います。
	heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	resDesc = CD3DX12_RESOURCE_DESC::Buffer(indices.size() * sizeof(indices[0]));
	result = _dev->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&model.idxBuff));

	//作ったバッファにインデックスデータをコピー
	uint32_t* mappedIdx = nullptr;
	model.idxBuff->Map(0, nullptr, (void**)&mappedIdx);
	std::copy(indices.begin(), indices.end(), mappedIdx);
	model.idxBuff->Unmap(0, nullptr);

	//インデックスバッファビューを作成
	model.ibView = {};
	model.ibView.BufferLocation = model.idxBuff->GetGPUVirtualAddress();
	model.ibView.Format = DXGI_FORMAT_R32_UINT;
	model.ibView.SizeInBytes = static_cast<UINT>(indices.size() * sizeof(indices[0]));

	//マテリアルバッファを作成
	auto materialBuffSize = sizeof(MaterialForHlsl);
	materialBuffSize = (materialBuffSize + 0xff) & ~0xff;
	heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	resDesc = CD3DX12_RESOURCE_DESC::Buffer(materialBuffSize * materialNum);//勿体ないけど仕方ないですね
	model.materialBuff = nullptr;
	result = _dev->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&model.materialBuff)
	);

	//マップマテリアルにコピー
	char* mapMaterial = nullptr;
	result = model.materialBuff->Map(0, nullptr, (void**)&mapMaterial);
	for (auto& m : materials) {
		MaterialForHlsl material = {};

		material.diffuse = m.diffuse;
		material.alpha = m.alpha;
		material.specular = m.specular;
		material.specularity = m.specularity;
		material.ambient = m.ambient;

		*((MaterialForHlsl*)mapMaterial) = material;

		mapMaterial += materialBuffSize;//次のアライメント位置まで進める
	}
	model.materialBuff->Unmap(0, nullptr);


	model.materialDescHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC materialDescHeapDesc = {};
	materialDescHeapDesc.NumDescriptors = materialNum * 5;//マテリアル数ぶん(定数1つ、テクスチャ3つ)
	materialDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	materialDescHeapDesc.NodeMask = 0;

	materialDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;//デスクリプタヒープ種別
	result = _dev->CreateDescriptorHeap(&materialDescHeapDesc, IID_PPV_ARGS(&model.materialDescHeap));//生成

	D3D12_CONSTANT_BUFFER_VIEW_DESC matCBVDesc = {};
	matCBVDesc.BufferLocation = model.materialBuff->GetGPUVirtualAddress();
	matCBVDesc.SizeInBytes = static_cast<UINT>(materialBuffSize);

	////通常テクスチャビュー作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;//後述
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;//ミップマップは使用しないので1

	auto matDescHeapH = model.materialDescHeap->GetCPUDescriptorHandleForHeapStart();
	auto incSize = _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	for (size_t i = 0; i < materialNum; ++i) {
		//マテリアル固定バッファビュー
		_dev->CreateConstantBufferView(&matCBVDesc, matDescHeapH);
		matDescHeapH.ptr += incSize;
		matCBVDesc.BufferLocation += materialBuffSize;
		if (model.textureResources[i] == nullptr) {
			srvDesc.Format = whiteTex->GetDesc().Format;
			_dev->CreateShaderResourceView(whiteTex, &srvDesc, matDescHeapH);
		}
		else {
			srvDesc.Format = model.textureResources[i]->GetDesc().Format;
			_dev->CreateShaderResourceView(model.textureResources[i], &srvDesc, matDescHeapH);
		}
		matDescHeapH.ptr += incSize;

		if (model.sphResources[i] == nullptr) {
			srvDesc.Format = whiteTex->GetDesc().Format;
			_dev->CreateShaderResourceView(whiteTex, &srvDesc, matDescHeapH);
		}
		else {
			srvDesc.Format = model.sphResources[i]->GetDesc().Format;
			_dev->CreateShaderResourceView(model.sphResources[i], &srvDesc, matDescHeapH);
		}
		matDescHeapH.ptr += incSize;

		if (model.spaResources[i] == nullptr) {
			srvDesc.Format = blackTex->GetDesc().Format;
			_dev->CreateShaderResourceView(blackTex, &srvDesc, matDescHeapH);
		}
		else {
			srvDesc.Format = model.spaResources[i]->GetDesc().Format;
			_dev->CreateShaderResourceView(model.spaResources[i], &srvDesc, matDescHeapH);
		}
		matDescHeapH.ptr += incSize;


		if (model.toonResources[i] == nullptr) {
			srvDesc.Format = gradTex->GetDesc().Format;
			_dev->CreateShaderResourceView(gradTex, &srvDesc, matDescHeapH);
		}
		else {
			srvDesc.Format = model.toonResources[i]->GetDesc().Format;
			_dev->CreateShaderResourceView(model.toonResources[i], &srvDesc, matDescHeapH);
		}
		matDescHeapH.ptr += incSize;

	}

	return true;
}