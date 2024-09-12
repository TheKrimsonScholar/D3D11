#include "Mesh.h"
#include "Graphics.h"

Mesh::Mesh(Vertex vertices[], int indices[])
{
	vertexCount = sizeof(vertices) / sizeof(Vertex);
	vertexCount = 3;
	indexCount = sizeof(indices) / sizeof(int);
	indexCount = 3;

	D3D11_BUFFER_DESC vbInfo;
	vbInfo.Usage = D3D11_USAGE_DYNAMIC;
	vbInfo.ByteWidth = sizeof(Vertex) * 3; //sizeof(vertices);
	vbInfo.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbInfo.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbInfo.MiscFlags = 0;
	vbInfo.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initialVertexData;
	initialVertexData.pSysMem = vertices;

	Graphics::Device->CreateBuffer(&vbInfo, &initialVertexData, vertexBuffer.GetAddressOf());

	D3D11_BUFFER_DESC ibInfo;
	ibInfo.Usage = D3D11_USAGE_IMMUTABLE;
	ibInfo.ByteWidth = sizeof(unsigned int) * 3; //sizeof(indices)
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

}

void Mesh::Draw()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	Graphics::Context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	Graphics::Context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	Graphics::Context->DrawIndexed(indexCount, 0, 0);
}