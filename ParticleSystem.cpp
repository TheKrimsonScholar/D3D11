#include "ParticleSystem.h"

#include "Graphics.h"

ParticleSystem::ParticleSystem() : 
	transform(), material(nullptr),
	maxParticles(), emissionPeriodMin(), emissionPeriodMax(), emissionCountMin(), emissionCountMax(),
	colorTintMin(), colorTintMax(), particleLifetimeMin(), particleLifetimeMax(), rotationMin(), rotationMax(), locationMin(), locationMax(), velocityMin(), velocityMax(), accelerationMin(), accelerationMax()
{

}
ParticleSystem::ParticleSystem(DirectX::XMFLOAT3 location, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale, std::shared_ptr<Material> material) :
	transform(location, rotation, scale), material(material),
	maxParticles(), emissionPeriodMin(), emissionPeriodMax(), emissionCountMin(), emissionCountMax(),
	colorTintMin(), colorTintMax(), particleLifetimeMin(), particleLifetimeMax(), rotationMin(), rotationMax(), locationMin(), locationMax(), velocityMin(), velocityMax(), accelerationMin(), accelerationMax()
{

}
ParticleSystem::~ParticleSystem()
{

}

void ParticleSystem::Initialize()
{
	/* Create all buffers */

	CreateBuffers();

	// Constant buffer for dead list count
	D3D11_BUFFER_DESC deadListCountBufferDesc = {};
	deadListCountBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	deadListCountBufferDesc.ByteWidth = 16;
	deadListCountBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	deadListCountBufferDesc.CPUAccessFlags = 0;

	Graphics::Device->CreateBuffer(&deadListCountBufferDesc, nullptr, deadListCounterBuffer.GetAddressOf());

	// DrawArgs buffer
	D3D11_BUFFER_DESC drawArgsBufferDesc = {};
	drawArgsBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	drawArgsBufferDesc.ByteWidth = sizeof(UINT) * 5; // 5 UINTs for args
	drawArgsBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	drawArgsBufferDesc.CPUAccessFlags = 0;
	drawArgsBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
	drawArgsBufferDesc.StructureByteStride = 0;
	
	Graphics::Device->CreateBuffer(&drawArgsBufferDesc, nullptr, drawArgsBuffer.GetAddressOf());

	// UAV for DrawArgs
	D3D11_UNORDERED_ACCESS_VIEW_DESC drawArgsUAVDesc = {};
	drawArgsUAVDesc.Format = DXGI_FORMAT_R32_UINT;
	drawArgsUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	drawArgsUAVDesc.Buffer.FirstElement = 0;
	drawArgsUAVDesc.Buffer.NumElements = 5; // 5 UINTs for draw arguments

	Graphics::Device->CreateUnorderedAccessView(drawArgsBuffer.Get(), &drawArgsUAVDesc, drawArgsUAV.GetAddressOf());

	// Staging buffer for debugging
	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = sizeof(D3D11_DRAW_INDEXED_INSTANCED_INDIRECT_ARGS); // Adjust size as needed
	desc.Usage = D3D11_USAGE_STAGING;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.BindFlags = 0;

	Graphics::Device->CreateBuffer(&desc, nullptr, stagingBuffer.GetAddressOf());

	// Dispatch particle initialization shader

	particleComputeShaderInitialize->SetShader();
	particleComputeShaderInitialize->SetUnorderedAccessView("DeadList", deadListUAV);

	particleComputeShaderInitialize->SetInt("maxParticles", maxParticles);

	particleComputeShaderInitialize->CopyAllBufferData();

	particleComputeShaderInitialize->DispatchByThreads(maxParticles, 1, 1);

	// Copy the counter from the dead list to initialize the counter buffer
	Graphics::Context->CopyStructureCount(deadListCounterBuffer.Get(), 0, deadListUAV.Get());
}
void ParticleSystem::CreateBuffers()
{
	/* Particle Index Buffer */
	{
		int numIndices = maxParticles * 6;
		unsigned int* indices = new unsigned int[numIndices];
		int indexCount = 0;
		for(uint32_t i = 0; i < maxParticles * 4; i += 4)
		{
			indices[indexCount++] = i;
			indices[indexCount++] = i + 1;
			indices[indexCount++] = i + 2;
			indices[indexCount++] = i;
			indices[indexCount++] = i + 2;
			indices[indexCount++] = i + 3;
		}
		D3D11_SUBRESOURCE_DATA indexData = {};
		indexData.pSysMem = indices;

		D3D11_BUFFER_DESC ibDesc = {};
		ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibDesc.CPUAccessFlags = 0;
		ibDesc.Usage = D3D11_USAGE_DEFAULT;
		ibDesc.ByteWidth = sizeof(unsigned int) * maxParticles * 6;

		Graphics::Device->CreateBuffer(&ibDesc, &indexData, indexBuffer.GetAddressOf());
		
		delete[] indices;
	}

	// ParticlePool structured buffer
	D3D11_BUFFER_DESC particleBufferDesc = {};
	particleBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	particleBufferDesc.ByteWidth = sizeof(Particle) * maxParticles;
	particleBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	particleBufferDesc.CPUAccessFlags = 0;
	particleBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	particleBufferDesc.StructureByteStride = sizeof(Particle);

	Graphics::Device->CreateBuffer(&particleBufferDesc, nullptr, particlePoolBuffer.GetAddressOf());

	// SRV for ParticlePool
	D3D11_SHADER_RESOURCE_VIEW_DESC particleSRVDesc = {};
	particleSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	particleSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	particleSRVDesc.Buffer.FirstElement = 0;
	particleSRVDesc.Buffer.NumElements = maxParticles;

	Graphics::Device->CreateShaderResourceView(particlePoolBuffer.Get(), &particleSRVDesc, particlePoolSRV.GetAddressOf());

	// UAV for ParticlePool
	D3D11_UNORDERED_ACCESS_VIEW_DESC particleUAVDesc = {};
	particleUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	particleUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	particleUAVDesc.Buffer.FirstElement = 0;
	particleUAVDesc.Buffer.NumElements = maxParticles;

	Graphics::Device->CreateUnorderedAccessView(particlePoolBuffer.Get(), &particleUAVDesc, particlePoolUAV.GetAddressOf());

	// DeadList structured buffer
	D3D11_BUFFER_DESC deadListBufferDesc = {};
	deadListBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	deadListBufferDesc.ByteWidth = sizeof(UINT) * maxParticles;
	deadListBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	deadListBufferDesc.CPUAccessFlags = 0;
	deadListBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	deadListBufferDesc.StructureByteStride = sizeof(UINT);

	Graphics::Device->CreateBuffer(&deadListBufferDesc, nullptr, deadListBuffer.GetAddressOf());

	// UAV for DeadList (append/consume)
	D3D11_UNORDERED_ACCESS_VIEW_DESC deadListUAVDesc = {};
	deadListUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	deadListUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	deadListUAVDesc.Buffer.FirstElement = 0;
	deadListUAVDesc.Buffer.NumElements = maxParticles;
	deadListUAVDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND; // Append/consume buffer

	Graphics::Device->CreateUnorderedAccessView(deadListBuffer.Get(), &deadListUAVDesc, deadListUAV.GetAddressOf());

	// DrawList structured buffer
	D3D11_BUFFER_DESC drawListBufferDesc = {};
	drawListBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	drawListBufferDesc.ByteWidth = sizeof(UINT) * maxParticles;
	drawListBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	drawListBufferDesc.CPUAccessFlags = 0;
	drawListBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	drawListBufferDesc.StructureByteStride = sizeof(UINT);

	Graphics::Device->CreateBuffer(&drawListBufferDesc, nullptr, drawListBuffer.GetAddressOf());

	// SRV for DrawList
	D3D11_SHADER_RESOURCE_VIEW_DESC drawListSRVDesc = {};
	drawListSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	drawListSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	drawListSRVDesc.Buffer.FirstElement = 0;
	drawListSRVDesc.Buffer.NumElements = maxParticles;

	Graphics::Device->CreateShaderResourceView(drawListBuffer.Get(), &drawListSRVDesc, drawListSRV.GetAddressOf());

	// UAV for DrawList (counter)
	D3D11_UNORDERED_ACCESS_VIEW_DESC drawListUAVDesc = {};
	drawListUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	drawListUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	drawListUAVDesc.Buffer.FirstElement = 0;
	drawListUAVDesc.Buffer.NumElements = maxParticles;
	drawListUAVDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;

	Graphics::Device->CreateUnorderedAccessView(drawListBuffer.Get(), &drawListUAVDesc, drawListUAV.GetAddressOf());
}

