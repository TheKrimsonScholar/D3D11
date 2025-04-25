#pragma once

#include "Transform.h"
#include "Camera.h"
#include "Material.h"

#include "SimpleShader.h"

#include <memory>

struct Particle
{
	bool isAlive;
	float age;
	float lifetime;
	DirectX::XMFLOAT3 location;
	float rotation;
	DirectX::XMFLOAT3 velocity;
	DirectX::XMFLOAT3 acceleration;
	DirectX::XMFLOAT4 color;
};

class ParticleSystem
{
public:
	static inline std::shared_ptr<SimpleComputeShader> particleComputeShaderInitialize;
	static inline std::shared_ptr<SimpleComputeShader> particleComputeShaderEmit;
	static inline std::shared_ptr<SimpleComputeShader> particleComputeShaderUpdate;
	static inline std::shared_ptr<SimpleComputeShader> particleComputeShaderDraw;

	static inline std::shared_ptr<SimpleVertexShader> particleVertexShader;
	static inline std::shared_ptr<SimplePixelShader> particlePixelShader;

private:
	Transform transform;
	std::shared_ptr<Material> material;

	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	Microsoft::WRL::ComPtr<ID3D11Buffer> particlePoolBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> particlePoolSRV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> particlePoolUAV;
	Microsoft::WRL::ComPtr<ID3D11Buffer> deadListBuffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> deadListUAV;
	Microsoft::WRL::ComPtr<ID3D11Buffer> drawListBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> drawListSRV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> drawListUAV;
	Microsoft::WRL::ComPtr<ID3D11Buffer> deadListCounterBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> drawArgsBuffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> drawArgsUAV;

	Microsoft::WRL::ComPtr<ID3D11Buffer> stagingBuffer;

	uint32_t maxParticles;
	float emissionPeriodMin;
	float emissionPeriodMax;
	uint32_t emissionCountMin;
	uint32_t emissionCountMax;

	DirectX::XMFLOAT4 colorTintMin;
	DirectX::XMFLOAT4 colorTintMax;
	float particleLifetimeMin;
	float particleLifetimeMax;
	float rotationMin;
	float rotationMax;
	DirectX::XMFLOAT3 locationMin;
	DirectX::XMFLOAT3 locationMax;
	DirectX::XMFLOAT3 velocityMin;
	DirectX::XMFLOAT3 velocityMax;
	DirectX::XMFLOAT3 accelerationMin;
	DirectX::XMFLOAT3 accelerationMax;

	float emissionCooldown = 0.0f;

public:
	ParticleSystem();
	ParticleSystem(DirectX::XMFLOAT3 location, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale, std::shared_ptr<Material> material);
	~ParticleSystem();

	void Initialize();
	void CreateBuffers();

	void Update(float deltaTime);
	void Emit(uint32_t count);
	void Draw(std::shared_ptr<Camera> camera);
	
	const Transform& GetTransform() const { return transform; }

	void SetMaxParticles(uint32_t maxParticles);
	void SetEmissionPeriodRange(float min, float max);
	void SetEmissionCountRange(uint32_t min, uint32_t max);
	void SetColorTintRange(DirectX::XMFLOAT4 min, DirectX::XMFLOAT4 max);
	void SetParticleLifetimeRange(float min, float max);
	void SetParticleRotationRange(float min, float max);
	void SetParticleLocationRange(DirectX::XMFLOAT3 min, DirectX::XMFLOAT3 max);
	void SetParticleVelocityRange(DirectX::XMFLOAT3 min, DirectX::XMFLOAT3 max);
	void SetParticleAccelerationRange(DirectX::XMFLOAT3 min, DirectX::XMFLOAT3 max);
};