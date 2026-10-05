#include "Renderer.h"
#include "D3D12Context.h"

// モデルを描画
void DrawModel(RenderObject& object, ID3D12Resource* cameraConstBuff, ID3D12RootSignature* rootSignature)
{
	Model& model = *object.model;

	// 三角形として描くように設定
	_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	// Modelの頂点バッファ, インデックスバッファをセット
	_cmdList->IASetVertexBuffers(0, 1, &model.vbView);
	_cmdList->IASetIndexBuffer(&model.ibView);

	// ルートシグネチャをセット
	_cmdList->SetGraphicsRootSignature(rootSignature);

	// WVP変換行列
	_cmdList->SetGraphicsRootConstantBufferView(1, cameraConstBuff->GetGPUVirtualAddress());

	// マテリアル
	_cmdList->SetDescriptorHeaps(1, &model.materialDescHeap);

	auto materialHeapStart = model.materialDescHeap->GetGPUDescriptorHandleForHeapStart();

	auto cbvsrvIncSize = _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 5;

	// Nodeごとに描画
	for (uint32_t nodeIdx = 0; nodeIdx < model.data.nodes.size(); ++nodeIdx) {
		const auto& node = model.data.nodes[nodeIdx];

		// このNode用のWorld行列CBをセット
		_cmdList->SetGraphicsRootConstantBufferView(0, object.nodeRenderDatas[nodeIdx].constBuff->GetGPUVirtualAddress());

		for (uint32_t meshIdx : node.meshIndices) {
			const auto& mesh = model.data.meshes[meshIdx];

			auto materialH = materialHeapStart;

			materialH.ptr += static_cast<SIZE_T>(mesh.materialIndex) * cbvsrvIncSize;

			_cmdList->SetGraphicsRootDescriptorTable(2, materialH);

			_cmdList->DrawIndexedInstanced(
				mesh.indexCount,
				1,
				mesh.indexOffset,
				0,
				0
			);
		}
	}
}