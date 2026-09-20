#pragma once

#include <DirectXMath.h>

#include "Transform.h"

struct Camera
{
    Transform transform;

    float fov = DirectX::XM_PIDIV4;
    float nearZ = 1.0f;
    float farZ = 100.0f;

    DirectX::XMVECTOR GetForward() const;
    DirectX::XMVECTOR GetRight() const;
    DirectX::XMVECTOR GetUp() const;

    DirectX::XMMATRIX GetViewMatrix() const;
    DirectX::XMMATRIX GetProjectionMatrix(float aspectRatio) const;
};