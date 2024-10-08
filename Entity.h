#pragma once

#include <memory>

#include "Transform.h"
#include "Mesh.h"
#include "Camera.h"
#include "Material.h"

class Entity
{
private:
	Transform transform;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;

	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;

public:
	Entity(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);

	void Draw(std::shared_ptr<Camera> camera);

	Transform* GetTransform();
	std::shared_ptr<Mesh> GetMesh();
	std::shared_ptr<Material> GetMaterial() { return material; };

	void SetMaterial(std::shared_ptr<Material> value) { material = value; };
};