#pragma once
#include "Engine/Engine.h"
#include <wrl.h>

// ポインタをテンプレートパラメータに設定してはいけない
template<class T>
class ConstBuffer {
public:
	inline ConstBuffer():
		bufferResource(nullptr),
		cbvDesc(),
		data(nullptr),
		isWright(true),
		roootParamater(),
		shaderVisibility(D3D12_SHADER_VISIBILITY_ALL),
		shaderRegister(0)
	{
		// バイトサイズは256アライメントする(vramを効率的に使うための仕組み)
		bufferResource = Engine::CreateBufferResuorce((sizeof(T) + 0xff) & ~0xff);
		cbvDesc.BufferLocation = bufferResource->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = UINT(bufferResource->GetDesc().Width);

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

	const D3D12_ROOT_PARAMETER& GetRoootParamater() {
		roootParamater.ShaderVisibility = shaderVisibility;
		roootParamater.Descriptor.ShaderRegister = shaderRegister;
		return roootParamater;
	}

	void CrerateView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle) {
		Engine::GetDevice()->CreateConstantBufferView(&cbvDesc, descriptorHandle);
	}

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> bufferResource = nullptr;
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;

	T* data;

	bool isWright;

	D3D12_ROOT_PARAMETER roootParamater;
public:
	D3D12_SHADER_VISIBILITY shaderVisibility;
	UINT shaderRegister;
};