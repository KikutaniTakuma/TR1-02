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
	void Create(const D3D12_ROOT_PARAMETER& rootParamater_, bool isTexture_ = false);

	inline ID3D12RootSignature* Get() const {
		return rootSignature.Get();
	}

	bool IsSame(const D3D12_ROOT_PARAMETER& rootParamater_, bool isTexture_) const;

private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
	D3D12_ROOT_PARAMETER rootParamater;
	bool isTexture;
};