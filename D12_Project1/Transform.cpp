#include "Transform.h"

DirectX::XMMATRIX Transform::GetWorldMatrix() const
{
    auto scaleMat = DirectX::XMMatrixScaling(
        scale.x,
        scale.y,
        scale.z
    );

    auto rotationMat = DirectX::XMMatrixRotationRollPitchYaw(
        rotation.x,
        rotation.y,
        rotation.z
    );

    auto translationMat = DirectX::XMMatrixTranslation(
        position.x,
        position.y,
        position.z
    );

    return scaleMat * rotationMat * translationMat;
}