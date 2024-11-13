#include "Game.h"
#include "Graphics.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Window.h"
#include <memory>
#include <iostream>

// From DirectX Tool Kit
#include "WICTextureLoader.h"

#include <DirectXMath.h>

// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Called once per program, after the window and graphics API
// are initialized but before the game loop begins
// --------------------------------------------------------
void Game::Initialize()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadTextures();
	LoadShaders();
	CreateMaterials();
	CreateGeometry();
	CreateLights();

	// Set initial graphics API state
	//  - These settings persist until we change them
	//  - Some of these, like the primitive topology & input layout, probably won't change
	//  - Others, like setting shaders, will need to be moved elsewhere later
	{
		// Tell the input assembler (IA) stage of the pipeline what kind of
		// geometric primitives (points, lines or triangles) we want to draw.  
		// Essentially: "What kind of shape should the GPU draw with our vertices?"
		Graphics::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	/* Create Camera */

	// First camera - this is the default active camera
	cameras.push_back(std::make_shared<Camera>(XMFLOAT3(0, 0, -1), XMFLOAT3(0, 0, 0), (float) Window::Width() / Window::Height(), 60.0f));

	// Low FOV camera from the left, looking 45 degrees to the right
	cameras.push_back(std::make_shared<Camera>(XMFLOAT3(-1, 0, -1), XMFLOAT3(0, XM_PIDIV4, 0), (float) Window::Width() / Window::Height(), 45.0f));
	// High FOV camera from the right, looking 45 degrees to the left
	cameras.push_back(std::make_shared<Camera>(XMFLOAT3(1, 0, -1), XMFLOAT3(0, -XM_PIDIV4, 0), (float) Window::Width() / Window::Height(), 90.0f));

	// Initialize ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(Window::Handle());
	ImGui_ImplDX11_Init(Graphics::Device.Get(), Graphics::Context.Get());

	ImGui::StyleColorsDark();
}


// --------------------------------------------------------
// Clean up memory or objects created by this class
// 
// Note: Using smart pointers means there probably won't
//       be much to manually clean up here!
// --------------------------------------------------------
Game::~Game()
{
	// Clean up for ImGui
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Game::LoadTextures()
{
	// Load all textures from their paths and associate them with identifiers in an unordered map
	for(std::wstring texturePath : texturePaths)
	{
		/* The overload that takes the device context also makes mipmaps */
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV;
		CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(texturePath).c_str(), 0, textureSRV.GetAddressOf());

		// Texture name is the final argument of the path (after the last /), i.e. "brokentiles.png"
		std::wstring textureName = texturePath.substr(texturePath.find_last_of('/') + 1);
		// Add the SRV to the map so it can be accessed by its texture name
		textureSRVs.insert({ textureName.c_str(), textureSRV });
	}

	// Load cubemaps from paths and add them to textureSRVs
	for(auto& cubemap : cubemapPaths)
		textureSRVs.insert({ cubemap.first, CreateCubemap(
			FixPath(cubemap.second[0]).c_str(),
			FixPath(cubemap.second[1]).c_str(),
			FixPath(cubemap.second[2]).c_str(),
			FixPath(cubemap.second[3]).c_str(),
			FixPath(cubemap.second[4]).c_str(),
			FixPath(cubemap.second[5]).c_str()) });

	// Create the basic texture sampler
	D3D11_SAMPLER_DESC samplerDesc = {};
	// Clamp coordinate values so texture stretches from extremes
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.MaxAnisotropy = 16; // Must be power of 2, 0-16 - higher is better

	Graphics::Device->CreateSamplerState(&samplerDesc, sampler.GetAddressOf());
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	vertexShader = std::make_shared<SimpleVertexShader>(Graphics::Device, Graphics::Context, FixPath(L"VertexShader.cso").c_str());
	pixelShader = std::make_shared<SimplePixelShader>(Graphics::Device, Graphics::Context, FixPath(L"PixelShader.cso").c_str());
	normalPixelShader = std::make_shared<SimplePixelShader>(Graphics::Device, Graphics::Context, FixPath(L"PSNormal.cso").c_str());
	uvPixelShader = std::make_shared<SimplePixelShader>(Graphics::Device, Graphics::Context, FixPath(L"PSUV.cso").c_str());
	customPixelShader = std::make_shared<SimplePixelShader>(Graphics::Device, Graphics::Context, FixPath(L"PSCustom.cso").c_str());

	skyboxVertexShader = std::make_shared<SimpleVertexShader>(Graphics::Device, Graphics::Context, FixPath(L"VSSky.cso").c_str());
	skyboxPixelShader = std::make_shared<SimplePixelShader>(Graphics::Device, Graphics::Context, FixPath(L"PSSky.cso").c_str());
}

