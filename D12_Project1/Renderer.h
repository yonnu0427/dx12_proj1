#pragma once

#include "RenderObject.h"

#include <d3d12.h>

// ƒ‚ƒfƒ‹‚Ì•`‰æ
void DrawModel(
    RenderObject& object,
    ID3D12Resource* cameraConstBuff,
    ID3D12RootSignature* rootSignature
);