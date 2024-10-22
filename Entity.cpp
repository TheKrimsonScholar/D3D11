#include "Entity.h"
#include "Graphics.h"

using namespace DirectX;

Entity::Entity(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material) :
	mesh(mesh), material(material)
{
	
}

void Entity::Draw(std::shared_ptr<Camera> camera, float totalTime)
{
	std::shared_ptr<SimpleVertexShader> vs = material->GetVertexShader();
	std::shared_ptr<SimplePixelShader> ps = material->GetPixelShader();

	material->GetVertexShader()->SetShader();
	material->GetPixelShader()->SetShader();

	// Create data to be sent to the vertex shader
	vs->SetMatrix4x4("worldMatrix", transform.GetWorldMatrix());
	vs->SetMatrix4x4("worldInvTranspose", transform.GetWorldInverseTransposeMatrix());
	vs->SetMatrix4x4("viewMatrix", camera->GetViewMatrix());
	vs->SetMatrix4x4("projMatrix", camera->GetProjectionMatrix());
	
	ps->SetFloat4("colorTint", material->GetColor());
	ps->SetFloat("roughness", material->GetRoughness());
	ps->SetFloat3("cameraLocation", camera->GetTransform().GetLocation());
	ps->SetFloat("totalTime", totalTime);

	vs->CopyAllBufferData();
	ps->CopyAllBufferData();

	mesh->Draw();
}

Transform* Entity::GetTransform() { return &transform; }
std::shared_ptr<Mesh> Entity::GetMesh() { return mesh; }