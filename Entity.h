#pragma once

#include <memory>

#include "Transform.h"
#include "Mesh.h"

class Entity
{
private:
	Transform transform;
	std::shared_ptr<Mesh> mesh;

	DirectX::XMFLOAT4 colorTint;

	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;

public:
	Entity(std::shared_ptr<Mesh> mesh);

	void Draw(Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer);

	Transform* GetTransform();
	std::shared_ptr<Mesh> GetMesh();
};