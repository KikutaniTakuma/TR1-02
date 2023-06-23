#include "Engine.h"
#include <cassert>
#include <format>
#include <filesystem>
#include "WinApp/WinApp.h"
#include "ShaderManager/ShaderManager.h"
#include "ConvertString/ConvertString.h"

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wPram, LPARAM lPram);


/// 
/// 各種初期化処理
/// 

Engine* Engine::engine = nullptr;

void Engine::Initalize(int windowWidth, int windowHeight, const std::string& windowName) {
	CoInitializeEx(0, COINIT_MULTITHREADED);

	engine = new Engine();
	assert(engine);

	engine->clientWidth = windowWidth;
	engine->clientHeight = windowHeight;

	WinApp::Initalize();

	// Window生成
	WinApp::GetInstance()->Create(ConvertString(windowName), windowWidth, windowHeight);

	ShaderManager::Initalize();

#ifdef _DEBUG
	// DebugLayer有効化
	engine->InitalizeDebugLayer();
#endif

	engine->pera = std::make_unique<PeraRender>();

	// Direct3D生成
	assert(engine->InitalizeDirect3D());

	// DirectX12生成
	engine->InitalizeDirect12();

	engine->InitalizeDraw();
}

void Engine::Finalize() {
	ShaderManager::Finalize();

	delete engine;
	engine = nullptr;

	WinApp::Finalize();

	// COM 終了
	CoUninitialize();
}



/// 
/// Window生成
/// 

// windowプロシージャ
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	assert(SUCCEEDED(CoInitializeEx(0, COINIT_MULTITHREADED)));

	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}

	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}




///
/// デバッグレイヤー初期化
/// 
#ifdef _DEBUG
void Engine::InitalizeDebugLayer() {
	debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		// デバッグレイヤーを有効化する
		debugController->EnableDebugLayer();
		// さらにGPU側でもチェックするようにする
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
}
#endif





///
/// Direct3D初期化
/// 

bool Engine::InitalizeDirect3D() {
	// IDXGIFactory生成
	auto hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	assert(SUCCEEDED(hr));
	if (hr != S_OK) {
		return false;
	}

	// 使用するグラボの設定
	useAdapter = nullptr;
	for (UINT i = 0;
		dxgiFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) != DXGI_ERROR_NOT_FOUND;
		++i) {

		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter->GetDesc3(&adapterDesc);
		if (hr != S_OK) {
			return false;
		}

		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			Log(ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
			break;
		}
		useAdapter = nullptr;
	}
	if (useAdapter == nullptr) {
		return false;
	}


	// Deviceの初期化
	// 使用しているデバイスによってD3D_FEATURE_LEVELの対応バージョンが違うので成功するまでバージョンを変えて繰り返す
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
	};
	const char* featureLevelString[] = {
		"12.2", "12.1", "12.0"
	};

	for (size_t i = 0; i < _countof(featureLevels); ++i) {
		hr = D3D12CreateDevice(useAdapter, featureLevels[i], IID_PPV_ARGS(&device));

		if (SUCCEEDED(hr)) {
			Log(std::format("FeatureLevel:{}\n", featureLevelString[i]));
			break;
		}
	}

	if (device == nullptr) {
		return false;
	}
	Log("Complete create D3D12Device!!!\n");

#ifdef _DEBUG
	ID3D12InfoQueue* infoQueue = nullptr;
	if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		// やばいエラーの予期に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		// エラーの時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		// 警告時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		// 抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		// 抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		// 指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);

		// 解放
		infoQueue->Release();
	}
#endif

	return true;
}




/// 
/// DirectX12
/// 

ID3D12DescriptorHeap* Engine::CreateDescriptorHeap(
	D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderrVisible
) {
	ID3D12DescriptorHeap* descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderrVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	assert(SUCCEEDED(engine->device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap))));
	return descriptorHeap;
}

