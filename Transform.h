#pragma once

#include <DirectXMath.h>

class Transform
{
public:
	Transform();

	// Setters

	// Getters
	DirectX::XMFLOAT4X4 GetWorldMatrix();

private:
	DirectX::XMFLOAT3 location;
	DirectX::XMFLOAT3 rotation;
	DirectX::XMFLOAT3 scale;

	DirectX::XMFLOAT4X4 worldMatrix;

	// Flag for if the stored world matrix needs to be updated to reflect transformations
	bool isWorldMatrixDirty;
};