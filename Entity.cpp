#include "Entity.h"
#include "BufferStructs.h"
#include "Graphics.h"

using namespace DirectX;

Entity::Entity(std::shared_ptr<Mesh> mesh) : 
	colorTint(1, 1, 1, 1)
{
	this->mesh = mesh;
}

void Entity::Draw(Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer)
{
	// Create data to be sent to the vertex shader
	VSData vsData;
	vsData.worldMatrix = transform.GetWorldMatrix();
	vsData.colorTint = colorTint;

	// Write to the constant buffer so it can be used by the vertex shader
	D3D11_MAPPED_SUBRESOURCE mapped = {};
	Graphics::Context->Map(constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	memcpy(mapped.pData, &vsData, sizeof(VSData));
	Graphics::Context->Unmap(constantBuffer.Get(), 0);

	mesh->Draw();
}

Transform* Entity::GetTransform() { return &transform; }
std::shared_ptr<Mesh> Entity::GetMesh() { return mesh; }