#pragma once

#include <memory>

#include "Camera.h"
#include "SimpleShader.h"

class FluidVolume
{
private:
	static const size_t width = 256, height = 256, depth = 256;

public:
	static inline std::shared_ptr<SimpleComputeShader> fluidComputeShaderAdvection;
	static inline std::shared_ptr<SimpleComputeShader> fluidComputeShaderDivergence;
	static inline std::shared_ptr<SimpleComputeShader> fluidComputeShaderPressure;
	static inline std::shared_ptr<SimpleComputeShader> fluidComputeShaderProjection;

	static inline std::shared_ptr<SimpleVertexShader> fluidVertexShader;
	static inline std::shared_ptr<SimplePixelShader> fluidPixelShader;

private:
	Transform transform;

	Microsoft::WRL::ComPtr<ID3D11Texture3D> velocityPreviousTexture;
	Microsoft::WRL::ComPtr<ID3D11Texture3D> velocityCurrentTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> velocityPreviousSRV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> velocityCurrentUAV;
	
	Microsoft::WRL::ComPtr<ID3D11Texture3D> pressurePreviousTexture;
	Microsoft::WRL::ComPtr<ID3D11Texture3D> pressureCurrentTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pressurePreviousSRV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> pressureCurrentUAV;
	
	Microsoft::WRL::ComPtr<ID3D11Texture3D> divergenceTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> divergenceSRV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> divergenceUAV;

	Microsoft::WRL::ComPtr<ID3D11Texture3D> densityPreviousTexture;
	Microsoft::WRL::ComPtr<ID3D11Texture3D> densityCurrentTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> densityPreviousSRV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> densityCurrentUAV;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;

public:
	FluidVolume();
	~FluidVolume();

	void Initialize();
	void CreateBuffers();

	void Update(float deltaTime);
	void Draw(std::shared_ptr<Camera> camera);
};