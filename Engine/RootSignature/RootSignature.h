#pragma once
#include "Engine/Engine.h"

class RootSignature {
public:
	RootSignature();
	~RootSignature() = default;

	RootSignature(const RootSignature& right);
	RootSignature(RootSignature&& right) noexcept;

public:
	RootSignature& operator=(const RootSignature& right);
	RootSignature& operator=(RootSignature&& right) noexcept;

	bool operator==(const RootSignature& right) const;
	bool operator!=(const RootSignature& right) const;

public:
	void Create(D3D12_ROOT_PARAMETER* rootParamater_, size_t rootParamaterSize_, bool isTexture_);

	inline ID3D12RootSignature* Get() const {
		return rootSignature.Get();
	}

	bool IsSame(D3D12_ROOT_PARAMETER* rootParamater_, size_t rootParamaterSize_, bool isTexture_) const;

private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
	D3D12_ROOT_PARAMETER* rootParamater;
	size_t rootParamaterSize;
	bool isTexture;
};

bool operator==(const D3D12_ROOT_PARAMETER& left, const D3D12_ROOT_PARAMETER& right);
bool operator!=(const D3D12_ROOT_PARAMETER& left, const D3D12_ROOT_PARAMETER& right);