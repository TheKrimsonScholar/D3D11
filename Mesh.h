#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <string>
#include "Vertex.h"

class Mesh
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	Vertex* vertices;
	UINT vertexCount;
	unsigned int* indices;
	UINT indexCount;
	std::string name;

public:
	Mesh(std::string name, UINT vertexCount, Vertex vertices[], UINT indexCount, unsigned int indices[]);
	Mesh(const char* filePath);
	~Mesh();

	void CreateBuffers();

	void Draw();

	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer() { return vertexBuffer; };
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer() { return indexBuffer; };
	size_t GetVertexCount() { return vertexCount; };
	size_t GetIndexCount() { return indexCount; };
	std::string GetName() { return name; };
};