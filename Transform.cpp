#include "Transform.h"

using namespace DirectX;

Transform::Transform() : 
	location(0, 0, 0),
	rotation(0, 0, 0),
	scale(1, 1, 1),
	isWorldMatrixDirty(false)
{
	XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());
}

XMFLOAT4X4 Transform::GetWorldMatrix()
{
	if(isWorldMatrixDirty)
	{
		XMMATRIX tra = XMMatrixTranslationFromVector(XMLoadFloat3(&location));
		XMMATRIX rot = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&rotation));
		XMMATRIX sca = XMMatrixScalingFromVector(XMLoadFloat3(&scale));

		XMStoreFloat4x4(&worldMatrix, sca * rot * tra);
	}

	return worldMatrix;
}