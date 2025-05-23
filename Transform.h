#pragma once

#include <DirectXMath.h>

class Transform
{
private:
	DirectX::XMFLOAT3 location;
	DirectX::XMFLOAT3 rotation;
	DirectX::XMFLOAT3 scale;

	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 worldInverseMatrix;
	DirectX::XMFLOAT4X4 worldInverseTransposeMatrix;

	// Flag for if the stored world matrix needs to be updated to reflect transformations
	bool isWorldMatrixDirty;

public:
	Transform();
	Transform(DirectX::XMFLOAT3 location, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale);

	void MoveAbsolute(float x, float y, float z);
	void MoveAbsolute(DirectX::XMFLOAT3 offset);
	void MoveRelative(float x, float y, float z);
	void MoveRelative(DirectX::XMFLOAT3 offset);
	void Rotate(float pitch, float yaw, float roll);
	void Rotate(DirectX::XMFLOAT3 pitchYawRoll);
	void Scale(float x, float y, float z);
	void Scale(DirectX::XMFLOAT3 scale);

	DirectX::XMFLOAT3 GetLocation();
	DirectX::XMFLOAT3 GetPitchYawRoll();
	DirectX::XMFLOAT3 GetScale();
	DirectX::XMFLOAT3 GetRight();
	DirectX::XMFLOAT3 GetUp();
	DirectX::XMFLOAT3 GetForward();
	DirectX::XMFLOAT4X4 GetWorldMatrix();
	DirectX::XMFLOAT4X4 GetWorldInverseMatrix();
	DirectX::XMFLOAT4X4 GetWorldInverseTransposeMatrix();

	void SetLocation(float x, float y, float z);
	void SetLocation(DirectX::XMFLOAT3 location);
	void SetRotation(float pitch, float yaw, float roll);
	void SetRotation(DirectX::XMFLOAT3 rotation);
	void SetScale(float x, float y, float z);
	void SetScale(DirectX::XMFLOAT3 scale);
};