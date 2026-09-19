#pragma once

#include <DirectXMath.h>

struct Camera
{
    DirectX::XMFLOAT3 position = { 0.0f, 15.0f, -15.0f };
    DirectX::XMFLOAT3 target = { 0.0f, 15.0f, 0.0f };
    DirectX::XMFLOAT3 up = { 0.0f, 1.0f, 0.0f };

    float fov = DirectX::XM_PIDIV4;
    float nearZ = 1.0f;
    float farZ = 100.0f;

    DirectX::XMMATRIX GetViewMatrix() const;
    DirectX::XMMATRIX GetProjectionMatrix(float aspectRatio) const;
};