void Game::CreateMaterials()
{
	materials.push_back(std::make_shared<Material>(vertexShader, pixelShader, XMFLOAT4(1, 1, 1, 1), 0.25f, XMFLOAT2(2, 2), XMFLOAT2(1, 1))); // White material (broken tiles)
	materials.push_back(std::make_shared<Material>(vertexShader, pixelShader, XMFLOAT4(1, 1, 1, 1), 0.25f, XMFLOAT2(0.5f, 0.5f))); // White material (metal)
	materials.push_back(std::make_shared<Material>(vertexShader, pixelShader, XMFLOAT4(1, 1, 1, 1), 0.25f)); // White material (tiles)
	materials.push_back(std::make_shared<Material>(vertexShader, pixelShader, XMFLOAT4(1, 1, 1, 1), 0.25f)); // White material (cobblestone)
	materials.push_back(std::make_shared<Material>(vertexShader, pixelShader, XMFLOAT4(1, 1, 1, 1), 0.25f)); // White material (cushion)
	materials.push_back(std::make_shared<Material>(vertexShader, pixelShader, XMFLOAT4(1, 1, 1, 1), 0.25f)); // White material (rock)
	materials.push_back(std::make_shared<Material>(vertexShader, pixelShader, XMFLOAT4(1, 0, 0, 1), 0.5f)); // Red material
	materials.push_back(std::make_shared<Material>(vertexShader, pixelShader, XMFLOAT4(1, 0, 1, 1), 1.0f)); // Purple material
	materials.push_back(std::make_shared<Material>(vertexShader, normalPixelShader, XMFLOAT4(1, 1, 1, 1), 0.0f)); // Normal material
	materials.push_back(std::make_shared<Material>(vertexShader, uvPixelShader, XMFLOAT4(1, 1, 1, 1), 0.25f)); // UV material
	materials.push_back(std::make_shared<Material>(vertexShader, customPixelShader, XMFLOAT4(1, 1, 1, 1), 0.75f)); // Custom material

	/* Materials with specular maps */

	materials[0]->AddTextureSRV("SurfaceColorTexture", textureSRVs[L"brokentiles.png"]);
	materials[0]->AddTextureSRV("SpecularMap", textureSRVs[L"brokentiles_specular.png"]);
	materials[0]->AddSampler("BasicSampler", sampler);

	materials[1]->AddTextureSRV("SurfaceColorTexture", textureSRVs[L"rustymetal.png"]);
	materials[1]->AddTextureSRV("SpecularMap", textureSRVs[L"rustymetal_specular.png"]);
	materials[1]->AddSampler("BasicSampler", sampler);

	materials[2]->AddTextureSRV("SurfaceColorTexture", textureSRVs[L"tiles.png"]);
	materials[2]->AddTextureSRV("SpecularMap", textureSRVs[L"tiles_specular.png"]);
	materials[2]->AddSampler("BasicSampler", sampler);

	/* Materials with normal maps */

	materials[3]->AddTextureSRV("SurfaceColorTexture", textureSRVs[L"cobblestone.png"]);
	materials[3]->AddTextureSRV("NormalMap", textureSRVs[L"cobblestone_normals.png"]);
	materials[3]->AddSampler("BasicSampler", sampler);

	materials[4]->AddTextureSRV("SurfaceColorTexture", textureSRVs[L"cushion.png"]);
	materials[4]->AddTextureSRV("NormalMap", textureSRVs[L"cushion_normals.png"]);
	materials[4]->AddSampler("BasicSampler", sampler);

	materials[5]->AddTextureSRV("SurfaceColorTexture", textureSRVs[L"rock.png"]);
	materials[5]->AddTextureSRV("NormalMap", textureSRVs[L"rock_normals.png"]);
	materials[5]->AddSampler("BasicSampler", sampler);
}

