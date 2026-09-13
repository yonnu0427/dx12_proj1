#pragma once

#include "PMD.h"

#include <d3d12.h>
#include <vector>

struct Model
{
    PMDData data;

    ID3D12Resource* vertBuff = nullptr;
    D3D12_VERTEX_BUFFER_VIEW vbView = {};

    ID3D12Resource* idxBuff = nullptr;
    D3D12_INDEX_BUFFER_VIEW ibView = {};

    ID3D12Resource* materialBuff = nullptr;
    ID3D12DescriptorHeap* materialDescHeap = nullptr;

    std::vector<ID3D12Resource*> textureResources;
    std::vector<ID3D12Resource*> sphResources;
    std::vector<ID3D12Resource*> spaResources;
    std::vector<ID3D12Resource*> toonResources;
};