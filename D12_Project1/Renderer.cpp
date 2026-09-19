#include "Renderer.h"
#include "D3D12Context.h"

void DrawModel(Model& model, ID3D12Resource* objectConstBuff, ID3D12Resource* cameraConstBuff, ID3D12RootSignature* rootSignature)
{
	// 三角形として描くように設定
	_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	// Modelの頂点バッファ, インデックスバッファをセット
	_cmdList->IASetVertexBuffers(0, 1, &model.vbView);
	_cmdList->IASetIndexBuffer(&model.ibView);

	// ルートシグネチャをセット
	_cmdList->SetGraphicsRootSignature(rootSignature);

	// WVP変換行列
	_cmdList->SetGraphicsRootConstantBufferView(0, objectConstBuff->GetGPUVirtualAddress());

	_cmdList->SetGraphicsRootConstantBufferView(1, cameraConstBuff->GetGPUVirtualAddress());

	// マテリアル
	_cmdList->SetDescriptorHeaps(1, &model.materialDescHeap);

	auto materialH = model.materialDescHeap->GetGPUDescriptorHandleForHeapStart();
	unsigned int idxOffset = 0;

	auto cbvsrvIncSize = _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 5;
	for (auto& m : model.data.materials) {
		_cmdList->SetGraphicsRootDescriptorTable(2, materialH);
		_cmdList->DrawIndexedInstanced(m.indicesNum, 1, idxOffset, 0, 0);
		materialH.ptr += cbvsrvIncSize;
		idxOffset += m.indicesNum;
	}
}