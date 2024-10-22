#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <memory>
#include <vector>

#include <DirectXMath.h>

#include "SimpleShader.h"

#include "Vertex.h"
#include "Mesh.h"
#include "Entity.h"
#include "Camera.h"
#include "Material.h"
#include "Lights.h"

class Game
{
private:
	// Shaders and shader-related constructs
	std::shared_ptr<SimpleVertexShader> vertexShader;
	std::shared_ptr<SimplePixelShader> pixelShader;
	std::shared_ptr<SimplePixelShader> normalPixelShader;
	std::shared_ptr<SimplePixelShader> uvPixelShader;
	std::shared_ptr<SimplePixelShader> customPixelShader;

	float backgroundColor[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	bool isDemoWindowHidden;

	std::vector<std::shared_ptr<Camera>> cameras;
	unsigned int activeCameraIndex;

	std::vector<std::shared_ptr<Material>> materials;
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::vector<std::shared_ptr<Entity>> entities;

	DirectX::XMFLOAT3 ambientLightColor { 0, 0, 0 };
	Light directionalLight = {};
	std::vector<Light> lights;

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

	// Returns the currently in-use camera using the active camera index (returns nullptr if index is out of bounds).
	std::shared_ptr<Camera> GetCamera() { return activeCameraIndex < cameras.size() ? cameras[activeCameraIndex] : nullptr; };

private:
	// Initialization helper methods - feel free to customize, combine, remove, etc.
	void LoadShaders();
	void CreateMaterials();
	void CreateGeometry();

	// ImGUI implementation
	void UpdateImGui(float deltaTime, float totalTime);
	void BuildUI();
};