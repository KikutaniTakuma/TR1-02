#pragma once
#include <wrl.h>
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#include "Engine/ConstBuffer/ConstBuffer.h"

class RenderTarget {
public:
	// ディスクリプタの大きさはデフォルトコンストラクタで4
	RenderTarget();
	RenderTarget(uint16_t numDescriptor);
	~RenderTarget();

public:
	// レンダーターゲットに設定する
	void SetThisRenderTarget();

	// メインレンダーターゲットに変更(RenderTarget::SetThisTarget()を使ったら必ず呼ばなければならない)
	void SetMainRenderTarget();

	// レンダーターゲットに設定したResourceをShaderResourceとして使う
	void UseThisRenderTargetShaderResource();

	template<class T>
	void CreateConstBufferView(ConstBuffer<T>& conBuf) {
		conBuf.CrerateView(srvHeapHandle);
		srvHeapHandle.ptr += Engine::GetIncrementSRVCBVUAVHeap();
	}

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> RTVHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> SRVHeap;

	D3D12_CPU_DESCRIPTOR_HANDLE srvHeapHandle;
};