#include "Mesh.h"
#include "Graphics.h"

#include <iostream>

Mesh::Mesh(size_t vertexCount, Vertex vertices[], size_t indexCount, unsigned int indices[])
{
	this->vertexCount = vertexCount;
	this->indexCount = indexCount;

	this->vertices = vertices;
	this->indices = indices;

	D3D11_BUFFER_DESC vbInfo;
	vbInfo.Usage = D3D11_USAGE_IMMUTABLE;
	vbInfo.ByteWidth = sizeof(Vertex) * vertexCount; //sizeof(vertices);
	vbInfo.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbInfo.CPUAccessFlags = 0;
	vbInfo.MiscFlags = 0;
	vbInfo.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initialVertexData;
	initialVertexData.pSysMem = vertices;

	Graphics::Device->CreateBuffer(&vbInfo, &initialVertexData, vertexBuffer.GetAddressOf());

	D3D11_BUFFER_DESC ibInfo;
	ibInfo.Usage = D3D11_USAGE_IMMUTABLE;
	ibInfo.ByteWidth = sizeof(unsigned int) * indexCount; //sizeof(indices)
	ibInfo.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibInfo.CPUAccessFlags = 0;
	ibInfo.MiscFlags = 0;
	ibInfo.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initialIndexData;
	initialIndexData.pSysMem = indices;

	Graphics::Device->CreateBuffer(&ibInfo, &initialIndexData, indexBuffer.GetAddressOf());
}
Mesh::~Mesh()
{
	delete[] vertices;
	delete[] indices;
}

void Mesh::Draw()
{
	UINT stride = sizeof(Vertex); // Space between starting indices for each vertex
	UINT offset = 0;
	Graphics::Context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	Graphics::Context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	Graphics::Context->DrawIndexed(indexCount, 0, 0);
}