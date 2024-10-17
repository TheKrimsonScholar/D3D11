#include "Game.h"
#include "Graphics.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Window.h"
#include <memory>
#include <iostream>

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
	LoadShaders();
	CreateMaterials();
	CreateGeometry();

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
}

void Game::CreateMaterials()
{
	materials.push_back(std::make_shared<Material>(vertexShader, pixelShader, XMFLOAT4(1, 1, 1, 1)));
	materials.push_back(std::make_shared<Material>(vertexShader, pixelShader, XMFLOAT4(1, 0, 0, 1)));
	materials.push_back(std::make_shared<Material>(vertexShader, pixelShader, XMFLOAT4(0, 1, 0, 1)));
	materials.push_back(std::make_shared<Material>(vertexShader, pixelShader, XMFLOAT4(0, 0, 1, 1)));
}

// --------------------------------------------------------
// Creates the geometry we're going to draw
// --------------------------------------------------------
void Game::CreateGeometry()
{
	// Create some temporary variables to represent colors
	// - Not necessary, just makes things more readable
	/*XMFLOAT4 red = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);*/

	// Set up the vertices of the triangle we would like to draw
	// - We're going to copy this array, exactly as it exists in CPU memory
	//    over to a Direct3D-controlled data structure on the GPU (the vertex buffer)
	// - Note: Since we don't have a camera or really any concept of
	//    a "3d world" yet, we're simply describing positions within the
	//    bounds of how the rasterizer sees our screen: [-1 to +1] on X and Y
	// - This means (0,0) is at the very center of the screen.
	// - These are known as "Normalized Device Coordinates" or "Homogeneous 
	//    Screen Coords", which are ways to describe a position without
	//    knowing the exact size (in pixels) of the image/window/etc.  
	// - Long story short: Resizing the window also resizes the triangle,
	//    since we're describing the triangle in terms of the window itself

	// Star mesh
	//meshes.push_back(std::make_shared<Mesh>(
	//	"Star",
	//	11,
	//	new Vertex[11]{
	//	{ DirectX::XMFLOAT3(+0.0f, +0.75f, +0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) }, // 0
	//	{ DirectX::XMFLOAT3(-0.5f, +0.25f, +0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) }, // 1
	//	{ DirectX::XMFLOAT3(+0.5f, +0.25f, +0.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) }, // 2
	//	{ DirectX::XMFLOAT3(-0.375f, -0.625f, +0.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) }, // 3
	//	{ DirectX::XMFLOAT3(+0.375f, -0.625f, +0.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) }, // 4
	//	{ DirectX::XMFLOAT3(-0.125f, +0.25f, +0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) }, // 5
	//	{ DirectX::XMFLOAT3(+0.125f, +0.25f, +0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) }, // 6
	//	{ DirectX::XMFLOAT3(-0.25f, -0.125f, +0.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) }, // 7
	//	{ DirectX::XMFLOAT3(+0.25f, -0.125f, +0.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) }, // 8
	//	{ DirectX::XMFLOAT3(+0.0f, -0.375f, +0.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) }, // 9
	//	{ DirectX::XMFLOAT3(+0.0f, +0.0f, +0.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) } }, // 10
	//	30,
	//	new unsigned int[30] {
	//		0, 6, 5,
	//		1, 5, 7,
	//		6, 2, 8,
	//		7, 9, 3,
	//		9, 8, 4,
	//		5, 6, 10,
	//		5, 10, 7,
	//		6, 8, 10,
	//		7, 10, 9,
	//		9, 10, 8}));

	//// Triangle mesh
	//meshes.push_back(std::make_shared<Mesh>(
	//	"Triangle",
	//	3, 
	//	new Vertex[3]{
	//	{ DirectX::XMFLOAT3(+0.0f, +0.75f, +0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
	//	{ DirectX::XMFLOAT3(+0.5f, -0.5f, +0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
	//	{ DirectX::XMFLOAT3(-0.5f, -0.5f, +0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) } }, 
	//	3, 
	//	new unsigned int[3] { 0, 1, 2 }));

	//// Quad mesh
	//meshes.push_back(std::make_shared<Mesh>(
	//	"Quad",
	//	4,
	//	new Vertex[4]{
	//	{ DirectX::XMFLOAT3(-0.75f, -0.75f, +0.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
	//	{ DirectX::XMFLOAT3(+0.75f, -0.75f, +0.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
	//	{ DirectX::XMFLOAT3(-0.75f, +0.75f, +0.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
	//	{ DirectX::XMFLOAT3(+0.75f, +0.75f, +0.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) } },
	//	6,
	//	new unsigned int[6] { 2, 1, 0, 2, 3, 1 }));
	meshes.push_back(std::make_shared<Mesh>(FixPath(L"../../Assets/Models/sphere.obj").c_str()));

	// Create an entity for each mesh
	// Gradually offset Z to prevent z-fighting
	float zOffset = 0;
	for(unsigned int i = 0; i < meshes.size(); i++)
	{
		std::shared_ptr<Entity> newEntity = std::make_shared<Entity>(meshes[i], materials[i]);
		newEntity->GetTransform()->MoveAbsolute(0, 0, zOffset);
		entities.push_back(newEntity);

		zOffset += 0.1f;
	}

	/* Create two extra star entities to show multiple entites using the same mesh */

	/*std::shared_ptr<Entity> newStar1 = std::make_shared<Entity>(meshes[0], materials[1]);
	newStar1->GetTransform()->MoveAbsolute(0, 0.5f, 1);
	entities.push_back(newStar1);

	std::shared_ptr<Entity> newStar2 = std::make_shared<Entity>(meshes[0], materials[2]);
	newStar2->GetTransform()->MoveAbsolute(0, -0.5f, 2);
	entities.push_back(newStar2);*/
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
	{
		e->GetTransform()->MoveAbsolute(cos(totalTime) * deltaTime, 0, 0);
		e->GetTransform()->Rotate(0, 0, deltaTime);

		float scaleAmount = 1 - (0.05f * deltaTime);
		e->GetTransform()->Scale(scaleAmount, scaleAmount, scaleAmount);
	}
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
	static int sliderValue = 50;

	ImGui::Begin("Custom Window");

	ImGui::Text("Framerate: %f", ImGui::GetIO().Framerate);
	ImGui::Text("Window Size: %ix%i", Window::Width(), Window::Height());
	ImGui::ColorEdit4("Background Color", backgroundColor);

	// Button to toggle visibility of the ImGui demo window
	if(ImGui::Button("Toggle Demo Window"))
		isDemoWindowHidden = !isDemoWindowHidden;

	// Slider control for setting the active camera
	int index = activeCameraIndex;
	ImGui::DragInt("Camera Index", &index, 0.01f, 0, cameras.size() - 1);
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
			e->Draw(GetCamera());
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



