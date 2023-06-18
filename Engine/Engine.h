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
#include <unordered_map>

#include "Math/Vector3D/Vector3D.h"
#include "Math/Mat4x4/Mat4x4.h"
#include "Math/Vector2D/Vector2D.h"
#include "Math/Vector4/Vector4.h" 
#include "RenderTarget/RenderTarget.h"

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
	static IDxcBlob* CompilerShader(
		// Compiler����Shader�t�@�C���ւ̃p�X
		const std::wstring& filePath,
		// Compiler�Ɏg�p����Profile
		const wchar_t* profile
	);

	static std::wstring ConvertString(const std::string& msg);
	static std::string ConvertString(const std::wstring& msg);

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
	static void Initalize(int windowWidth, int windowHeight, const std::string& windowName);

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

	static ID3D12Device* GetDevice() {
		return engine->device;
	}

	static ID3D12DescriptorHeap* GetDSVHeap() {
		return engine->dsvHeap;
	}

	static D3D12_DESCRIPTOR_HEAP_DESC GetMainRTVDesc() {
		return engine->rtvDescriptorHeap->GetDesc();
	}

	static D3D12_RESOURCE_DESC GetSwapchainBufferDesc() {
		return engine->swapChianResource[0]->GetDesc();
	}

	static D3D12_CPU_DESCRIPTOR_HANDLE GetDsvHandle() {
		return engine->dsvHeap->GetCPUDescriptorHandleForHeapStart();
	}


	///
	/// Window�����p
	/// 
private:
	bool InitalizeWindow(const std::wstring& windowName);

private:
	WNDCLASSEX w{};
	HWND hwnd{};

public:
	int32_t clientWidth = 0;
	int32_t clientHeight = 0;



#ifdef _DEBUG
	///
	/// Debug�p
	/// 
private:
	void InitalizeDebugLayer();

private:
	ID3D12Debug1* debugController = nullptr;
#endif




	/// 
	/// Dirct3D
	/// 
private:
	bool InitalizeDirect3D();

private:
	ID3D12Device* device = nullptr;
	IDXGIFactory7* dxgiFactory = nullptr;
	IDXGIAdapter4* useAdapter = nullptr;




	/// 
	/// DirectX12
	/// 
private:
	bool InitalizeDirect12();

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

	IDxcUtils* dxcUtils = nullptr;
	IDxcCompiler3* dxcCompiler = nullptr;

	IDxcIncludeHandler* includeHandler = nullptr;




	/// 
	/// �`��֌W
	/// 
public:
	void InitalizeDraw();

	void LoadShader();


private:
	ID3D12Resource* depthStencilResource = nullptr;
	ID3D12DescriptorHeap* dsvHeap = nullptr;


	std::unordered_map<std::string, IDxcBlob*> vertexShaders;
	std::unordered_map<std::string, IDxcBlob*> pixelShaders;
	


	// �|�X�g�G�t�F�N�g�p
	void CreatePera();

	PeraRender pera;



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