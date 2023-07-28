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
	ShaderResourceHeap(uint16_t numDescriptor);
	~ShaderResourceHeap();
public:
	ShaderResourceHeap& operator=(const ShaderResourceHeap& right);

public:
	void Use();

	template<class T>
	D3D12_CPU_DESCRIPTOR_HANDLE CreateConstBufferView(ConstBuffer<T>& conBuf) {
		auto result = srvHeapHandle;
		conBuf.CrerateView(srvHeapHandle);
		srvHeapHandle.ptr += Engine::GetIncrementSRVCBVUAVHeap();
		heapOrder.push_back(HeapType::CBV);
		return result;
	}

	inline D3D12_CPU_DESCRIPTOR_HANDLE CreateTxtureView(Texture* tex) {
		auto result = srvHeapHandle;
		assert(tex != nullptr);
		tex->CreateSRVView(srvHeapHandle);
		srvHeapHandle.ptr += Engine::GetIncrementSRVCBVUAVHeap();
		heapOrder.push_back(HeapType::SRV);
		return result;
	}
	inline void CreateTxtureView(Texture* tex, D3D12_CPU_DESCRIPTOR_HANDLE handle) {
		assert(tex != nullptr);
		tex->CreateSRVView(handle);
	}

	D3D12_ROOT_PARAMETER GetParameter();

private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> SRVHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE srvHeapHandle;

	std::vector<HeapType> heapOrder;

	std::vector<D3D12_DESCRIPTOR_RANGE> descriptorRanges;
};