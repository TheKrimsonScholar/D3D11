#include "Game.h"
#include "Graphics.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Window.h"
#include "BufferStructs.h"
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

		// Ensure the pipeline knows how to interpret all the numbers stored in
		// the vertex buffer. For this course, all of your vertices will probably
		// have the same layout, so we can just set this once at startup.
		Graphics::Context->IASetInputLayout(inputLayout.Get());

		// Set the active vertex and pixel shaders
		//  - Once you start applying different shaders to different objects,
		//    these calls will need to happen multiple times per frame
		Graphics::Context->VSSetShader(vertexShader.Get(), 0, 0);
		Graphics::Context->PSSetShader(pixelShader.Get(), 0, 0);
	}

	D3D11_BUFFER_DESC cbData = {};
	cbData.ByteWidth = (sizeof(VSData) + 15) / 16 * 16; // Dirty way of aligning to nearest 16-byte increment
	cbData.Usage = D3D11_USAGE_DYNAMIC; // Can be changed at any time
	cbData.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbData.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	Graphics::Device->CreateBuffer(&cbData, 0, constantBuffer.GetAddressOf());

	Graphics::Context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

	// Initialize ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(Window::Handle());
	ImGui_ImplDX11_Init(Graphics::Device.Get(), Graphics::Context.Get());

	// Pick a style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();
	//ImGui::StyleColorsClassic();
}