// --------------------------------------------------------
// Creates the geometry we're going to draw
// --------------------------------------------------------
void Game::CreateGeometry()
{
	// Import meshes from file
	meshes.push_back(std::make_shared<Mesh>(FixPath(L"../../Assets/Models/cube.obj").c_str()));
	meshes.push_back(std::make_shared<Mesh>(FixPath(L"../../Assets/Models/cylinder.obj").c_str()));
	meshes.push_back(std::make_shared<Mesh>(FixPath(L"../../Assets/Models/helix.obj").c_str()));
	meshes.push_back(std::make_shared<Mesh>(FixPath(L"../../Assets/Models/sphere.obj").c_str()));
	meshes.push_back(std::make_shared<Mesh>(FixPath(L"../../Assets/Models/torus.obj").c_str()));
	meshes.push_back(std::make_shared<Mesh>(FixPath(L"../../Assets/Models/quad.obj").c_str()));
	meshes.push_back(std::make_shared<Mesh>(FixPath(L"../../Assets/Models/quad_double_sided.obj").c_str()));

	/* Create skybox */
	skybox = std::make_shared<Skybox>(skyboxVertexShader, skyboxPixelShader, meshes[0], sampler, textureSRVs[L"Cold Sunset"]);

	/* Create entities */
	float spacing = 1.5f;
	for(unsigned int i = 0; i < meshes.size(); i++)
	{
		std::shared_ptr<Entity> newEntity = std::make_shared<Entity>(meshes[(i % 2 == 0) ? 0 : 3], materials[3 + i % 3]); // Use the 3 white normal map materials
		newEntity->GetTransform()->MoveAbsolute(-4.5f + i * spacing, -1.5f, 5.0f);
		newEntity->GetTransform()->Scale(0.5f, 0.5f, 0.5f);
		entities.push_back(newEntity);
	}
}

void Game::CreateLights()
{
	/* Directional Light Settings */
	directionalLight.LightType = LIGHT_TYPE_DIRECTIONAL;
	directionalLight.Direction = DirectX::XMFLOAT3(1, -1, 0);
	directionalLight.Color = DirectX::XMFLOAT3(0.25f, 0, 0.25f);
	directionalLight.Intensity = 1.0f;

	Light directionalLight2 = {};
	directionalLight2.LightType = LIGHT_TYPE_DIRECTIONAL;
	directionalLight2.Direction = DirectX::XMFLOAT3(0, 1, 0);
	directionalLight2.Color = DirectX::XMFLOAT3(1, 0.5f, 0);
	directionalLight2.Intensity = 1.0f;

	Light directionalLight3 = {};
	directionalLight3.LightType = LIGHT_TYPE_DIRECTIONAL;
	directionalLight3.Direction = DirectX::XMFLOAT3(-1, -1, 0);
	directionalLight3.Color = DirectX::XMFLOAT3(0, 1, 0);
	directionalLight3.Intensity = 0.5f;

	Light pointLight1 = {};
	pointLight1.LightType = LIGHT_TYPE_POINT;
	pointLight1.Location = DirectX::XMFLOAT3(0, -1, 5);
	pointLight1.Color = DirectX::XMFLOAT3(1, 0, 0);
	pointLight1.Intensity = 1.0f;
	pointLight1.Range = 4.0f;

	Light pointLight2 = {};
	pointLight2.LightType = LIGHT_TYPE_POINT;
	pointLight2.Location = DirectX::XMFLOAT3(2.5f, 0, 5);
	pointLight2.Color = DirectX::XMFLOAT3(0, 0, 1);
	pointLight2.Intensity = 1.0f;
	pointLight2.Range = 4.0f;

	lights.push_back(directionalLight);
	lights.push_back(directionalLight2);
	lights.push_back(directionalLight3);
	lights.push_back(pointLight1);
	lights.push_back(pointLight2);
}

