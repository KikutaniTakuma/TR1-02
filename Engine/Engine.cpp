#include "Engine.h"
#include <cassert>
#include <format>
#include <filesystem>
#include "WinApp/WinApp.h"
#include "ShaderManager/ShaderManager.h"
#include "ConvertString/ConvertString.h"
#include "TextureManager/TextureManager.h"

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wPram, LPARAM lPram);


#ifdef _DEBUG
Engine::Debug Engine::debugLayer;

Engine::Debug::Debug() :
	debugController(nullptr)
{}

Engine::Debug::~Debug() {
	debugController.Reset();

	// ���\�[�X���[�N�`�F�b�N
	IDXGIDebug1* debug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		debug->Release();
	}
}

///
/// �f�o�b�O���C���[������
/// 

void Engine::Debug::InitializeDebugLayer() {
	debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf())))) {
		// �f�o�b�O���C���[��L��������
		debugController->EnableDebugLayer();
		// �����GPU���ł��`�F�b�N����悤�ɂ���
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
}

#endif // _DEBUG

/// 
/// �e�평��������
/// 

Engine* Engine::engine = nullptr;

void Engine::Initialize(int windowWidth, int windowHeight, const std::string& windowName) {
	HRESULT hr =  CoInitializeEx(0, COINIT_MULTITHREADED);
	if (hr != S_OK) {
		return;
	}

	engine = new Engine();
	assert(engine);

	ShaderManager::Initialize();
	TextureManager::Initialize();

	engine->clientWidth = windowWidth;
	engine->clientHeight = windowHeight;

	// Window����
	WinApp::GetInstance()->Create(ConvertString(windowName), windowWidth, windowHeight);

#ifdef _DEBUG
	// DebugLayer�L����
	debugLayer.InitializeDebugLayer();
#endif

	// Direct3D����
	engine->InitializeDirect3D();

	// DirectX12����
	engine->InitializeDirect12();

	engine->InitalizeDraw();
}

void Engine::Finalize() {
	TextureManager::Finalize();
	ShaderManager::Finalize();

	delete engine;
	engine = nullptr;

	// COM �I��
	CoUninitialize();
}



/// 
/// Window����
/// 

// window�v���V�[�W��
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
/// Direct3D������
/// 

void Engine::InitializeDirect3D() {
	// IDXGIFactory����
	auto hr = CreateDXGIFactory(IID_PPV_ARGS(dxgiFactory.GetAddressOf()));
	assert(SUCCEEDED(hr));
	if (hr != S_OK) {
		return;
	}

	// �g�p����O���{�̐ݒ�
	useAdapter = nullptr;
	for (UINT i = 0;
		dxgiFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(useAdapter.GetAddressOf())) != DXGI_ERROR_NOT_FOUND;
		++i) {

		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter->GetDesc3(&adapterDesc);
		if (hr != S_OK) {
			return;
		}

		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			Log(ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
			break;
		}
		useAdapter.Reset();
	}
	if (useAdapter == nullptr) {
		return;
	}


	// Device�̏�����
	// �g�p���Ă���f�o�C�X�ɂ����D3D_FEATURE_LEVEL�̑Ή��o�[�W�������Ⴄ�̂Ő�������܂Ńo�[�W������ς��ČJ��Ԃ�
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
	};
	const char* featureLevelString[] = {
		"12.2", "12.1", "12.0"
	};

	for (size_t i = 0; i < _countof(featureLevels); ++i) {
		hr = D3D12CreateDevice(useAdapter.Get(), featureLevels[i], IID_PPV_ARGS(device.GetAddressOf()));

		if (SUCCEEDED(hr)) {
			Log(std::format("FeatureLevel:{}\n", featureLevelString[i]));
			break;
		}
	}

	if (device == nullptr) {
		return;
	}
	Log("Complete create D3D12Device!!!\n");

#ifdef _DEBUG
	ID3D12InfoQueue* infoQueue = nullptr;
	if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		// ��΂��G���[�̗\���Ɏ~�܂�
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		// �G���[�̎��Ɏ~�܂�
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		// �x�����Ɏ~�܂�
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		// �}�����郁�b�Z�[�W��ID
		D3D12_MESSAGE_ID denyIds[] = {
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		// �}�����郌�x��
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		// �w�肵�����b�Z�[�W�̕\����}������
		infoQueue->PushStorageFilter(&filter);

		// ���
		infoQueue->Release();
	}
#endif
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
	if (SUCCEEDED(engine->device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap)))) {
		return descriptorHeap;
	}
	assert(!"Failed");

	return nullptr;
}

