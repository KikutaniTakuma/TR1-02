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

class Engine {
public:
	Engine() = default;
	~Engine();


	/// <summary>
	/// 失敗した場合内部でassertで止められる
	/// </summary>
	/// <param name="windowWidth">Windowの横幅</param>
	/// <param name="windowHeight">Windowの縦幅</param>
	/// <param name="windowName">Windowの名前</param>
	void Initalize(int windowWidth, int windowHeight, const std::string& windowName);


	///
	/// Window生成用
	/// 
private:
	bool InitalizeWindow(int windowWidth, int windowHeight, const std::wstring& windowName);

private:
	WNDCLASSEX w{};
	HWND hwnd{};

	int clientWidth = 0;
	int clientHeight = 0;



#ifdef _DEBUG
	///
	/// Debug用
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

	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	ID3D12RootSignature* rootSignature = nullptr;




	/// 
	/// 描画関係
	/// 
public:
	void InitalizeDraw();

	void LoadShader();
	void LoadObj(const std::string& fileName);

	/// <summary>
	/// 読み込んだobjファイルを元に描画していく
	/// </summary>
	void Draw();

	void DrawTriangle(const Vector3D& pos, const Vector3D& size, uint32_t color);

private:
	// これらは後で増やす
	ID3D12Resource* wvpResource = nullptr;
	Mat4x4* wvpData = nullptr;

	ID3D12Resource* vertexResuorce = nullptr;
	ID3D12Resource* indexBufferResuorce = nullptr;

	ID3D12Resource* depthStencilResource = nullptr;
	ID3D12DescriptorHeap* dsvHeap = nullptr;

	ID3D12PipelineState* graphicsPipelineState = nullptr;


	std::unordered_map<std::string, IDxcBlob*> vertexShaders;
	std::unordered_map<std::string, IDxcBlob*> pixelShaders;







	///
	/// MainLoop
	/// 
public:
	bool WindowMassage();

	void FrameStart();

	void FrameEnd();

private:
	MSG msg{};
};