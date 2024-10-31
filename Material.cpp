#include "Material.h"

Material::Material(std::shared_ptr<SimpleVertexShader> vertexShader, std::shared_ptr<SimplePixelShader> pixelShader, DirectX::XMFLOAT4 color, float roughness) : 
	vertexShader(vertexShader), pixelShader(pixelShader), color(color), roughness(roughness)
{

}
Material::~Material()
{

}

void Material::PrepareMaterial()
{
	for(auto srv : textureSRVs)
		pixelShader->SetShaderResourceView(srv.first.c_str(), srv.second);
	for(auto sampler : samplers)
		pixelShader->SetSamplerState(sampler.first.c_str(), sampler.second);
}

void Material::AddTextureSRV(std::string identifier, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv)
{
	textureSRVs.insert({identifier, srv});
}
void Material::AddSampler(std::string identifier, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler)
{
	samplers.insert({identifier, sampler});
}