#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include "Vertex.h"

class Mesh
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	Vertex* vertices;
	size_t vertexCount;
	unsigned int* indices;
	size_t indexCount;

public:
	Mesh(size_t vertexCount, Vertex vertices[], size_t indexCount, unsigned int indices[]);
	~Mesh();

	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer() { return vertexBuffer; };
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer() { return indexBuffer; };
	int GetVertexCount() { return vertexCount; };
	int GetIndexCount() { return indexCount; };
	void Draw();
};