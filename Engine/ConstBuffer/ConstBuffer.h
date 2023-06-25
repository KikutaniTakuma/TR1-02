#pragma once
#include "Engine/Engine.h"

template<class T>
class ConstBuffer {
public:
	inline ConstBuffer():
		bufferResource(nullptr),
		data(nullptr),
		isWright(true)
	{
		bufferResource = Engine::CreateBufferResuorce(sizeof(T));
		if (isWright) {
			bufferResource->Map(0, nullptr, reinterpret_cast<void**>(&data));
		}
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


private:
	ID3D12Resource* bufferResource = nullptr;

	T* data;

	bool isWright;
};