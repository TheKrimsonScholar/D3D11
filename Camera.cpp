#include "Camera.h"

using namespace DirectX;

Camera::Camera(DirectX::XMFLOAT3 location, DirectX::XMFLOAT3 rotation, float aspectRatio, float fov) : 
	transform(location, rotation, XMFLOAT3(1, 1, 1)), aspectRatio(aspectRatio), fov(fov)
{
	UpdateViewMatrix();
	UpdateProjectionMatrix(aspectRatio);
}

void Camera::Update(float deltaTime)
{
	// Minecraft controls
	if(Input::KeyDown('W'))
		transform.MoveRelative(0, 0, deltaTime * movementSpeed);
	if(Input::KeyDown('S'))
		transform.MoveRelative(0, 0, -deltaTime * movementSpeed);
	if(Input::KeyDown('D'))
		transform.MoveRelative(deltaTime * movementSpeed, 0, 0);
	if(Input::KeyDown('A'))
		transform.MoveRelative(-deltaTime * movementSpeed, 0, 0);
	if(Input::KeyDown(VK_SPACE))
		transform.MoveAbsolute(0, deltaTime * movementSpeed, 0);
	if(Input::KeyDown(VK_LSHIFT))
		transform.MoveAbsolute(0, -deltaTime * movementSpeed, 0);

	// Camera looking (while LMB is held)
	if(Input::MouseLeftDown())
	{
		float deltaX = Input::GetMouseXDelta() * lookSpeed;
		float deltaY = Input::GetMouseYDelta() * lookSpeed;

		transform.Rotate(deltaY, deltaX, 0);

		// Clamp camera pitch between -1/2 PI and +1/2 PI
		if(transform.GetPitchYawRoll().x < -XM_PIDIV2)
			transform.SetRotation(-XM_PIDIV2, transform.GetPitchYawRoll().y, transform.GetPitchYawRoll().z);
		if(transform.GetPitchYawRoll().x > XM_PIDIV2)
			transform.SetRotation(XM_PIDIV2, transform.GetPitchYawRoll().y, transform.GetPitchYawRoll().z);
	}

	// Make sure the view matrix reflects any changes
	UpdateViewMatrix();
}

XMFLOAT4X4 Camera::UpdateViewMatrix()
{
	XMFLOAT3 location = transform.GetLocation();
	XMFLOAT3 forward = transform.GetForward();
	XMFLOAT3 up = transform.GetUp();

	XMStoreFloat4x4(&viewMatrix, XMMatrixLookToLH(
		XMLoadFloat3(&location), 
		XMLoadFloat3(&forward), 
		XMLoadFloat3(&up)));
	return viewMatrix;
}
XMFLOAT4X4 Camera::UpdateProjectionMatrix(float aspectRatio)
{
	XMStoreFloat4x4(&projMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(fov), aspectRatio, nearDistance, farDistance));
	return projMatrix;
}

XMFLOAT4X4 Camera::GetViewMatrix()
{
	return viewMatrix;
}
XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return projMatrix;
}