#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include "Vertex.h"

#include <DirectXMath.h>

class Game
{
private:
	float backgroundColor[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	bool isDemoWindowHidden;

	Vertex vertices[3] = {
		{ DirectX::XMFLOAT3(+0.0f, +0.5f, +0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) }, // Red
		{ DirectX::XMFLOAT3(+0.5f, -0.5f, +0.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) }, // Blue
		{ DirectX::XMFLOAT3(-0.5f, -0.5f, +0.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) }, // Green
	};

public:
	// Basic OOP setup
	Game() = default;
	~Game();
	Game(const Game&) = delete; // Remove copy constructor
	Game& operator=(const Game&) = delete; // Remove copy-assignment operator

	// Primary functions
	void Initialize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);
	void OnResize();

private:
	// Initialization helper methods - feel free to customize, combine, remove, etc.
	void LoadShaders();
	void CreateGeometry();

	// ImGUI implementation
	void UpdateImGui(float deltaTime, float totalTime);
	void BuildUI();

	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//     Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	// Buffers to hold actual geometry data
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	// Shaders and shader-related constructs
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
};

