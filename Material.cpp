#include "Material.h"

Material::Material(std::shared_ptr<SimpleVertexShader> vertexShader, std::shared_ptr<SimplePixelShader> pixelShader, DirectX::XMFLOAT4 color) : 
	vertexShader(vertexShader), pixelShader(pixelShader), color(color)
{

}
Material::~Material()
{

}