void ParticleSystem::Update(float deltaTime)
{
	// Reset UAVs
	ID3D11UnorderedAccessView* none[8] = {};
	Graphics::Context->CSSetUnorderedAccessViews(0, 8, none, 0);

	// Check if particle system should emit
	emissionCooldown -= deltaTime;
	if(emissionCooldown < 0)
	{
		Emit(emissionCountMin + rand() % (emissionCountMax - emissionCountMin));

		emissionCooldown = ((emissionPeriodMin * 100) + rand() % ((uint32_t) ((emissionPeriodMax - emissionPeriodMin) * 100))) / 100.0f;
	}

	// Dispatch particle update shader

	particleComputeShaderUpdate->SetShader();
	particleComputeShaderUpdate->SetUnorderedAccessView("ParticlePool", particlePoolUAV);
	particleComputeShaderUpdate->SetUnorderedAccessView("DeadList", deadListUAV);
	particleComputeShaderUpdate->SetUnorderedAccessView("DrawList", drawListUAV, 0);

	particleComputeShaderUpdate->SetInt("maxParticles", maxParticles);
	particleComputeShaderUpdate->SetFloat("deltaTime", deltaTime);

	particleComputeShaderUpdate->CopyAllBufferData();

	particleComputeShaderUpdate->DispatchByThreads(maxParticles, 1, 1);

	// Copy the hidden counter in dead list UAV into counter buffer
	Graphics::Context->CopyStructureCount(deadListCounterBuffer.Get(), 0, deadListUAV.Get());
}
void ParticleSystem::Emit(uint32_t count)
{
	// Dispatch particle emit shader

	particleComputeShaderEmit->SetShader();
	particleComputeShaderEmit->SetUnorderedAccessView("ParticlePool", particlePoolUAV);
	particleComputeShaderEmit->SetUnorderedAccessView("DeadList", deadListUAV);

	particleComputeShaderEmit->SetInt("emitCount", count);
	particleComputeShaderEmit->SetFloat4("colorTintMin", colorTintMin);
	particleComputeShaderEmit->SetFloat4("colorTintMax", colorTintMax);
	particleComputeShaderEmit->SetFloat("lifetimeMin", particleLifetimeMin);
	particleComputeShaderEmit->SetFloat("lifetimeMax", particleLifetimeMax);
	particleComputeShaderEmit->SetFloat("rotationMin", rotationMin);
	particleComputeShaderEmit->SetFloat("rotationMax", rotationMax);
	particleComputeShaderEmit->SetFloat3("locationMin", locationMin);
	particleComputeShaderEmit->SetFloat3("locationMax", locationMax);
	particleComputeShaderEmit->SetFloat3("velocityMin", velocityMin);
	particleComputeShaderEmit->SetFloat3("velocityMax", velocityMax);
	particleComputeShaderEmit->SetFloat3("accelerationMin", accelerationMin);
	particleComputeShaderEmit->SetFloat3("accelerationMax", accelerationMax);

	particleComputeShaderEmit->CopyAllBufferData();

	// Manually set dead list counter buffer
	Graphics::Context->CSSetConstantBuffers(1, 1, deadListCounterBuffer.GetAddressOf());

	particleComputeShaderEmit->DispatchByThreads(count, 1, 1);
}
void ParticleSystem::Draw(std::shared_ptr<Camera> camera)
{
	// Dispatch particle draw shader
	{
		ID3D11UnorderedAccessView* none[8] = {};
		Graphics::Context->CSSetUnorderedAccessViews(0, 8, none, 0);

		particleComputeShaderDraw->SetShader();
		particleComputeShaderDraw->SetUnorderedAccessView("DrawArgs", drawArgsUAV);
		particleComputeShaderDraw->SetUnorderedAccessView("DrawList", drawListUAV);

		particleComputeShaderDraw->SetInt("verticesPerParticle", 6);

		particleComputeShaderDraw->CopyAllBufferData();

		particleComputeShaderDraw->DispatchByThreads(1, 1, 1);

		// Reset again before the normal rendering pipeline steps
		Graphics::Context->CSSetUnorderedAccessViews(0, 8, none, 0);
	}

	// Vertex and Pixel shaders
	{
		UINT stride = 0;
		UINT offset = 0;
		ID3D11Buffer* nullBuffer = 0;
		Graphics::Context->IASetVertexBuffers(0, 1, &nullBuffer, &stride, &offset);
		Graphics::Context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		particleVertexShader->SetShader();
		particlePixelShader->SetShader();

		// Have the material set up the shader with its private values
		material->PrepareMaterial();

		particleVertexShader->SetShaderResourceView("ParticlePool", particlePoolSRV);
		particleVertexShader->SetShaderResourceView("DrawList", drawListSRV);

		// Create data to be sent to the vertex shader
		particleVertexShader->SetMatrix4x4("viewMatrix", camera->GetViewMatrix());
		particleVertexShader->SetMatrix4x4("projMatrix", camera->GetProjectionMatrix());

		particleVertexShader->CopyAllBufferData();
		particlePixelShader->CopyAllBufferData();

		// Use draw args buffer for an indirect draw
		Graphics::Context->DrawIndexedInstancedIndirect(drawArgsBuffer.Get(), 0);

		ID3D11ShaderResourceView* none[16] = {};
		Graphics::Context->VSSetShaderResources(0, 16, none);
	}
}

