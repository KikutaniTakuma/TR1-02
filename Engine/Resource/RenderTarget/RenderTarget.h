#pragma once
#include <wrl.h>
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#include "Engine/ConstBuffer/ConstBuffer.h"

class RenderTarget {
public:
	// �f�B�X�N���v�^�̑傫���̓f�t�H���g�R���X�g���N�^��4
	RenderTarget();
	RenderTarget(uint16_t numDescriptor);
	~RenderTarget();

public:
	// �����_�[�^�[�Q�b�g�ɐݒ肷��
	void SetThisTarget();

	// ���C�������_�[�^�[�Q�b�g�ɕύX(RenderTarget::SetThisTarget()���g������K���Ă΂Ȃ���΂Ȃ�Ȃ�)
	void SetMainRenderTarget();

	// �����_�[�^�[�Q�b�g�ɐݒ肵��Resource��ShaderResource�Ƃ��Ďg��
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