bool Engine::InitalizeDirect12() {
	// コマンドキューを作成
	commandQueue = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	HRESULT hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
	assert(SUCCEEDED(hr));

	// コマンドアロケータを生成する
	commandAllocator = nullptr;
	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
	assert(SUCCEEDED(hr));

	// コマンドリストを作成する
	commandList = nullptr;
	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr, IID_PPV_ARGS(&commandList));
	assert(SUCCEEDED(hr));


	// スワップチェーンの作成
	swapChain = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = clientWidth;
	swapChainDesc.Height = clientHeight;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue, WinApp::GetInstance()->GetHwnd(), &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(&swapChain));
	assert(SUCCEEDED(hr));

	dxgiFactory->MakeWindowAssociation(
		WinApp::GetInstance()->GetHwnd(), DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER);


	// デスクリプタヒープの作成
	rtvDescriptorHeap = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);

	// SRV用のヒープ
	srvDescriptorHeap = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);

	// SwepChainのメモリとディスクリプタと関連付け
	// バックバッファの数を取得
	DXGI_SWAP_CHAIN_DESC backBufferNum{};
	hr = swapChain->GetDesc(&backBufferNum);
	// SwapChainResource初期化
	swapChianResource.reserve(backBufferNum.BufferCount);
	swapChianResource.resize(backBufferNum.BufferCount);

	// RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	// ディスクリプタの先頭を取得
	rtvHandles.reserve(backBufferNum.BufferCount);
	rtvHandles.resize(backBufferNum.BufferCount);
	auto rtvStartHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	for (UINT i = 0; i < backBufferNum.BufferCount; ++i) {
		hr = swapChain->GetBuffer(i, IID_PPV_ARGS(&swapChianResource[i]));
		assert(SUCCEEDED(hr));
		rtvHandles[i].ptr = rtvStartHandle.ptr + (i * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
		device->CreateRenderTargetView(swapChianResource[i], &rtvDesc, rtvHandles[i]);
	}

	// ImGuiの初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(WinApp::GetInstance()->GetHwnd());
	ImGui_ImplDX12_Init(
		device,
		swapChainDesc.BufferCount,
		rtvDesc.Format,
		srvDescriptorHeap,
		srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart()
	);


	// 初期値0でFenceを作る
	fence = nullptr;
	fenceVal = 0;
	hr = device->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	assert(SUCCEEDED(hr));

	// FenceのSignalを持つためのイベントを作成する
	fenceEvent = nullptr;
	fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent != nullptr);


	return true;
}






///
/// 描画用
/// 

struct PeraVertexData {
	Vector3D position;
	Vector2D uv;
};

ID3D12Resource* Engine::CreateBufferResuorce(size_t sizeInBytes) {
	if (!engine->device) {
		OutputDebugStringA("device is nullptr!!");
		return nullptr;
	}

	// Resourceを生成する
	// リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapPropaerties{};
	uploadHeapPropaerties.Type = D3D12_HEAP_TYPE_UPLOAD;
	// リソースの設定
	D3D12_RESOURCE_DESC resouceDesc{};
	// バッファリソース。テクスチャの場合はまた別の設定にする
	resouceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resouceDesc.Width = sizeInBytes;
	// バッファの場合はこれにする決まり
	resouceDesc.Height = 1;
	resouceDesc.DepthOrArraySize = 1;
	resouceDesc.MipLevels = 1;
	resouceDesc.SampleDesc.Count = 1;
	// バッファの場合はこれにする決まり
	resouceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	// 実際にリソースを作る
	ID3D12Resource* resuorce = nullptr;
	HRESULT hr = engine->device->CreateCommittedResource(&uploadHeapPropaerties, D3D12_HEAP_FLAG_NONE, &resouceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resuorce));
	if (!SUCCEEDED(hr)) {
		OutputDebugStringA("CreateCommittedResource Function Failed!!");
		return nullptr;
	}

	return resuorce;
}

ID3D12Resource* Engine::CreateDepthStencilTextureResource(int32_t width, int32_t height) {
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;
	resourceDesc.Height = height;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Format = DXGI_FORMAT_D32_FLOAT;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;

	ID3D12Resource* resource = nullptr;
	assert(SUCCEEDED(
		engine->device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(&resource))
		)
	);

	return resource;
}

void Engine::InitalizeDraw() {
	// DepthStencilTextureをウィンドウサイズで作成
	depthStencilResource = CreateDepthStencilTextureResource(clientWidth, clientHeight);
	assert(depthStencilResource);

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

	dsvHeap = nullptr;
	assert(SUCCEEDED(device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap))));


	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	device->CreateDepthStencilView(depthStencilResource, &dsvDesc, dsvHeap->GetCPUDescriptorHandleForHeapStart());


	CreatePera();

	pera->CreateShader("PostShader/Post.VS.hlsl", "PostShader/PostNone.PS.hlsl");
	
	pera->CreateGraphicsPipeline();
}

void  Engine::CreatePera() {
	pera->CreateDescriptor();
}

Vector4 Engine::UintToVector4(uint32_t color) {
	float normal = 1.0f / 255.0f;
	return Vector4(static_cast<float>((color & 0xff000000) >> 24) * normal, static_cast<float>((color & 0xff0000) >> 16) * normal, static_cast<float>((color & 0xff00) >> 8) * normal, static_cast<float>(color & 0xff) * normal);
}

void Engine::Barrier(ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after) {
	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier{};
	// 今回のバリアはTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	// Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	// バリアを張る対象のリソース
	barrier.Transition.pResource = resource;
	// 遷移前(現在)のResouceState
	barrier.Transition.StateBefore = before;
	// 遷移後のResouceState
	barrier.Transition.StateAfter = after;
	// TransitionBarrierを張る
	engine->commandList->ResourceBarrier(1, &barrier);
}







