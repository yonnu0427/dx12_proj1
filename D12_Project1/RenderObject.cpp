#include "RenderObject.h"

#include "ModelLoader.h"
#include "D3D12Context.h"

#include <d3dx12.h>

// 描画するオブジェクトの初期化を行う
bool InitializeRenderObject(RenderObject& object, Model* model) {
	object.model = model;

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