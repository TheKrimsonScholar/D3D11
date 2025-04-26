#include "FluidVolume.h"

#include "Graphics.h"

FluidVolume::FluidVolume()
{

}
FluidVolume::~FluidVolume()
{

}

void FluidVolume::Initialize()
{
	CreateBuffers();
}
void FluidVolume::CreateBuffers()
{
	/* Velocity */
	{
		// Previous Velocity
		D3D11_TEXTURE3D_DESC velocityPreviousTextureDesc = {};
		velocityPreviousTextureDesc.Width = width;
		velocityPreviousTextureDesc.Height = height;
		velocityPreviousTextureDesc.Depth = depth;
		velocityPreviousTextureDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		velocityPreviousTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		Graphics::Device->CreateTexture3D(&velocityPreviousTextureDesc, nullptr, velocityPreviousTexture.GetAddressOf());

		// Current Velocity
		D3D11_TEXTURE3D_DESC velocityCurrentTextureDesc = {};
		velocityCurrentTextureDesc.Width = width;
		velocityCurrentTextureDesc.Height = height;
		velocityCurrentTextureDesc.Depth = depth;
		velocityCurrentTextureDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		velocityCurrentTextureDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;

		Graphics::Device->CreateTexture3D(&velocityCurrentTextureDesc, nullptr, velocityCurrentTexture.GetAddressOf());

		// Previous Velocity SRV
		D3D11_SHADER_RESOURCE_VIEW_DESC velocityPreviousSRVDesc = {};
		velocityPreviousSRVDesc.Format = velocityPreviousTextureDesc.Format;
		velocityPreviousSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;

		Graphics::Device->CreateShaderResourceView(velocityPreviousTexture.Get(), &velocityPreviousSRVDesc, velocityPreviousSRV.GetAddressOf());

		// Current Velocity UAV
		D3D11_UNORDERED_ACCESS_VIEW_DESC velocityCurrentUAVDesc = {};
		velocityCurrentUAVDesc.Format = velocityCurrentTextureDesc.Format;
		velocityCurrentUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;

		Graphics::Device->CreateUnorderedAccessView(velocityCurrentTexture.Get(), &velocityCurrentUAVDesc, velocityCurrentUAV.GetAddressOf());
	}

	/* Pressure */
	{
		// Previous Pressure
		D3D11_TEXTURE3D_DESC pressurePreviousTextureDesc = {};
		pressurePreviousTextureDesc.Width = width;
		pressurePreviousTextureDesc.Height = height;
		pressurePreviousTextureDesc.Depth = depth;
		pressurePreviousTextureDesc.Format = DXGI_FORMAT_R32_FLOAT;
		pressurePreviousTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		Graphics::Device->CreateTexture3D(&pressurePreviousTextureDesc, nullptr, pressurePreviousTexture.GetAddressOf());

		// Current Pressure
		D3D11_TEXTURE3D_DESC pressureCurrentTextureDesc = {};
		pressureCurrentTextureDesc.Width = width;
		pressureCurrentTextureDesc.Height = height;
		pressureCurrentTextureDesc.Depth = depth;
		pressureCurrentTextureDesc.Format = DXGI_FORMAT_R32_FLOAT;
		pressureCurrentTextureDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;

		Graphics::Device->CreateTexture3D(&pressureCurrentTextureDesc, nullptr, pressureCurrentTexture.GetAddressOf());

		// Previous Pressure SRV
		D3D11_SHADER_RESOURCE_VIEW_DESC pressurePreviousSRVDesc = {};
		pressurePreviousSRVDesc.Format = pressurePreviousTextureDesc.Format;
		pressurePreviousSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;

		Graphics::Device->CreateShaderResourceView(pressurePreviousTexture.Get(), &pressurePreviousSRVDesc, pressurePreviousSRV.GetAddressOf());

		// Current Pressure UAV
		D3D11_UNORDERED_ACCESS_VIEW_DESC pressureCurrentUAVDesc = {};
		pressureCurrentUAVDesc.Format = pressureCurrentTextureDesc.Format;
		pressureCurrentUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;

		Graphics::Device->CreateUnorderedAccessView(pressureCurrentTexture.Get(), &pressureCurrentUAVDesc, pressureCurrentUAV.GetAddressOf());
	}

	/* Divergence */
	{
		// Divergence
		D3D11_TEXTURE3D_DESC divergenceTextureDesc = {};
		divergenceTextureDesc.Width = width;
		divergenceTextureDesc.Height = height;
		divergenceTextureDesc.Depth = depth;
		divergenceTextureDesc.Format = DXGI_FORMAT_R32_FLOAT;
		divergenceTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;

		Graphics::Device->CreateTexture3D(&divergenceTextureDesc, nullptr, divergenceTexture.GetAddressOf());

		// Divergence SRV
		D3D11_SHADER_RESOURCE_VIEW_DESC divergenceSRVDesc = {};
		divergenceSRVDesc.Format = divergenceTextureDesc.Format;
		divergenceSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;

		Graphics::Device->CreateShaderResourceView(divergenceTexture.Get(), &divergenceSRVDesc, divergenceSRV.GetAddressOf());

		// Divergence UAV
		D3D11_UNORDERED_ACCESS_VIEW_DESC divergenceUAVDesc = {};
		divergenceUAVDesc.Format = divergenceTextureDesc.Format;
		divergenceUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;

		Graphics::Device->CreateUnorderedAccessView(pressureCurrentTexture.Get(), &divergenceUAVDesc, divergenceUAV.GetAddressOf());
	}

	/* Density */
	{
		// Previous Density
		D3D11_TEXTURE3D_DESC densityPreviousTextureDesc = {};
		densityPreviousTextureDesc.Width = width;
		densityPreviousTextureDesc.Height = height;
		densityPreviousTextureDesc.Depth = depth;
		densityPreviousTextureDesc.Format = DXGI_FORMAT_R32_FLOAT;
		densityPreviousTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		Graphics::Device->CreateTexture3D(&densityPreviousTextureDesc, nullptr, densityPreviousTexture.GetAddressOf());

		// Current Density
		D3D11_TEXTURE3D_DESC densityCurrentTextureDesc = {};
		densityCurrentTextureDesc.Width = width;
		densityCurrentTextureDesc.Height = height;
		densityCurrentTextureDesc.Depth = depth;
		densityCurrentTextureDesc.Format = DXGI_FORMAT_R32_FLOAT;
		densityCurrentTextureDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;

		Graphics::Device->CreateTexture3D(&densityCurrentTextureDesc, nullptr, densityCurrentTexture.GetAddressOf());

		// Previous Density SRV
		D3D11_SHADER_RESOURCE_VIEW_DESC densityPreviousSRVDesc = {};
		densityPreviousSRVDesc.Format = densityPreviousTextureDesc.Format;
		densityPreviousSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;

		Graphics::Device->CreateShaderResourceView(densityPreviousTexture.Get(), &densityPreviousSRVDesc, densityPreviousSRV.GetAddressOf());

		// Current Density UAV
		D3D11_UNORDERED_ACCESS_VIEW_DESC densityCurrentUAVDesc = {};
		densityCurrentUAVDesc.Format = densityCurrentTextureDesc.Format;
		densityCurrentUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;

		Graphics::Device->CreateUnorderedAccessView(densityCurrentTexture.Get(), &densityCurrentUAVDesc, densityCurrentUAV.GetAddressOf());
	}

	/* Sampler */
	{
		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		Graphics::Device->CreateSamplerState(&samplerDesc, sampler.GetAddressOf());
	}
}

