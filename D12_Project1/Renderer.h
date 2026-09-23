#pragma once

#include "Model.h"

#include <d3d12.h>

// ƒ‚ƒfƒ‹‚Ì•`‰æ
void DrawModel(
    Model& model,
    ID3D12Resource* objectConstBuff,
    ID3D12Resource* cameraConstBuff,
    ID3D12RootSignature* rootSignature
);