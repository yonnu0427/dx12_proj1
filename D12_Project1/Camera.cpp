#include "Camera.h"

DirectX::XMVECTOR Camera::GetForward() const
{
	DirectX::XMVECTOR rotation = DirectX::XMLoadFloat4(&transform.rotation);

	return DirectX::XMVector3Rotate(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), rotation);
}

DirectX::XMVECTOR Camera::GetRight() const
{
	DirectX::XMVECTOR rotation = DirectX::XMLoadFloat4(&transform.rotation);

	return DirectX::XMVector3Rotate(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), rotation);
}

DirectX::XMVECTOR Camera::GetUp() const
{
	DirectX::XMVECTOR rotation = DirectX::XMLoadFloat4(&transform.rotation);

	return DirectX::XMVector3Rotate(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), rotation);
}

DirectX::XMMATRIX Camera::GetViewMatrix() const
{
	DirectX::XMVECTOR position = XMLoadFloat3(&transform.position);

	return DirectX::XMMatrixLookToLH(
		position,
		GetForward(),
		GetUp()
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