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
	Mesh(const wchar_t* filePath);
	~Mesh();

	void CreateBuffers(Vertex* vertices, UINT* indices, int vertexCount, int indexCount);

	void Draw();

	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer() { return vertexBuffer; };
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer() { return indexBuffer; };
	UINT GetVertexCount() { return vertexCount; };
	UINT GetIndexCount() { return indexCount; };
	std::string GetName() { return name; };
};