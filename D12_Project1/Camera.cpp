#include "Camera.h"

DirectX::XMMATRIX Camera::GetViewMatrix() const
{
	return DirectX::XMMatrixLookAtLH(
		DirectX::XMLoadFloat3(&position),
		DirectX::XMLoadFloat3(&target),
		DirectX::XMLoadFloat3(&up)
	);
}
DirectX::XMMATRIX Camera::GetProjectionMatrix(float aspectRatio) const
{
	return DirectX::XMMatrixPerspectiveFovLH(
		fov,
		aspectRatio,
		nearZ,
		farZ
	);
}