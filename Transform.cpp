#include "Transform.h"

using namespace DirectX;

Transform::Transform() : 
	location(0, 0, 0),
	rotation(0, 0, 0),
	scale(1, 1, 1),
	isWorldMatrixDirty(false)
{
	XMStoreFloat4x4(&offset, XMMatrixIdentity());
	XMStoreFloat4x4(&worldInverseTransposeMatrix, XMMatrixIdentity());
}

void Transform::MoveAbsolute(float x, float y, float z)
{
	SetLocation(location.x + x, location.y + y, location.z + z);
}
void Transform::MoveAbsolute(DirectX::XMFLOAT3 offset)
{
	MoveAbsolute(offset.x, offset.y, offset.z);
}
void Transform::Rotate(float pitch, float yaw, float roll)
{
	SetRotation(rotation.x + pitch, rotation.y + yaw, rotation.z + roll);
}
void Transform::Rotate(DirectX::XMFLOAT3 pitchYawRoll)
{
	Rotate(pitchYawRoll.x, pitchYawRoll.y, pitchYawRoll.z);
}
void Transform::Scale(float x, float y, float z)
{
	SetScale(scale.x * x, scale.y * y, scale.z * z);
}
void Transform::Scale(DirectX::XMFLOAT3 scale)
{
	Scale(scale.x, scale.y, scale.z);
}

#pragma region Getters
XMFLOAT3 Transform::GetLocation() { return location; }
XMFLOAT3 Transform::GetPitchYawRoll() { return rotation; }
XMFLOAT3 Transform::GetScale() { return scale; }
XMFLOAT4X4 Transform::GetWorldMatrix()
{
	if(isWorldMatrixDirty)
	{
		XMVECTOR translationVector = XMLoadFloat3(&location);
		XMVECTOR rotationVector = XMLoadFloat3(&rotation);
		XMVECTOR scaleVector = XMLoadFloat3(&scale);

		XMMATRIX translation = XMMatrixTranslationFromVector(translationVector);
		XMMATRIX rotation = XMMatrixRotationRollPitchYawFromVector(rotationVector);
		XMMATRIX scale = XMMatrixScalingFromVector(scaleVector);

		XMMATRIX world = scale * rotation * translation;

		XMStoreFloat4x4(&offset, world);
		XMStoreFloat4x4(&worldInverseTransposeMatrix, XMMatrixInverse(0, XMMatrixTranspose(world)));

		isWorldMatrixDirty = false;
	}

	return offset;
}
XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix()
{
	// Make sure the matrices are up-to-date
	if(isWorldMatrixDirty)
		GetWorldMatrix();

	return worldInverseTransposeMatrix;
}
#pragma endregion

#pragma region Setters
void Transform::SetLocation(float x, float y, float z) { SetLocation(XMFLOAT3(x, y, z)); }
void Transform::SetLocation(XMFLOAT3 location)
{
	this->location = location;

	isWorldMatrixDirty = true;
}
void Transform::SetRotation(float pitch, float yaw, float roll) { SetRotation(XMFLOAT3(pitch, yaw, roll)); }
void Transform::SetRotation(XMFLOAT3 rotation)
{
	this->rotation = rotation;

	isWorldMatrixDirty = true;
}
void Transform::SetScale(float x, float y, float z) { SetScale(XMFLOAT3(x, y, z)); }
void Transform::SetScale(XMFLOAT3 scale)
{
	this->scale = scale;

	isWorldMatrixDirty = true;
}
#pragma endregion