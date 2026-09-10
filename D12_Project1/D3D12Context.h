#pragma once

#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>

class D3D12Context
{
public:
    bool Initialize(
        HWND hwnd,
        unsigned int width,
        unsigned int height
    );

    ID3D12Device* GetDevice() const
    {
        return device_;
    }

    ID3D12GraphicsCommandList* GetCommandList() const
    {
        return commandList_;
    }

    ID3D12CommandQueue* GetCommandQueue() const
    {
        return commandQueue_;
    }

    IDXGISwapChain4* GetSwapChain() const
    {
        return swapChain_;
    }

private:
    bool InitializeDevice();
    bool InitializeCommand();
    bool CreateSwapChain(HWND hwnd);
    bool CreateRenderTargets();

#ifdef _DEBUG
    void EnableDebugLayer();
#endif

private:
    unsigned int width_ = 0;
    unsigned int height_ = 0;

    IDXGIFactory4* dxgiFactory_ = nullptr;
    ID3D12Device* device_ = nullptr;

    ID3D12CommandAllocator* commandAllocator_ = nullptr;
    ID3D12GraphicsCommandList* commandList_ = nullptr;
    ID3D12CommandQueue* commandQueue_ = nullptr;

    IDXGISwapChain4* swapChain_ = nullptr;

    ID3D12DescriptorHeap* rtvHeap_ = nullptr;
    std::vector<ID3D12Resource*> backBuffers_;
};