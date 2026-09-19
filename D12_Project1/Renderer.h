#pragma once

#include "Model.h"

#include <d3d12.h>

void DrawModel(
    Model& model,
    ID3D12Resource* sceneConstBuff,
    ID3D12RootSignature* rootSignature
);