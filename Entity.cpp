#include "Entity.h"
#include "Graphics.h"

using namespace DirectX;

Entity::Entity(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material) :
	mesh(mesh), material(material)
{
	
}

void Entity::Draw(std::shared_ptr<Camera> camera)
{
	material->GetVertexShader()->SetShader();
	material->GetPixelShader()->SetShader();

	// Create data to be sent to the vertex shader
	std::shared_ptr<SimpleVertexShader> vs = material->GetVertexShader();
	vs->SetMatrix4x4("worldMatrix", transform.GetWorldMatrix());
	vs->SetMatrix4x4("viewMatrix", camera->GetViewMatrix());
	vs->SetMatrix4x4("projMatrix", camera->GetProjectionMatrix());
	vs->SetFloat4("colorTint", material->GetColor());

	vs->CopyAllBufferData();

	mesh->Draw();
}

Transform* Entity::GetTransform() { return &transform; }
std::shared_ptr<Mesh> Entity::GetMesh() { return mesh; }