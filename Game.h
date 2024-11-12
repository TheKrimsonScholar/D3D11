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
#include "Skybox.h"

class Game
{
private:
	#pragma region FilePaths
	// File paths of all textures that will be used
	const std::vector<std::wstring> texturePaths =
	{
		L"../../Assets/Textures/brokentiles.png",
		L"../../Assets/Textures/rustymetal.png",
		L"../../Assets/Textures/tiles.png",
		L"../../Assets/Textures/cobblestone.png",
		L"../../Assets/Textures/cushion.png",
		L"../../Assets/Textures/rock.png",

		L"../../Assets/Textures/brokentiles_specular.png",
		L"../../Assets/Textures/rustymetal_specular.png",
		L"../../Assets/Textures/tiles_specular.png",

		L"../../Assets/Textures/flat_normals.png",
		L"../../Assets/Textures/cobblestone_normals.png",
		L"../../Assets/Textures/cushion_normals.png",
		L"../../Assets/Textures/rock_normals.png",
	};
	const std::unordered_map<std::wstring, std::vector<std::wstring>> cubemapPaths =
	{
		{ L"Cold Sunset",
			{
				L"../../Assets/Textures/Skies/Cold Sunset/right.png",
				L"../../Assets/Textures/Skies/Cold Sunset/left.png",
				L"../../Assets/Textures/Skies/Cold Sunset/up.png",
				L"../../Assets/Textures/Skies/Cold Sunset/down.png",
				L"../../Assets/Textures/Skies/Cold Sunset/front.png",
				L"../../Assets/Textures/Skies/Cold Sunset/back.png"
			}
		}
	};
	#pragma endregion

	// Shaders and shader-related constructs
	std::shared_ptr<SimpleVertexShader> vertexShader;
	std::shared_ptr<SimplePixelShader> pixelShader;
	std::shared_ptr<SimplePixelShader> normalPixelShader;
	std::shared_ptr<SimplePixelShader> uvPixelShader;
	std::shared_ptr<SimplePixelShader> customPixelShader;

	std::unordered_map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textureSRVs;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;

	float backgroundColor[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	bool isDemoWindowHidden;

	std::vector<std::shared_ptr<Camera>> cameras;
	unsigned int activeCameraIndex;

	std::shared_ptr<Skybox> skybox;

	std::vector<std::shared_ptr<Material>> materials;
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::vector<std::shared_ptr<Entity>> entities;

	DirectX::XMFLOAT3 ambientLightColor { 0.1f, 0.1f, 0.1f };
	Light directionalLight = {};
	std::vector<Light> lights;

	float totalTime;

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
	// Initialization helper methods
	void LoadTextures();
	void LoadShaders();
	void CreateMaterials();
	void CreateGeometry();
	void CreateSky();
	void CreateLights();

	// Helper for creating a cubemap from 6 individual textures
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateCubemap(
		const wchar_t* right, const wchar_t* left, const wchar_t* up, const wchar_t* down, const wchar_t* front, const wchar_t* back);

	// ImGUI implementation
	void UpdateImGui(float deltaTime, float totalTime);
	void BuildUI();
};