// --------------------------------------------------------
// Author: Chris Cascioli
// Purpose: Creates a cube map on the GPU from 6 individual textures
// 
// - You are allowed to directly copy/paste this into your code base
//   for assignments, given that you clearly cite that this is not
//   code of your own design.
//
// - Note: This code assumes you’re putting the function in Sky.cpp, 
//   you’ve included WICTextureLoader.h and you have an ID3D11Device 
//   ComPtr called “device”.  Make any adjustments necessary for
//   your own implementation.
// --------------------------------------------------------

// --------------------------------------------------------
// Loads six individual textures (the six faces of a cube map), then
// creates a blank cube map and copies each of the six textures to
// another face.  Afterwards, creates a shader resource view for
// the cube map and cleans up all of the temporary resources.
// --------------------------------------------------------
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Game::CreateCubemap(
	const wchar_t* right,
	const wchar_t* left,
	const wchar_t* up,
	const wchar_t* down,
	const wchar_t* front,
	const wchar_t* back)
{
	// Load the 6 textures into an array.
	// - We need references to the TEXTURES, not SHADER RESOURCE VIEWS!
	// - Explicitly NOT generating mipmaps, as we don't need them for the sky!
	// - Order matters here!  +X, -X, +Y, -Y, +Z, -Z
	Microsoft::WRL::ComPtr<ID3D11Texture2D> textures[6] = {};
	CreateWICTextureFromFile(Graphics::Device.Get(), right, (ID3D11Resource**)textures[0].GetAddressOf(), 0);
	CreateWICTextureFromFile(Graphics::Device.Get(), left, (ID3D11Resource**)textures[1].GetAddressOf(), 0);
	CreateWICTextureFromFile(Graphics::Device.Get(), up, (ID3D11Resource**)textures[2].GetAddressOf(), 0);
	CreateWICTextureFromFile(Graphics::Device.Get(), down, (ID3D11Resource**)textures[3].GetAddressOf(), 0);
	CreateWICTextureFromFile(Graphics::Device.Get(), front, (ID3D11Resource**)textures[4].GetAddressOf(), 0);
	CreateWICTextureFromFile(Graphics::Device.Get(), back, (ID3D11Resource**)textures[5].GetAddressOf(), 0);

	// We'll assume all of the textures are the same color format and resolution,
	// so get the description of the first texture
	D3D11_TEXTURE2D_DESC faceDesc = {};
	textures[0]->GetDesc(&faceDesc);

	// Describe the resource for the cube map, which is simply 
	// a "texture 2d array" with the TEXTURECUBE flag set.  
	// This is a special GPU resource format, NOT just a 
	// C++ array of textures!!!
	D3D11_TEXTURE2D_DESC cubeDesc = {};
	cubeDesc.ArraySize = 6;            // Cube map!
	cubeDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; // We'll be using as a texture in a shader
	cubeDesc.CPUAccessFlags = 0;       // No read back
	cubeDesc.Format = faceDesc.Format; // Match the loaded texture's color format
	cubeDesc.Width = faceDesc.Width;   // Match the size
	cubeDesc.Height = faceDesc.Height; // Match the size
	cubeDesc.MipLevels = 1;            // Only need 1
	cubeDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE; // This should be treated as a CUBE, not 6 separate textures
	cubeDesc.Usage = D3D11_USAGE_DEFAULT; // Standard usage
	cubeDesc.SampleDesc.Count = 1;
	cubeDesc.SampleDesc.Quality = 0;

	// Create the final texture resource to hold the cube map
	Microsoft::WRL::ComPtr<ID3D11Texture2D> cubeMapTexture;
	Graphics::Device->CreateTexture2D(&cubeDesc, 0, cubeMapTexture.GetAddressOf());

	// Loop through the individual face textures and copy them,
	// one at a time, to the cube map texure
	for (int i = 0; i < 6; i++)
	{
		// Calculate the subresource position to copy into
		unsigned int subresource = D3D11CalcSubresource(
			0,  // Which mip (zero, since there's only one)
			i,  // Which array element?
			1); // How many mip levels are in the texture?

		// Copy from one resource (texture) to another
		Graphics::Context->CopySubresourceRegion(
			cubeMapTexture.Get(),  // Destination resource
			subresource,           // Dest subresource index (one of the array elements)
			0, 0, 0,               // XYZ location of copy
			textures[i].Get(),     // Source resource
			0,                     // Source subresource index (we're assuming there's only one)
			0);                    // Source subresource "box" of data to copy (zero means the whole thing)
	}

	// At this point, all of the faces have been copied into the 
	// cube map texture, so we can describe a shader resource view for it
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = cubeDesc.Format;         // Same format as texture
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE; // Treat this as a cube!
	srvDesc.TextureCube.MipLevels = 1;        // Only need access to 1 mip
	srvDesc.TextureCube.MostDetailedMip = 0;  // Index of the first mip we want to see

	// Make the SRV
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeSRV;
	Graphics::Device->CreateShaderResourceView(cubeMapTexture.Get(), &srvDesc, cubeSRV.GetAddressOf());

	// Send back the SRV, which is what we need for our shaders
	return cubeSRV;
}