void FluidVolume::Update(float deltaTime)
{
	/* Advection */
	{
		fluidComputeShaderAdvection->SetShaderResourceView("VelocityPrevious", velocityPreviousSRV);
		fluidComputeShaderAdvection->SetShaderResourceView("PressurePrevious", pressurePreviousSRV);
		fluidComputeShaderAdvection->SetShaderResourceView("DensityPrevious", densityPreviousSRV);

		fluidComputeShaderAdvection->SetSamplerState("Sampler", sampler);

		fluidComputeShaderAdvection->SetUnorderedAccessView("VelocityCurrent", velocityCurrentUAV);
		fluidComputeShaderAdvection->SetUnorderedAccessView("PressureCurrent", pressureCurrentUAV);
		fluidComputeShaderAdvection->SetUnorderedAccessView("DensityCurrent", densityCurrentUAV);

		fluidComputeShaderAdvection->SetFloat("deltaTime", deltaTime);

		fluidComputeShaderAdvection->CopyAllBufferData();

		fluidComputeShaderAdvection->DispatchByThreads(width, height, depth);

		// Swap buffers
		std::swap(velocityPreviousTexture, velocityCurrentTexture);
		std::swap(pressurePreviousTexture, pressureCurrentTexture);
		std::swap(densityPreviousTexture, densityCurrentTexture);
	}

	/* Divergence */
	{
		fluidComputeShaderDivergence->SetShaderResourceView("VelocityPrevious", velocityPreviousSRV);

		fluidComputeShaderDivergence->SetUnorderedAccessView("Divergence", divergenceUAV);

		fluidComputeShaderDivergence->CopyAllBufferData();

		fluidComputeShaderDivergence->DispatchByThreads(width, height, depth);
	}

	/* Pressure */
	{
		fluidComputeShaderPressure->SetShaderResourceView("PressurePrevious", pressurePreviousSRV);
		fluidComputeShaderPressure->SetShaderResourceView("Divergence", divergenceSRV);

		fluidComputeShaderPressure->SetUnorderedAccessView("PressureCurrent", pressureCurrentUAV);

		fluidComputeShaderPressure->CopyAllBufferData();

		fluidComputeShaderPressure->DispatchByThreads(width, height, depth);

		// Swap buffers
		std::swap(pressurePreviousTexture, pressureCurrentTexture);
	}

	/* Projection */
	{
		fluidComputeShaderProjection->SetShaderResourceView("VelocityPrevious", velocityPreviousSRV);
		fluidComputeShaderProjection->SetShaderResourceView("PressurePrevious", pressurePreviousSRV);

		fluidComputeShaderProjection->SetUnorderedAccessView("VelocityCurrent", velocityCurrentUAV);

		fluidComputeShaderProjection->CopyAllBufferData();

		fluidComputeShaderProjection->DispatchByThreads(width, height, depth);
	}
}
void FluidVolume::Draw(std::shared_ptr<Camera> camera)
{
	/*UINT stride = 0;
	UINT offset = 0;
	ID3D11Buffer* nullBuffer = 0;
	Graphics::Context->IASetVertexBuffers(0, 1, &nullBuffer, &stride, &offset);
	Graphics::Context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);*/

	fluidVertexShader->SetShader();
	fluidPixelShader->SetShader();

	// Have the material set up the shader with its private values
	//material->PrepareMaterial();

	fluidVertexShader->SetMatrix4x4("worldMatrix", transform.GetWorldMatrix());
	fluidVertexShader->SetMatrix4x4("viewMatrix", camera->GetViewMatrix());
	fluidVertexShader->SetMatrix4x4("projMatrix", camera->GetProjectionMatrix());

	fluidVertexShader->CopyAllBufferData();
	fluidPixelShader->CopyAllBufferData();

	// Draw 36 vertices (box)
	Graphics::Context->Draw(36, 0);

	ID3D11ShaderResourceView* none[16] = {};
	Graphics::Context->VSSetShaderResources(0, 16, none);
}