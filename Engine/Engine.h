#pragma once
#include <Windows.h>
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#include <dxgi1_6.h>
#pragma comment(lib, "dxgi.lib")
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#include <dxcapi.h>
#pragma comment(lib, "dxcompiler.lib")

#include <string>
#include <vector>
#include <chrono>

#include "Math/Vector3D/Vector3D.h"
#include "Math/Mat4x4/Mat4x4.h"
#include "Math/Vector2D/Vector2D.h"
#include "Math/Vector4/Vector4.h" 

class Engine {
public:
/// <summary>
/// �ÓI�֐�
/// </summary>
	static ID3D12DescriptorHeap* CreateDescriptorHeap(
		D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderrVisible
	);

	static ID3D12Resource* CreateBufferResuorce(size_t sizeInBytes);

	static ID3D12Resource* CreateDepthStencilTextureResource(int32_t width, int32_t height);

	static Vector4 UintToVector4(uint32_t color);

	static void Barrier(ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);

private:
	Engine() = default;
	~Engine();

public:
	/// <summary>
	/// ���s�����ꍇ������assert�Ŏ~�߂���
	/// </summary>
	/// <param name="windowWidth">Window�̉���</param>
	/// <param name="windowHeight">Window�̏c��</param>
	/// <param name="windowName">Window�̖��O</param>
	static void Initialize(int windowWidth, int windowHeight, const std::string& windowName);

	static void Finalize();

private:
	/// <summary>
	/// �V���O���g���C���X�^���X
	/// </summary>
	static Engine* engine;

public:
	static inline Engine* GetInstance() {
		return engine;
	}

	static ID3D12GraphicsCommandList* GetCommandList() {
		return engine->commandList;
	}

	static inline ID3D12Device* GetDevice() {
		return engine->device;
	}

	static inline ID3D12DescriptorHeap* GetDSVHeap() {
		return engine->dsvHeap;
	}

	static inline D3D12_DESCRIPTOR_HEAP_DESC GetMainRTVDesc() {
		return engine->rtvDescriptorHeap->GetDesc();
	}

	static inline D3D12_RESOURCE_DESC GetSwapchainBufferDesc() {
		return engine->swapChianResource[0]->GetDesc();
	}

	static inline D3D12_CPU_DESCRIPTOR_HANDLE GetDsvHandle() {
		return engine->dsvHeap->GetCPUDescriptorHandleForHeapStart();
	}

	static inline D3D12_CPU_DESCRIPTOR_HANDLE GetMainRendertTargetHandle() {
		UINT backBufferIndex = engine->swapChain->GetCurrentBackBufferIndex();
		return engine->rtvHandles[backBufferIndex];
	}


	///
	/// Window�����p
	/// 
public:
	int32_t clientWidth = 0;
	int32_t clientHeight = 0;



#ifdef _DEBUG
	///
	/// Debug�p
	/// 
private:
	void InitializeDebugLayer();

private:
	ID3D12Debug1* debugController = nullptr;
#endif




	/// 
	/// Dirct3D
	/// 
private:
	void InitializeDirect3D();

private:
	ID3D12Device* device = nullptr;
	IDXGIFactory7* dxgiFactory = nullptr;
	IDXGIAdapter4* useAdapter = nullptr;




	/// 
	/// DirectX12
	/// 
private:
	void InitializeDirect12();

private:
	ID3D12CommandQueue* commandQueue = nullptr;
	ID3D12CommandAllocator* commandAllocator = nullptr;
	ID3D12GraphicsCommandList* commandList = nullptr;

	IDXGISwapChain4* swapChain = nullptr;
	std::vector<ID3D12Resource*> swapChianResource;

	ID3D12DescriptorHeap* rtvDescriptorHeap = nullptr;
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandles;

	ID3D12DescriptorHeap* srvDescriptorHeap = nullptr;

	ID3D12Fence* fence = nullptr;
	uint64_t fenceVal = 0;
	HANDLE fenceEvent = nullptr;


	/// 
	/// �`��֌W
	/// 
public:
	void InitalizeDraw();


private:
	ID3D12Resource* depthStencilResource = nullptr;
	ID3D12DescriptorHeap* dsvHeap = nullptr;



	///
	/// MainLoop
	/// 
public:
	static bool WindowMassage();

	static void FrameStart();

	static void FrameEnd();

private:
	MSG msg{};
};