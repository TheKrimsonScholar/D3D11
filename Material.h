#pragma once

#include <d3d11.h>
#include <memory>

#include <DirectXMath.h>

#include "SimpleShader.h"

class Material
{
private:
	std::shared_ptr<SimpleVertexShader> vertexShader;
	std::shared_ptr<SimplePixelShader> pixelShader;

	DirectX::XMFLOAT4 color;

public:
	Material(std::shared_ptr<SimpleVertexShader> vertexShader, std::shared_ptr<SimplePixelShader> pixelShader, DirectX::XMFLOAT4 color);
	~Material();

	std::shared_ptr<SimpleVertexShader> GetVertexShader() { return vertexShader; };
	std::shared_ptr<SimplePixelShader> GetPixelShader() { return pixelShader; };
	DirectX::XMFLOAT4 GetColor() { return color; };

	void SetVertexShader(std::shared_ptr<SimpleVertexShader> value) { vertexShader = value; };
	void SetPixelShader(std::shared_ptr<SimplePixelShader> value) { pixelShader = value; };
	void SetColor(DirectX::XMFLOAT4 value) { color = value; };
};