#pragma once

#include "Transform.h"
#include "Input.h"

class Camera
{
private:
	Transform transform;

	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projMatrix;

	float fov = 60.0f;
	float nearDistance = 0.1f;
	float farDistance = 100.0f;

	float movementSpeed = 1.0f;
	float lookSpeed = 1.0f;

public:
	Camera(DirectX::XMFLOAT3 location, DirectX::XMFLOAT3 rotation, float aspectRatio);

	DirectX::XMFLOAT4X4 UpdateProjectionMatrix(float aspectRatio);
	DirectX::XMFLOAT4X4 UpdateViewMatrix();
};