/// 
/// MianLoop用
/// 

bool Engine::WindowMassage() {
	if (PeekMessage(&engine->msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&engine->msg);
		DispatchMessage(&engine->msg);
	}

	return engine->msg.message != WM_QUIT;
}

void Engine::FrameStart() {
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = engine->swapChain->GetCurrentBackBufferIndex();

	Barrier(
		engine->swapChianResource[backBufferIndex], 
		D3D12_RESOURCE_STATE_PRESENT, 
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	// 描画先をRTVを設定する
	auto dsvH = engine->dsvHeap->GetCPUDescriptorHandleForHeapStart();
	engine->commandList->OMSetRenderTargets(1, &engine->rtvHandles[backBufferIndex], false, &dsvH);
	engine->commandList->ClearDepthStencilView(engine->dsvHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// 指定した色で画面全体をクリアする
	float clearColor[] = { 0.4f, 0.7f, 1.0f, 1.0f };
	engine->commandList->ClearRenderTargetView(engine->rtvHandles[backBufferIndex], clearColor, 0, nullptr);


	// ビューポート
	D3D12_VIEWPORT viewport{};
	// クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = static_cast<float>(engine->clientWidth);
	viewport.Height = static_cast<float>(engine->clientHeight);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	engine->commandList->RSSetViewports(1, &viewport);


	// シザー矩形
	D3D12_RECT scissorRect{};
	// 基本的にビューポートと同じ矩形が構成されるようになる
	scissorRect.left = 0;
	scissorRect.right = engine->clientWidth;
	scissorRect.top = 0;
	scissorRect.bottom = engine->clientHeight;
	engine->commandList->RSSetScissorRects(1, &scissorRect);

	engine->pera->PreDraw();
}

void Engine::FrameEnd() {
	// これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = engine->swapChain->GetCurrentBackBufferIndex();
	// 描画先をRTVを設定する
	auto dsvH = engine->dsvHeap->GetCPUDescriptorHandleForHeapStart();
	engine->commandList->OMSetRenderTargets(1, &engine->rtvHandles[backBufferIndex], false, &dsvH);

	engine->pera->Draw();

	ID3D12DescriptorHeap* descriptorHeaps[] = {
		engine->srvDescriptorHeap
	};
	engine->commandList->SetDescriptorHeaps(1, descriptorHeaps);

	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), engine->commandList);


	Barrier(
		engine->swapChianResource[backBufferIndex],
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);

	// コマンドリストを確定させる
	HRESULT hr = engine->commandList->Close();
	assert(SUCCEEDED(hr));

	// GPUにコマンドリストの実行を行わせる
	ID3D12CommandList* commandLists[] = { engine->commandList };
	engine->commandQueue->ExecuteCommandLists(1, commandLists);


	// GPUとOSに画面の交換を行うように通知する
	engine->swapChain->Present(1, 0);

	// Fenceの値を更新
	engine->fenceVal++;
	// GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
	engine->commandQueue->Signal(engine->fence, engine->fenceVal);

	// Fenceの値が指定したSigna値にたどり着いているか確認する
	// GetCompletedValueの初期値はFence作成時に渡した初期値
	if (engine->fence->GetCompletedValue() < engine->fenceVal) {
		// 指定したSignal値にたどり着いていないので、たどり着くまで待つようにイベントを設定する
		engine->fence->SetEventOnCompletion(engine->fenceVal, engine->fenceEvent);
		// イベントを待つ
		WaitForSingleObject(engine->fenceEvent, INFINITE);
	}

	// 次フレーム用のコマンドリストを準備
	hr = engine->commandAllocator->Reset();
	assert(SUCCEEDED(hr));
	hr = engine->commandList->Reset(engine->commandAllocator, nullptr);
	assert(SUCCEEDED(hr));
}





/// 
/// 各種解放処理
/// 
Engine::~Engine() {
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	pera.reset();

	dsvHeap->Release();
	depthStencilResource->Release();
	CloseHandle(fenceEvent);
	fence->Release();
	srvDescriptorHeap->Release();
	rtvDescriptorHeap->Release();
	for (auto i = swapChianResource.rbegin(); i != swapChianResource.rend(); i++) {
		(*i)->Release();
	}
	swapChain->Release();
	commandList->Release();
	commandAllocator->Release();
	commandQueue->Release();
	device->Release();
	useAdapter->Release();
	dxgiFactory->Release();
#ifdef _DEBUG
	debugController->Release();
#endif


	// リソースリークチェック
	IDXGIDebug1* debug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		debug->Release();
	}
}