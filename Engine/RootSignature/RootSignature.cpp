#include "RootSignature.h"
#include <cassert>


RootSignature::RootSignature():
	rootSignature{},
	rootParamater{},
	isTexture(false)
{}
RootSignature::RootSignature(const RootSignature& right) {
	*this = right;
}
RootSignature::RootSignature(RootSignature&& right) noexcept {
	*this = std::move(right);
}
RootSignature& RootSignature::operator=(const RootSignature& right) {
	rootSignature = right.rootSignature;
	rootParamater = right.rootParamater;
	isTexture = right.isTexture;

	return *this;
}
RootSignature& RootSignature::operator=(RootSignature&& right) noexcept {
	rootSignature = std::move(right.rootSignature);
	rootParamater = std::move(right.rootParamater);
	isTexture = std::move(right.isTexture);

	return *this;
}

bool RootSignature::operator==(const RootSignature& right) const {
	return (rootParamater.ParameterType == right.rootParamater.ParameterType
		&& (
			(
				rootParamater.DescriptorTable.NumDescriptorRanges == right.rootParamater.DescriptorTable.NumDescriptorRanges
				&& rootParamater.DescriptorTable.pDescriptorRanges == right.rootParamater.DescriptorTable.pDescriptorRanges
			)
			||
			(
				rootParamater.Constants.ShaderRegister == right.rootParamater.Constants.ShaderRegister
				&& rootParamater.Constants.RegisterSpace == right.rootParamater.Constants.RegisterSpace
				&& rootParamater.Constants.Num32BitValues == right.rootParamater.Constants.Num32BitValues
			)
			||
			(
				rootParamater.Descriptor.ShaderRegister == right.rootParamater.Descriptor.ShaderRegister
				&& rootParamater.Descriptor.RegisterSpace == right.rootParamater.Descriptor.RegisterSpace
			)
			)
		&& rootParamater.ShaderVisibility == right.rootParamater.ShaderVisibility
		)
		&& isTexture == right.isTexture;
}
bool RootSignature::operator!=(const RootSignature& right) const {
	return !(*this == right);
}

void RootSignature::Create(const D3D12_ROOT_PARAMETER& rootParamater_, bool isTexture_) {
	// RootSignatureの生成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	rootParamater = rootParamater_;
	descriptionRootSignature.pParameters = &rootParamater;
	descriptionRootSignature.NumParameters = 1;

	isTexture = isTexture_;

	D3D12_STATIC_SAMPLER_DESC staticSamplers{};
	staticSamplers.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	staticSamplers.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers.MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers.ShaderRegister = 0;
	staticSamplers.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	descriptionRootSignature.pStaticSamplers = isTexture ? &staticSamplers : nullptr;
	descriptionRootSignature.NumStaticSamplers = isTexture ? 1u : 0u;

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

bool RootSignature::IsSame(const D3D12_ROOT_PARAMETER& rootParamater_, bool isTexture_) const {
	return (rootParamater.ParameterType == rootParamater_.ParameterType
		&& (
			(
				rootParamater.DescriptorTable.NumDescriptorRanges == rootParamater_.DescriptorTable.NumDescriptorRanges
				&& rootParamater.DescriptorTable.pDescriptorRanges == rootParamater_.DescriptorTable.pDescriptorRanges
				)
			||
			(
				rootParamater.Constants.ShaderRegister == rootParamater_.Constants.ShaderRegister
				&& rootParamater.Constants.RegisterSpace == rootParamater_.Constants.RegisterSpace
				&& rootParamater.Constants.Num32BitValues == rootParamater_.Constants.Num32BitValues
				)
			||
			(
				rootParamater.Descriptor.ShaderRegister == rootParamater_.Descriptor.ShaderRegister
				&& rootParamater.Descriptor.RegisterSpace == rootParamater_.Descriptor.RegisterSpace
				)
			)
		&& rootParamater.ShaderVisibility == rootParamater_.ShaderVisibility
		)
		&& isTexture == isTexture_;
}