void Engine::InitializeDirect12() {
	// �R�}���h�L���[���쐬
	commandQueue = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	HRESULT hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(commandQueue.GetAddressOf()));
	assert(SUCCEEDED(hr));

	// �R�}���h�A���P�[�^�𐶐�����
	commandAllocator = nullptr;
	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator.GetAddressOf()));
	assert(SUCCEEDED(hr));

	// �R�}���h���X�g���쐬����
	commandList = nullptr;
	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(commandList.GetAddressOf()));
	assert(SUCCEEDED(hr));


	// �X���b�v�`�F�[���̍쐬
	swapChain = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = clientWidth;
	swapChainDesc.Height = clientHeight;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue.Get(), WinApp::GetInstance()->GetHwnd(), &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain.GetAddressOf()));
	assert(SUCCEEDED(hr));

	dxgiFactory->MakeWindowAssociation(
		WinApp::GetInstance()->GetHwnd(), DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER);


	// �f�X�N���v�^�q�[�v�̍쐬
	rtvDescriptorHeap = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);

	// SRV�p�̃q�[�v
	srvDescriptorHeap = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1, true);

	// SwepChain�̃������ƃf�B�X�N���v�^�Ɗ֘A�t��
	// �o�b�N�o�b�t�@�̐����擾
	DXGI_SWAP_CHAIN_DESC backBufferNum{};
	hr = swapChain->GetDesc(&backBufferNum);
	// SwapChainResource������
	swapChianResource.reserve(backBufferNum.BufferCount);
	swapChianResource.resize(backBufferNum.BufferCount);

	// RTV�̐ݒ�
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	// �f�B�X�N���v�^�̐擪���擾
	rtvHandles.reserve(backBufferNum.BufferCount);
	rtvHandles.resize(backBufferNum.BufferCount);
	auto rtvStartHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	for (UINT i = 0; i < backBufferNum.BufferCount; ++i) {
		hr = swapChain->GetBuffer(i, IID_PPV_ARGS(swapChianResource[i].GetAddressOf()));
		assert(SUCCEEDED(hr));
		rtvHandles[i].ptr = rtvStartHandle.ptr + (i * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
		device->CreateRenderTargetView(swapChianResource[i].Get(), &rtvDesc, rtvHandles[i]);
	}

	// ImGui�̏�����
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(WinApp::GetInstance()->GetHwnd());
	ImGui_ImplDX12_Init(
		device.Get(),
		swapChainDesc.BufferCount,
		rtvDesc.Format,
		srvDescriptorHeap.Get(),
		srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart()
	);


	// �����l0��Fence�����
	fence = nullptr;
	fenceVal = 0;
	hr = device->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf()));
	assert(SUCCEEDED(hr));

	// Fence��Signal�������߂̃C�x���g���쐬����
	fenceEvent = nullptr;
	fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent != nullptr);
}






///
/// �`��p
/// 

ID3D12Resource* Engine::CreateBufferResuorce(size_t sizeInBytes) {
	if (!engine->device) {
		OutputDebugStringA("device is nullptr!!");
		return nullptr;
	}

	// Resource�𐶐�����
	// ���\�[�X�p�̃q�[�v�̐ݒ�
	D3D12_HEAP_PROPERTIES uploadHeapPropaerties{};
	uploadHeapPropaerties.Type = D3D12_HEAP_TYPE_UPLOAD;
	// ���\�[�X�̐ݒ�
	D3D12_RESOURCE_DESC resouceDesc{};
	// �o�b�t�@���\�[�X�B�e�N�X�`���̏ꍇ�͂܂��ʂ̐ݒ�ɂ���
	resouceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resouceDesc.Width = sizeInBytes;
	// �o�b�t�@�̏ꍇ�͂���ɂ��錈�܂�
	resouceDesc.Height = 1;
	resouceDesc.DepthOrArraySize = 1;
	resouceDesc.MipLevels = 1;
	resouceDesc.SampleDesc.Count = 1;
	// �o�b�t�@�̏ꍇ�͂���ɂ��錈�܂�
	resouceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	// ���ۂɃ��\�[�X�����
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
	if (!SUCCEEDED(
		engine->device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&depthClearValue,
			IID_PPV_ARGS(&resource))
	)) {
		assert(!"CreateDepthStencilTextureResource Failed");
	}

	return resource;
}

void Engine::InitalizeDraw() {
	// DepthStencilTexture���E�B���h�E�T�C�Y�ō쐬
	depthStencilResource = CreateDepthStencilTextureResource(clientWidth, clientHeight);
	assert(depthStencilResource);

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

	dsvHeap = nullptr;
	if(!SUCCEEDED(device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(dsvHeap.GetAddressOf())))) {
		assert(!"CreateDescriptorHeap failed");
	}


	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	device->CreateDepthStencilView(depthStencilResource.Get(), &dsvDesc, dsvHeap->GetCPUDescriptorHandleForHeapStart());
}

Vector4 Engine::UintToVector4(uint32_t color) {
	static const float normal = 1.0f / 255.0f;
	return Vector4(static_cast<float>((color & 0xff000000) >> 24) * normal, static_cast<float>((color & 0xff0000) >> 16) * normal, static_cast<float>((color & 0xff00) >> 8) * normal, static_cast<float>(color & 0xff) * normal);
}

