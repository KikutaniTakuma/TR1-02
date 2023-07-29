#pragma once
#include <wrl.h>
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#include "Engine/ConstBuffer/ConstBuffer.h"
#include "TextureManager/Texture/Texture.h"
#include <memory>
#include <vector>

class ShaderResourceHeap {
private:
	enum class HeapType {
		SRV,
		CBV,
		UAV
	};

public:
	ShaderResourceHeap();
	ShaderResourceHeap(const ShaderResourceHeap& right);
	ShaderResourceHeap(uint16_t numDescriptor);
	~ShaderResourceHeap();
public:
	ShaderResourceHeap& operator=(const ShaderResourceHeap& right);

public:
	void InitializeReset();

	void Use();
	void Use(D3D12_GPU_DESCRIPTOR_HANDLE handle);

	template<class T>
	D3D12_CPU_DESCRIPTOR_HANDLE CreateConstBufferView(ConstBuffer<T>& conBuf) {
		auto result = srvCpuHeapHandle;
		conBuf.CrerateView(srvCpuHeapHandle);
		srvCpuHeapHandle.ptr += Engine::GetIncrementSRVCBVUAVHeap();
		srvGpuHeapHandle.ptr += Engine::GetIncrementSRVCBVUAVHeap();
		
		heapOrder.push_back(HeapType::CBV);
		return result;
	}

	inline D3D12_GPU_DESCRIPTOR_HANDLE CreateTxtureViewGPUh(Texture* tex) {
		auto result = srvGpuHeapHandle;

		assert(tex != nullptr);
		tex->CreateSRVView(srvCpuHeapHandle);
		srvCpuHeapHandle.ptr += Engine::GetIncrementSRVCBVUAVHeap();
		srvGpuHeapHandle.ptr += Engine::GetIncrementSRVCBVUAVHeap();
		
		heapOrder.push_back(HeapType::SRV);
		
		return result;
	}
	inline D3D12_CPU_DESCRIPTOR_HANDLE CreateTxtureViewCPUh(Texture* tex) {
		auto result = srvCpuHeapHandle;

		assert(tex != nullptr);
		tex->CreateSRVView(srvCpuHeapHandle);
		srvCpuHeapHandle.ptr += Engine::GetIncrementSRVCBVUAVHeap();
		srvGpuHeapHandle.ptr += Engine::GetIncrementSRVCBVUAVHeap();
		
		heapOrder.push_back(HeapType::SRV);

		return result;
	}
	inline void CreateTxtureView(Texture* tex, D3D12_CPU_DESCRIPTOR_HANDLE handle) {
		assert(tex != nullptr);
		tex->CreateSRVView(handle);
	}

	D3D12_ROOT_PARAMETER GetParameter();

	D3D12_CPU_DESCRIPTOR_HANDLE GetSrvCpuHeapHandle(){
		return srvCpuHeapHandle;
	}
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvGpuHeapHandle(){
		return srvGpuHeapHandle;
	}

private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> SRVHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE srvCpuHeapHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHeapHandle;

	std::vector<HeapType> heapOrder;

	std::vector<D3D12_DESCRIPTOR_RANGE> descriptorRanges;
};