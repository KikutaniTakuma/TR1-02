#pragma once
#include "Engine/Engine.h"

class RootSignature {
public:
	RootSignature() = default;
	~RootSignature() = default;

	RootSignature(const RootSignature&) = delete;
	RootSignature(RootSignature&&) = delete;
	RootSignature& operator=(const RootSignature&) = delete;
	RootSignature& operator=(RootSignature&&) = delete;

public:
	void Cretate(const D3D12_ROOT_PARAMETER& rootParamator, bool isTexture = false);

	inline ID3D12RootSignature* get(){
		return rootSignature.Get();
	}

private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
};