// --------------------------------------------------------
// Clean up memory or objects created by this class
// 
// Note: Using smart pointers means there probably won't
//       be much to manually clean up here!
// --------------------------------------------------------
Game::~Game()
{
	delete[] worldMatrix;
	delete[] colorTint;

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
	// BLOBs (or Binary Large OBjects) for reading raw data from external files
	// - This is a simplified way of handling big chunks of external data
	// - Literally just a big array of bytes read from a file
	ID3DBlob* pixelShaderBlob;
	ID3DBlob* vertexShaderBlob;

	// Loading shaders
	//  - Visual Studio will compile our shaders at build time
	//  - They are saved as .cso (Compiled Shader Object) files
	//  - We need to load them when the application starts
	{
		// Read our compiled shader code files into blobs
		// - Essentially just "open the file and plop its contents here"
		// - Uses the custom FixPath() helper from Helpers.h to ensure relative paths
		// - Note the "L" before the string - this tells the compiler the string uses wide characters
		D3DReadFileToBlob(FixPath(L"PixelShader.cso").c_str(), &pixelShaderBlob);
		D3DReadFileToBlob(FixPath(L"VertexShader.cso").c_str(), &vertexShaderBlob);

		// Create the actual Direct3D shaders on the GPU
		Graphics::Device->CreatePixelShader(
			pixelShaderBlob->GetBufferPointer(),	// Pointer to blob's contents
			pixelShaderBlob->GetBufferSize(),		// How big is that data?
			0,										// No classes in this shader
			pixelShader.GetAddressOf());			// Address of the ID3D11PixelShader pointer

		Graphics::Device->CreateVertexShader(
			vertexShaderBlob->GetBufferPointer(),	// Get a pointer to the blob's contents
			vertexShaderBlob->GetBufferSize(),		// How big is that data?
			0,										// No classes in this shader
			vertexShader.GetAddressOf());			// The address of the ID3D11VertexShader pointer
	}

	// Create an input layout 
	//  - This describes the layout of data sent to a vertex shader
	//  - In other words, it describes how to interpret data (numbers) in a vertex buffer
	//  - Doing this NOW because it requires a vertex shader's byte code to verify against!
	//  - Luckily, we already have that loaded (the vertex shader blob above)
	{
		D3D11_INPUT_ELEMENT_DESC inputElements[2] = {};

		// Set up the first element - a position, which is 3 float values
		inputElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;				// Most formats are described as color channels; really it just means "Three 32-bit floats"
		inputElements[0].SemanticName = "POSITION";							// This is "POSITION" - needs to match the semantics in our vertex shader input!
		inputElements[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// How far into the vertex is this?  Assume it's after the previous element

		// Set up the second element - a color, which is 4 more float values
		inputElements[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;			// 4x 32-bit floats
		inputElements[1].SemanticName = "COLOR";							// Match our vertex shader input!
		inputElements[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// After the previous element

		// Create the input layout, verifying our description against actual shader code
		Graphics::Device->CreateInputLayout(
			inputElements,							// An array of descriptions
			2,										// How many elements in that array?
			vertexShaderBlob->GetBufferPointer(),	// Pointer to the code of a shader that uses this layout
			vertexShaderBlob->GetBufferSize(),		// Size of the shader code that uses this layout
			inputLayout.GetAddressOf());			// Address of the resulting ID3D11InputLayout pointer
	}
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
	/*Vertex vertices[] =
	{
		{ XMFLOAT3(+0.0f, +0.5f, +0.0f), red },
		{ XMFLOAT3(+0.5f, -0.5f, +0.0f), blue },
		{ XMFLOAT3(-0.5f, -0.5f, +0.0f), green },
	};*/

	// Set up indices, which tell us which vertices to use and in which order
	// - This is redundant for just 3 vertices, but will be more useful later
	// - Indices are technically not required if the vertices are in the buffer 
	//    in the correct order and each one will be used exactly once
	// - But just to see how it's done...
	unsigned int indices[] = { 0, 1, 2 };


	// Create a VERTEX BUFFER
	// - This holds the vertex data of triangles for a single object
	// - This buffer is created on the GPU, which is where the data needs to
	//    be if we want the GPU to act on it (as in: draw it to the screen)
	{
		// First, we need to describe the buffer we want Direct3D to make on the GPU
		//  - Note that this variable is created on the stack since we only need it once
		//  - After the buffer is created, this description variable is unnecessary
		D3D11_BUFFER_DESC vbd = {};
		vbd.Usage = D3D11_USAGE_IMMUTABLE;	// Will NEVER change
		vbd.ByteWidth = sizeof(Vertex) * 3;       // 3 = number of vertices in the buffer
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER; // Tells Direct3D this is a vertex buffer
		vbd.CPUAccessFlags = 0;	// Note: We cannot access the data from C++ (this is good)
		vbd.MiscFlags = 0;
		vbd.StructureByteStride = 0;

		// Create the proper struct to hold the initial vertex data
		// - This is how we initially fill the buffer with data
		// - Essentially, we're specifying a pointer to the data to copy
		D3D11_SUBRESOURCE_DATA initialVertexData = {};
		initialVertexData.pSysMem = vertices; // pSysMem = Pointer to System Memory

		// Actually create the buffer on the GPU with the initial data
		// - Once we do this, we'll NEVER CHANGE DATA IN THE BUFFER AGAIN
		Graphics::Device->CreateBuffer(&vbd, &initialVertexData, vertexBuffer.GetAddressOf());
	}

	// Create an INDEX BUFFER
	// - This holds indices to elements in the vertex buffer
	// - This is most useful when vertices are shared among neighboring triangles
	// - This buffer is created on the GPU, which is where the data needs to
	//    be if we want the GPU to act on it (as in: draw it to the screen)
	{
		// Describe the buffer, as we did above, with two major differences
		//  - Byte Width (3 unsigned integers vs. 3 whole vertices)
		//  - Bind Flag (used as an index buffer instead of a vertex buffer) 
		D3D11_BUFFER_DESC ibd = {};
		ibd.Usage = D3D11_USAGE_IMMUTABLE;	// Will NEVER change
		ibd.ByteWidth = sizeof(unsigned int) * 3;	// 3 = number of indices in the buffer
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;	// Tells Direct3D this is an index buffer
		ibd.CPUAccessFlags = 0;	// Note: We cannot access the data from C++ (this is good)
		ibd.MiscFlags = 0;
		ibd.StructureByteStride = 0;

		// Specify the initial data for this buffer, similar to above
		D3D11_SUBRESOURCE_DATA initialIndexData = {};
		initialIndexData.pSysMem = indices; // pSysMem = Pointer to System Memory

		// Actually create the buffer with the initial data
		// - Once we do this, we'll NEVER CHANGE THE BUFFER AGAIN
		Graphics::Device->CreateBuffer(&ibd, &initialIndexData, indexBuffer.GetAddressOf());
	}

	// Star mesh
	meshes.push_back(std::make_shared<Mesh>(
		"Star",
		11,
		new Vertex[11]{
		{ DirectX::XMFLOAT3(+0.0f, +0.75f, +0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) }, // 0
		{ DirectX::XMFLOAT3(-0.5f, +0.25f, +0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) }, // 1
		{ DirectX::XMFLOAT3(+0.5f, +0.25f, +0.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) }, // 2
		{ DirectX::XMFLOAT3(-0.375f, -0.625f, +0.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) }, // 3
		{ DirectX::XMFLOAT3(+0.375f, -0.625f, +0.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) }, // 4
		{ DirectX::XMFLOAT3(-0.125f, +0.25f, +0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) }, // 5
		{ DirectX::XMFLOAT3(+0.125f, +0.25f, +0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) }, // 6
		{ DirectX::XMFLOAT3(-0.25f, -0.125f, +0.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) }, // 7
		{ DirectX::XMFLOAT3(+0.25f, -0.125f, +0.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) }, // 8
		{ DirectX::XMFLOAT3(+0.0f, -0.375f, +0.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) }, // 9
		{ DirectX::XMFLOAT3(+0.0f, +0.0f, +0.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) } }, // 10
		30,
		new unsigned int[30] {
			0, 6, 5,
			1, 5, 7,
			6, 2, 8,
			7, 9, 3,
			9, 8, 4,
			5, 6, 10,
			5, 10, 7,
			6, 8, 10,
			7, 10, 9,
			9, 10, 8}));

	// Triangle mesh
	meshes.push_back(std::make_shared<Mesh>(
		"Triangle",
		3, 
		new Vertex[3]{
		{ DirectX::XMFLOAT3(+0.0f, +0.75f, +0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ DirectX::XMFLOAT3(+0.5f, -0.5f, +0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ DirectX::XMFLOAT3(-0.5f, -0.5f, +0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) } }, 
		3, 
		new unsigned int[3] { 0, 1, 2 }));

	// Quad mesh
	meshes.push_back(std::make_shared<Mesh>(
		"Quad",
		4,
		new Vertex[4]{
		{ DirectX::XMFLOAT3(-0.75f, -0.75f, +0.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ DirectX::XMFLOAT3(+0.75f, -0.75f, +0.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ DirectX::XMFLOAT3(-0.75f, +0.75f, +0.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ DirectX::XMFLOAT3(+0.75f, +0.75f, +0.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) } },
		6,
		new unsigned int[6] { 2, 1, 0, 2, 3, 1 }));

	// Create an entity for each mesh
	for(std::shared_ptr<Mesh> m : meshes)
		entities.push_back(std::make_shared<Entity>(m));

	/* Create two extra star entities to show multiple entites using the same mesh */

	std::shared_ptr<Entity> newStar1 = std::make_shared<Entity>(meshes[0]);
	newStar1->GetTransform()->MoveAbsolute(0, 0.5f, 0);
	entities.push_back(newStar1);

	std::shared_ptr<Entity> newStar2 = std::make_shared<Entity>(meshes[0]);
	newStar2->GetTransform()->MoveAbsolute(0, -0.5f, 0);
	entities.push_back(newStar2);
}


// --------------------------------------------------------
// Handle resizing to match the new window size
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
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
	if (ImGui::Button("Toggle Demo Window"))
		isDemoWindowHidden = !isDemoWindowHidden;

	// Vertex offset and color tint controls
	ImGui::DragFloat3("Offset", worldMatrix, 0.1f, -1, 1);
	ImGui::ColorEdit4("ColorTint", colorTint);

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

	/* Controls for manipulating vertices */

	ImGui::Text("Vertex 0");
	ImGui::SliderFloat("Vertex 0: X", &vertices[0].Position.x, -1.0f, 1.0f);
	ImGui::SliderFloat("Vertex 0: Y", &vertices[0].Position.y, -1.0f, 1.0f);
	ImGui::SliderFloat("Vertex 0: Z", &vertices[0].Position.z, -1.0f, 1.0f);

	ImGui::Text("Vertex 1");
	ImGui::SliderFloat("Vertex 1: X", &vertices[1].Position.x, -1.0f, 1.0f);
	ImGui::SliderFloat("Vertex 1: Y", &vertices[1].Position.y, -1.0f, 1.0f);
	ImGui::SliderFloat("Vertex 1: Z", &vertices[1].Position.z, -1.0f, 1.0f);

	ImGui::Text("Vertex 2");
	ImGui::SliderFloat("Vertex 2: X", &vertices[2].Position.x, -1.0f, 1.0f);
	ImGui::SliderFloat("Vertex 2: Y", &vertices[2].Position.y, -1.0f, 1.0f);
	ImGui::SliderFloat("Vertex 2: Z", &vertices[2].Position.z, -1.0f, 1.0f);

	ImGui::End();
}
void EntityUI()
{

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

	// Create data to be sent to the vertex shader
	VSData vsData;
	vsData.worldMatrix = XMFLOAT4X4(worldMatrix);
	vsData.colorTint = XMFLOAT4(colorTint);

	// Write to the constant buffer so it can be used by the vertex shader
	D3D11_MAPPED_SUBRESOURCE mapped = {};
	Graphics::Context->Map(constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	memcpy(mapped.pData, &vsData, sizeof(VSData));
	Graphics::Context->Unmap(constantBuffer.Get(), 0);

	// DRAW geometry
	// - These steps are generally repeated for EACH object you draw
	// - Other Direct3D calls will also be necessary to do more complex things
	{
		// Set buffers in the input assembler (IA) stage
		//  - Do this ONCE PER OBJECT, since each object may have different geometry
		//  - For this demo, this step *could* simply be done once during Init()
		//  - However, this needs to be done between EACH DrawIndexed() call
		//     when drawing different geometry, so it's here as an example
		UINT stride = sizeof(Vertex);
		UINT worldMatrix = 0;
		Graphics::Context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &worldMatrix);
		Graphics::Context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		// Tell Direct3D to draw
		//  - Begins the rendering pipeline on the GPU
		//  - Do this ONCE PER OBJECT you intend to draw
		//  - This will use all currently set Direct3D resources (shaders, buffers, etc)
		//  - DrawIndexed() uses the currently set INDEX BUFFER to look up corresponding
		//     vertices in the currently set VERTEX BUFFER
		//Graphics::Context->DrawIndexed(
		//	3,     // The number of indices to use (we could draw a subset if we wanted)
		//	0,     // Offset to the first index we want to use
		//	0);    // Offset to add to each index when looking up vertices

		// Draw all meshes
		//for(std::shared_ptr<Mesh> mesh : meshes)
			//mesh->Draw();

		// Draw all entities
		for(std::shared_ptr<Entity> e : entities)
			e->Draw(constantBuffer);
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