void Engine::Barrier(ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after) {
	// TransitionBarrier�̐ݒ�
	D3D12_RESOURCE_BARRIER barrier{};
	// ����̃o���A��Transition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	// None�ɂ��Ă���
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	// �o���A�𒣂�Ώۂ̃��\�[�X
	barrier.Transition.pResource = resource;
	// �J�ڑO(����)��ResouceState
	barrier.Transition.StateBefore = before;
	// �J�ڌ��ResouceState
	barrier.Transition.StateAfter = after;
	// TransitionBarrier�𒣂�
	engine->commandList->ResourceBarrier(1, &barrier);
}







/// 
/// MianLoop�p
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

	// ���ꂩ�珑�����ރo�b�N�o�b�t�@�̃C���f�b�N�X���擾
	UINT backBufferIndex = engine->swapChain->GetCurrentBackBufferIndex();

	Barrier(
		engine->swapChianResource[backBufferIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT, 
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	// �`����RTV��ݒ肷��
	auto dsvH = engine->dsvHeap->GetCPUDescriptorHandleForHeapStart();
	engine->commandList->OMSetRenderTargets(1, &engine->rtvHandles[backBufferIndex], false, &dsvH);
	engine->commandList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// �w�肵���F�ŉ�ʑS�̂��N���A����
	float clearColor[] = { 0.1f, 0.25f, 0.5f, 0.0f };
	engine->commandList->ClearRenderTargetView(engine->rtvHandles[backBufferIndex], clearColor, 0, nullptr);


	// �r���[�|�[�g
	D3D12_VIEWPORT viewport{};
	// �N���C�A���g�̈�̃T�C�Y�ƈꏏ�ɂ��ĉ�ʑS�̂ɕ\��
	viewport.Width = static_cast<float>(engine->clientWidth);
	viewport.Height = static_cast<float>(engine->clientHeight);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	engine->commandList->RSSetViewports(1, &viewport);


	// �V�U�[��`
	D3D12_RECT scissorRect{};
	// ��{�I�Ƀr���[�|�[�g�Ɠ�����`���\�������悤�ɂȂ�
	scissorRect.left = 0;
	scissorRect.right = engine->clientWidth;
	scissorRect.top = 0;
	scissorRect.bottom = engine->clientHeight;
	engine->commandList->RSSetScissorRects(1, &scissorRect);
}

void Engine::FrameEnd() {
	// �`����RTV��ݒ肷��
	UINT backBufferIndex = engine->swapChain->GetCurrentBackBufferIndex();
	auto dsvH = engine->dsvHeap->GetCPUDescriptorHandleForHeapStart();
	engine->commandList->OMSetRenderTargets(1, &engine->rtvHandles[backBufferIndex], false, &dsvH);

	engine->commandList->SetDescriptorHeaps(1, engine->srvDescriptorHeap.GetAddressOf());

	// ImGui�`��
	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), engine->commandList.Get());

	Barrier(
		engine->swapChianResource[backBufferIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);

	// �R�}���h���X�g���m�肳����
	HRESULT hr = engine->commandList->Close();
	assert(SUCCEEDED(hr));

	// GPU�ɃR�}���h���X�g�̎��s���s�킹��
	ID3D12CommandList* commandLists[] = { engine->commandList.Get()};
	engine->commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);


	// GPU��OS�ɉ�ʂ̌������s���悤�ɒʒm����
	engine->swapChain->Present(1, 0);

	// Fence�̒l���X�V
	engine->fenceVal++;
	// GPU�������܂ł��ǂ蒅�����Ƃ��ɁAFence�̒l���w�肵���l�ɑ������悤��Signal�𑗂�
	engine->commandQueue->Signal(engine->fence.Get(), engine->fenceVal);

	// Fence�̒l���w�肵��Signa�l�ɂ��ǂ蒅���Ă��邩�m�F����
	// GetCompletedValue�̏����l��Fence�쐬���ɓn���������l
	if (engine->fence->GetCompletedValue() < engine->fenceVal) {
		// �w�肵��Signal�l�ɂ��ǂ蒅���Ă��Ȃ��̂ŁA���ǂ蒅���܂ő҂悤�ɃC�x���g��ݒ肷��
		engine->fence->SetEventOnCompletion(engine->fenceVal, engine->fenceEvent);
		// �C�x���g��҂�
		WaitForSingleObject(engine->fenceEvent, INFINITE);
	}

	// ���t���[���p�̃R�}���h���X�g������
	hr = engine->commandAllocator->Reset();
	assert(SUCCEEDED(hr));
	hr = engine->commandList->Reset(engine->commandAllocator.Get(), nullptr);
	assert(SUCCEEDED(hr));

	// ���̃t���[���ŉ摜�ǂݍ��݂��������Ă�����J������
	// �܂�Unload����Ă����炻����R���e�i����폜����
	TextureManager::GetInstance()->ReleaseIntermediateResource();
}





/// 
/// �e��������
/// 
Engine::~Engine() {
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	dsvHeap->Release();
	depthStencilResource->Release();
	CloseHandle(fenceEvent);
	srvDescriptorHeap->Release();
	rtvDescriptorHeap->Release();
}