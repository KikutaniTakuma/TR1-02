#pragma once
#include "Engine/Engine.h"

// ポインタをテンプレートパラメータに設定してはいけない
template<class T>
class ConstBuffer {
public:
	inline ConstBuffer():
		bufferResource(nullptr),
		data(nullptr),
		isWright(true),
		roootParamater(),
		shaderVisibility(D3D12_SHADER_VISIBILITY_ALL),
		shaderRegister(0)
	{
		bufferResource = Engine::CreateBufferResuorce(sizeof(T));
		if (isWright) {
			bufferResource->Map(0, nullptr, reinterpret_cast<void**>(&data));
		}
		roootParamater.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;

	}

	inline ~ConstBuffer() {
		bufferResource->Release();
	}

public:
	void OnWright() {
		if (!isWright) {
			bufferResource->Map(0, nullptr, reinterpret_cast<void**>(&data));
			isWright = !isWright;
		}
	}

	void OffWright() {
		if (isWright) {
			bufferResource->Unmap(0, nullptr);
			isWright = !isWright;
		}
	}

	T& operator*() const {
		return *data;
	}

	T* operator->() const {
		return data;
	}

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVtlAdrs() const {
		return bufferResource->GetGPUVirtualAddress();
	}

	const D3D12_ROOT_PARAMETER& getRoootParamater() {
		roootParamater.ShaderVisibility = shaderVisibility;
		roootParamater.Descriptor.ShaderRegister = shaderRegister;
		return roootParamater;
	}

private:
	ID3D12Resource* bufferResource = nullptr;

	T* data;

	bool isWright;

	D3D12_ROOT_PARAMETER roootParamater;
public:
	D3D12_SHADER_VISIBILITY shaderVisibility;
	UINT shaderRegister;
};