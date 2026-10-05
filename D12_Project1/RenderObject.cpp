#include "RenderObject.h"

#include "ModelLoader.h"
#include "D3D12Context.h"

#include <d3dx12.h>

// 描画するオブジェクトの初期化を行う
bool InitializeRenderObject(RenderObject& object, Model* model) {
	object.model = model;

	object.nodePoses.resize(model->data.nodes.size());

	for (size_t i = 0; i < model->data.nodes.size(); ++i) {
		const ModelNode& node = model->data.nodes[i];

		NodePose& pose = object.nodePoses[i];

		pose.position = node.position;
		pose.rotation = node.rotation;
		pose.scale = node.scale;

		DirectX::XMStoreFloat4x4(&pose.globalMatrix, DirectX::XMMatrixIdentity());
	}

	UpdateNodeMatrices(object);

	object.nodeRenderDatas.resize(model->data.nodes.size());

	for (size_t i = 0; i < object.nodeRenderDatas.size(); ++i) {
		NodeRenderData& renderData = object.nodeRenderDatas[i];

		auto nodeHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

		auto nodeResDesc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(ObjectData) + 0xff) & ~0xff);

		auto result = _dev->CreateCommittedResource(
			&nodeHeapProp,
			D3D12_HEAP_FLAG_NONE,
			&nodeResDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&renderData.constBuff)
		);

		if (FAILED(result)) {
			return false;
		}

		result = renderData.constBuff->Map(
			0,
			nullptr,
			reinterpret_cast<void**>(&renderData.mappedData)
		);

		if (FAILED(result))
		{
			return false;
		}
	}

	UpdateRenderObjectMatrices(object);

	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	auto resDesc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(ObjectData) + 0xff) & ~0xff);

	auto result = _dev->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&object.objectConstBuff)
	);

	if (FAILED(result))
	{
		return false;
	}

	result = object.objectConstBuff->Map(
		0,
		nullptr,
		reinterpret_cast<void**>(
			&object.mapScene
			)
	);

	if (FAILED(result))
	{
		return false;
	}

	return true;
}

static DirectX::XMMATRIX GetNodePoseLocalMatrix(const NodePose& pose)
{
	using namespace DirectX;

	XMMATRIX scaleMatrix = XMMatrixScaling(
		pose.scale.x,
		pose.scale.y,
		pose.scale.z
	);

	XMVECTOR rotation = XMLoadFloat4(&pose.rotation);

	XMMATRIX rotationMatrix =
		XMMatrixRotationQuaternion(rotation);

	XMMATRIX translationMatrix = XMMatrixTranslation(
		pose.position.x,
		pose.position.y,
		pose.position.z
	);

	return scaleMatrix * rotationMatrix * translationMatrix;
}


void UpdateNodeMatrices(RenderObject& object)
{
	using namespace DirectX;

	const auto& nodes = object.model->data.nodes;

	for (size_t i = 0; i < nodes.size(); ++i)
	{
		const ModelNode& node = nodes[i];
		NodePose& pose = object.nodePoses[i];

		XMMATRIX localMatrix =
			GetNodePoseLocalMatrix(pose);

		XMMATRIX globalMatrix;

		if (node.parentIndex < 0)
		{
			// Root Node
			globalMatrix = localMatrix;
		}
		else
		{
			XMMATRIX parentGlobal =
				XMLoadFloat4x4(
					&object.nodePoses[node.parentIndex].globalMatrix
				);

			globalMatrix =
				localMatrix * parentGlobal;
		}

		XMStoreFloat4x4(
			&pose.globalMatrix,
			globalMatrix
		);
	}
}

void UpdateRenderObjectMatrices(RenderObject& object) {
	using namespace DirectX;

	// アニメーションでNodePoseが変わっている可能性があるのでNodeのGlobalMatrixを更新
	UpdateNodeMatrices(object);

	// オブジェクト全体のWorldMatrix
	XMMATRIX objectWorld = object.transform.GetWorldMatrix();

	for (size_t i = 0; i < object.nodePoses.size(); ++i) {
		XMMATRIX nodeGlobal = XMLoadFloat4x4(&object.nodePoses[i].globalMatrix);

		XMMATRIX world = nodeGlobal * objectWorld;

		object.nodeRenderDatas[i].mappedData->world = world;
	}
}