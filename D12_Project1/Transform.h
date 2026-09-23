#pragma once

#include <DirectXMath.h>

// オブジェクト, カメラなどの位置, 回転, 大きさを管理する
struct Transform
{
    DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT4 rotation = { 0.0f, 0.0f, 0.0f , 1.0f };
    DirectX::XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f };

    DirectX::XMMATRIX GetWorldMatrix() const;

    void SetRotationEuler(float pitch, float yaw, float roll);
};