void ParticleSystem::SetMaxParticles(uint32_t maxParticles)
{
	this->maxParticles = maxParticles;
	
	// Recreate buffers with variable size
	//CreateBuffers();
}
void ParticleSystem::SetEmissionPeriodRange(float min, float max)
{
	emissionPeriodMin = min;
	emissionPeriodMax = max;
}
void ParticleSystem::SetEmissionCountRange(uint32_t min, uint32_t max)
{
	emissionCountMin = min;
	emissionCountMax = max;
}
void ParticleSystem::SetColorTintRange(DirectX::XMFLOAT4 min, DirectX::XMFLOAT4 max)
{
	colorTintMin = min;
	colorTintMax = max;
}
void ParticleSystem::SetParticleLifetimeRange(float min, float max)
{
	particleLifetimeMin = min;
	particleLifetimeMax = max;
}
void ParticleSystem::SetParticleRotationRange(float min, float max)
{
	rotationMin = min;
	rotationMax = max;
}
void ParticleSystem::SetParticleLocationRange(DirectX::XMFLOAT3 min, DirectX::XMFLOAT3 max)
{
	locationMin = min;
	locationMax = max;
}
void ParticleSystem::SetParticleVelocityRange(DirectX::XMFLOAT3 min, DirectX::XMFLOAT3 max)
{
	velocityMin = min;
	velocityMax = max;
}
void ParticleSystem::SetParticleAccelerationRange(DirectX::XMFLOAT3 min, DirectX::XMFLOAT3 max)
{
	accelerationMin = min;
	accelerationMax = max;
}