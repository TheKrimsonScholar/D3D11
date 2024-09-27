#include "Entity.h"
#include "BufferStructs.h"
#include "Graphics.h"

using namespace DirectX;

Entity::Entity(std::shared_ptr<Mesh> mesh) : 
	colorTint(1, 1, 1, 1)
{
	this->mesh = mesh;

	/* Create Constant Buffer */

	D3D11_BUFFER_DESC cbData = {};
	cbData.ByteWidth = (sizeof(VSData) + 15) / 16 * 16; // Dirty way of aligning to nearest 16-byte increment
	cbData.Usage = D3D11_USAGE_DYNAMIC; // Can be changed at any time
	cbData.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbData.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	Graphics::Device->CreateBuffer(&cbData, 0, constantBuffer.GetAddressOf());

	Graphics::Context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
}

void Entity::Draw(Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer)
{
	// Create data to be sent to the vertex shader
	VSData vsData;
	vsData.offset = transform.GetWorldMatrix();
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