// --------------------------------------------------------
// Handle resizing to match the new window size
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	for(std::shared_ptr<Camera> camera : cameras)
		camera->UpdateProjectionMatrix((float) Window::Width() / Window::Height());
}


// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	this->totalTime = totalTime;

	UpdateImGui(deltaTime, totalTime);

	// Example input checking: Quit if the escape key is pressed
	if (Input::KeyDown(VK_ESCAPE))
		Window::Quit();

	BuildUI();

	GetCamera()->Update(deltaTime);

	// Animate material texture - only for demonstrating UV offset
	materials[0]->SetUVOffset(XMFLOAT2(totalTime, totalTime));

	for(std::shared_ptr<Entity> e : entities)
		e->GetTransform()->Rotate(0, deltaTime, 0);
}

void Game::UpdateImGui(float deltaTime, float totalTime)
{
	// Feed fresh data to ImGui
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = deltaTime;
	io.DisplaySize.x = (float)Window::Width();
	io.DisplaySize.y = (float)Window::Height();

	// Reset the frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// Determine new input capture
	Input::SetKeyboardCapture(io.WantCaptureKeyboard);
	Input::SetMouseCapture(io.WantCaptureMouse);

	// Show the demo window
	if (!isDemoWindowHidden)
		ImGui::ShowDemoWindow();
}

