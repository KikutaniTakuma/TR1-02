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
#include <wrl.h>

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
/// 静的関数
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
	/// 失敗した場合内部でassertで止められる
	/// </summary>
	/// <param name="windowWidth">Windowの横幅</param>
	/// <param name="windowHeight">Windowの縦幅</param>
	/// <param name="windowName">Windowの名前</param>
	static void Initialize(int windowWidth, int windowHeight, const std::string& windowName);

	static void Finalize();

private:
	/// <summary>
	/// シングルトンインスタンス
	/// </summary>
	static Engine* engine;

public:
	static inline Engine* GetInstance() {
		return engine;
	}

	static ID3D12GraphicsCommandList* GetCommandList() {
		return engine->commandList.Get();
	}

	static inline ID3D12Device* GetDevice() {
		return engine->device.Get();
	}

	static inline ID3D12DescriptorHeap* GetDSVHeap() {
		return engine->dsvHeap.Get();
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
	/// Window生成用
	/// 
public:
	int32_t clientWidth = 0;
	int32_t clientHeight = 0;



#ifdef _DEBUG
	///
	/// Debug用
	/// 
private:
	class Debug {
	public:
		Debug();
		~Debug();

	public:
		void InitializeDebugLayer();

	private:
		Microsoft::WRL::ComPtr<ID3D12Debug1> debugController;
	};

	static Debug debugLayer;
#endif




	/// 
	/// Dirct3D
	/// 
private:
	void InitializeDirect3D();

private:
	Microsoft::WRL::ComPtr<ID3D12Device> device;
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory;
	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter;




	/// 
	/// DirectX12
	/// 
private:
	void InitializeDirect12();

private:
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;

	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain;
	std::vector< Microsoft::WRL::ComPtr<ID3D12Resource>> swapChianResource;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandles;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap;

	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	uint64_t fenceVal = 0;
	HANDLE fenceEvent;


	/// 
	/// 描画関係
	/// 
public:
	void InitalizeDraw();


private:
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;



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