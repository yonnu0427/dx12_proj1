#pragma once

#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>

extern IDXGIFactory4* _dxgiFactory;
extern ID3D12Device* _dev;
extern ID3D12CommandAllocator* _cmdAllocator;
extern ID3D12GraphicsCommandList* _cmdList;
extern ID3D12CommandQueue* _cmdQueue;
extern IDXGISwapChain4* _swapchain;

void EnableDebugLayer();

HRESULT CreateSwapChain(
    const HWND& hwnd,
    IDXGIFactory4*& dxgiFactory
);

HRESULT InitializeDXGIDevice();

HRESULT InitializeCommand();

HRESULT CreateFinalRenderTarget(
    ID3D12DescriptorHeap*& rtvHeaps,
    std::vector<ID3D12Resource*>& backBuffers
);