#include "Transform.h"

// ローカル座標からワールド座標に変換する
DirectX::XMMATRIX Transform::GetWorldMatrix() const
{
    DirectX::XMMATRIX scaleMat = DirectX::XMMatrixScaling(
        scale.x,
        scale.y,
        scale.z
    );

    DirectX::XMVECTOR q = DirectX::XMVectorSet(
        rotation.x,
        rotation.y,
        rotation.z,
        rotation.w
    );


    auto rotationMat = DirectX::XMMatrixRotationQuaternion(q);

    auto translationMat = DirectX::XMMatrixTranslation(
        position.x,
        position.y,
        position.z
    );

    return scaleMat * rotationMat * translationMat;
}

// オイラー角の入力をクォータニオンにしてrotationにセットする
void Transform::SetRotationEuler(float pitch, float yaw, float roll) {
    DirectX::XMVECTOR q = DirectX::XMQuaternionRotationRollPitchYaw(
        pitch,
        yaw,
        roll
    );

    DirectX::XMStoreFloat4(&rotation, q);
}