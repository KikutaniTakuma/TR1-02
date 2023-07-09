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
	void Use();

	template<class T>
	void CreateConstBufferView(ConstBuffer<T>& conBuf) {
		conBuf.CrerateView(srvHeapHandle);
		srvHeapHandle.ptr += Engine::GetIncrementSRVCBVUAVHeap();
		heapOrder.push_back(HeapType::CBV);
	}

	void CreateTxtureView(std::shared_ptr<Texture> tex) {
		tex->CreateSRVView(srvHeapHandle);
		srvHeapHandle.ptr += Engine::GetIncrementSRVCBVUAVHeap();
		heapOrder.push_back(HeapType::SRV);
	}

	D3D12_ROOT_PARAMETER GetParameter();

private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> SRVHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE srvHeapHandle;

	std::vector<HeapType> heapOrder;

	std::vector<D3D12_DESCRIPTOR_RANGE> descriptorRanges;
};