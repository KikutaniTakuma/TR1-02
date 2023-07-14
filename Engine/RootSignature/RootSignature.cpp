#include "RootSignature.h"
#include <cassert>

void RootSignature::Cretate(const D3D12_ROOT_PARAMETER& rootParamator, bool isTexture) {
	// RootSignatureの生成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_ROOT_PARAMETER roootParamater = rootParamator;
	descriptionRootSignature.pParameters = &roootParamater;
	descriptionRootSignature.NumParameters = 1;

	if (isTexture) {
		// sampler
		D3D12_STATIC_SAMPLER_DESC staticSamplers{};
		staticSamplers.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		staticSamplers.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		staticSamplers.MaxLOD = D3D12_FLOAT32_MAX;
		staticSamplers.ShaderRegister = 0;
		staticSamplers.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		descriptionRootSignature.pStaticSamplers = &staticSamplers;
		descriptionRootSignature.NumStaticSamplers = 1;
	}

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
	HRESULT  hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, signatureBlob.GetAddressOf(), errorBlob.GetAddressOf());
	if (FAILED(hr)) {
		OutputDebugStringA(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	// バイナリをもとに生成
	if (rootSignature) {
		rootSignature.Reset();
	}
	hr = Engine::GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(rootSignature.GetAddressOf()));
	assert(SUCCEEDED(hr));
	if (errorBlob) { errorBlob.Reset(); }
	signatureBlob.Reset();
}