void Game::BuildUI()
{
	ImGui::Begin("Custom Window");

	ImGui::Text("Framerate: %f", ImGui::GetIO().Framerate);
	ImGui::Text("Window Size: %ix%i", Window::Width(), Window::Height());
	ImGui::ColorEdit4("Background Color", backgroundColor);

	// Button to toggle visibility of the ImGui demo window
	if(ImGui::Button("Toggle Demo Window"))
		isDemoWindowHidden = !isDemoWindowHidden;

	if(ImGui::TreeNode("Lights"))
	{
		for(int i = 0; i < lights.size(); i++)
		{
			ImGui::PushID(&lights[i]);
			if(ImGui::TreeNode("Light", "Light %i", i))
			{
				ImGui::DragInt("Light Type (0: Dir, 1: Point, 2: Spot)", &lights[i].LightType, 0.1f, 0, 2);

				ImGui::DragFloat3("Location", &lights[i].Location.x, 0.01f, -10, 10);
				ImGui::DragFloat3("Color", &lights[i].Color.x, 0.01f, 0, 1);
				ImGui::DragFloat3("Direction", &lights[i].Direction.x, 0.01f, -1, 1);
				ImGui::DragFloat("Intensity", &lights[i].Intensity, 0.01f, 0, 10);
				ImGui::DragFloat("Range", &lights[i].Range, 0.01f, 0, 10);
				ImGui::TreePop();
			}
			ImGui::PopID();
		}
		ImGui::TreePop();
	}

	// Slider control for setting the active camera
	int index = activeCameraIndex;
	ImGui::DragInt("Camera Index", &index, 0.01f, 0, (int) cameras.size() - 1);
	activeCameraIndex = index;

	// Active camera information
	XMFLOAT3 location = GetCamera()->GetTransform().GetLocation();
	ImGui::Text("Location: (%.2f, %.2f, %.2f)", location.x, location.y, location.z);
	XMFLOAT3 rotation = GetCamera()->GetTransform().GetPitchYawRoll();
	ImGui::Text("Rotation (Radians): (%.2f, %.2f, %.2f)", rotation.x, rotation.y, rotation.z);
	ImGui::Text("FOV (Degrees): %.1f", GetCamera()->GetFOV());

	if(ImGui::TreeNode("Meshes"))
	{
		// Display the vertex and triangle count of each mesh
		for(std::shared_ptr<Mesh> mesh : meshes)
		{
			if(ImGui::TreeNode(mesh->GetName().c_str()))
			{
				ImGui::Text("Vertices: %i", mesh->GetVertexCount());
				ImGui::Text("Triangles: %i", mesh->GetIndexCount() / 3);
				ImGui::Text("Indices: %i", mesh->GetIndexCount());
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
	if(ImGui::TreeNode("Entities"))
	{
		// Display the transform information for each entity
		for(int i = 0; i < entities.size(); i++)
		{
			std::shared_ptr<Entity> entity = entities[i];

			ImGui::PushID(entity.get());
			if(ImGui::TreeNode("Entity", "Entity %i", i))
			{
				ImGui::Text("Mesh: %s", entity->GetMesh()->GetName().c_str());

				Transform* transform = entity->GetTransform();

				XMFLOAT3 location = transform->GetLocation();
				XMFLOAT3 rotation = transform->GetPitchYawRoll();
				XMFLOAT3 scale = transform->GetScale();

				if(ImGui::DragFloat3("Location", &location.x, 0.1f))
					transform->SetLocation(location);
				if(ImGui::DragFloat3("Rotation", &rotation.x, 0.1f))
					transform->SetRotation(rotation);
				if(ImGui::DragFloat3("Scale", &scale.x, 0.1f))
					transform->SetScale(scale);

				ImGui::Text("Mesh Index Count: %i", entity->GetMesh()->GetIndexCount());

				ImGui::TreePop();
			}
			ImGui::PopID();
		}
		ImGui::TreePop();
	}

	ImGui::End();
}


// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Frame START
	// - These things should happen ONCE PER FRAME
	// - At the beginning of Game::Draw() before drawing *anything*
	{
		// Clear the back buffer (erase what's on screen) and depth buffer
		Graphics::Context->ClearRenderTargetView(Graphics::BackBufferRTV.Get(),	backgroundColor);
		Graphics::Context->ClearDepthStencilView(Graphics::DepthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	// DRAW geometry
	// - These steps are generally repeated for EACH object you draw
	// - Other Direct3D calls will also be necessary to do more complex things
	{
		// Draw all entities
		for(std::shared_ptr<Entity> e : entities)
		{
			// Manually set the color of ambient light on the entity material's pixel shader
			e->GetMaterial()->GetPixelShader()->SetFloat3("ambient", ambientLightColor);

			// Give light data for directional light
			e->GetMaterial()->GetPixelShader()->SetInt("lightCount", (int) lights.size());
			e->GetMaterial()->GetPixelShader()->SetData("lights", &lights[0], sizeof(Light) * (int) lights.size());

			e->Draw(GetCamera(), totalTime);
		}

		// Draw skybox
		skybox->Draw(GetCamera());
	}

	// Frame END
	// - These should happen exactly ONCE PER FRAME
	// - At the very end of the frame (after drawing *everything*)
	{
		ImGui::Render(); // Turn's this frame's UI into renderable triangles
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // Draws to the screen

		// Present at the end of the frame
		bool vsync = Graphics::VsyncState();
		Graphics::SwapChain->Present(
			vsync ? 1 : 0,
			vsync ? 0 : DXGI_PRESENT_ALLOW_TEARING);

		// Re-bind back buffer and depth buffer after presenting
		Graphics::Context->OMSetRenderTargets(
			1,
			Graphics::BackBufferRTV.GetAddressOf(),
			Graphics::DepthBufferDSV.Get());
	}
}



