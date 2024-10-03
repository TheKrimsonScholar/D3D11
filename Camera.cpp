#include "Camera.h"

using namespace DirectX;

Camera::Camera(DirectX::XMFLOAT3 location, DirectX::XMFLOAT3 rotation, float aspectRatio) : 
	transform(location, rotation, XMFLOAT3(1, 1, 1))
{
	UpdateViewMatrix();
	UpdateProjectionMatrix(aspectRatio);
}

DirectX::XMFLOAT4X4 Camera::UpdateProjectionMatrix(float aspectRatio)
{
	return DirectX::XMFLOAT4X4();
}
DirectX::XMFLOAT4X4 Camera::UpdateViewMatrix()
{
	return DirectX::XMFLOAT4X4();
}