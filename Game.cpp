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
	CreateWICTextureFromFile(Graphics::Device.Get(), FixPath(L"../../../Assets/Specular Maps/brokentiles.png").c_str(), 0, textureSRV.GetAddressOf());

	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
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
}

void Game::CreateMaterials()
{
	materials.push_back(std::make_shared<Material>(vertexShader, pixelShader, XMFLOAT4(1, 1, 1, 1), 0.25f)); // White material
	materials.push_back(std::make_shared<Material>(vertexShader, pixelShader, XMFLOAT4(1, 0, 0, 1), 0.5f)); // Red material
	materials.push_back(std::make_shared<Material>(vertexShader, pixelShader, XMFLOAT4(1, 0, 1, 1), 1.0f)); // Purple material
	materials.push_back(std::make_shared<Material>(vertexShader, normalPixelShader, XMFLOAT4(1, 1, 1, 1), 0.0f)); // Normal material
	materials.push_back(std::make_shared<Material>(vertexShader, uvPixelShader, XMFLOAT4(1, 1, 1, 1), 0.25f)); // UV material
	materials.push_back(std::make_shared<Material>(vertexShader, customPixelShader, XMFLOAT4(1, 1, 1, 1), 0.75f)); // Custom material
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

	/* Create entities */
	float spacing = 1.5f;
	for(unsigned int i = 0; i < meshes.size(); i++)
	{
		std::shared_ptr<Entity> newEntity = std::make_shared<Entity>(meshes[i], materials[0]); // Use white material
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

	/*directionalLight.LightType = LIGHT_TYPE_DIRECTIONAL;
	directionalLight.Direction = DirectX::XMFLOAT3(1, 0, 0);
	directionalLight.Color = DirectX::XMFLOAT3(1, 0, 0);
	directionalLight.Intensity = 1.0f;

	Light directionalLight2 = {};
	directionalLight2.LightType = LIGHT_TYPE_DIRECTIONAL;
	directionalLight2.Direction = DirectX::XMFLOAT3(0, -1, 0);
	directionalLight2.Color = DirectX::XMFLOAT3(0, 1, 0);
	directionalLight2.Intensity = 1.0f;

	Light directionalLight3 = {};
	directionalLight3.LightType = LIGHT_TYPE_DIRECTIONAL;
	directionalLight3.Direction = DirectX::XMFLOAT3(-1, 1, -0.5f);
	directionalLight3.Color = DirectX::XMFLOAT3(0, 0, 1);
	directionalLight3.Intensity = 1.0f;

	Light pointLight1 = {};
	pointLight1.LightType = LIGHT_TYPE_POINT;
	pointLight1.Location = DirectX::XMFLOAT3(-1, -1.5f, 5);
	pointLight1.Color = DirectX::XMFLOAT3(1, 1, 1);
	pointLight1.Intensity = 1.0f;
	pointLight1.Range = 10.0f;

	Light pointLight2 = {};
	pointLight2.LightType = LIGHT_TYPE_POINT;
	pointLight2.Location = DirectX::XMFLOAT3(1, -1.5f, 5);
	pointLight2.Color = DirectX::XMFLOAT3(1, 1, 1);
	pointLight2.Intensity = 0.5f;
	pointLight2.Range = 10.0f;*/

	lights.push_back(directionalLight);
	lights.push_back(directionalLight2);
	lights.push_back(directionalLight3);
	lights.push_back(pointLight1);
	lights.push_back(pointLight2);
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
	UpdateImGui(deltaTime, totalTime);

	// Example input checking: Quit if the escape key is pressed
	if (Input::KeyDown(VK_ESCAPE))
		Window::Quit();

	BuildUI();

	GetCamera()->Update(deltaTime);

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
			// Set the color texture using the SRV
			e->GetMaterial()->GetPixelShader()->SetShaderResourceView("SurfaceColorTexture", textureSRV);
			e->GetMaterial()->GetPixelShader()->SetSamplerState("BasicSampler", samplerOptions);

			// Manually set the color of ambient light on the entity material's pixel shader
			e->GetMaterial()->GetPixelShader()->SetFloat3("ambient", ambientLightColor);

			// Give light data for directional light
			e->GetMaterial()->GetPixelShader()->SetInt("lightCount", (int) lights.size());
			e->GetMaterial()->GetPixelShader()->SetData("lights", &lights[0], sizeof(Light) * (int) lights.size());

			e->Draw(GetCamera(), totalTime);
		}
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



