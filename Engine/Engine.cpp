#include "Engine.h"
#include <cassert>
#include <format>
#include <filesystem>

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wPram, LPARAM lPram);

//�f�o�b�O�p
void Log(const std::string& meg) {
	OutputDebugStringA(meg.c_str());
}


#pragma region String type change Function
/// 
/// string to wstring
/// 
std::wstring ConvertString(const std::string& msg) {
	if (msg.empty()) {
		return std::wstring();
	}

	auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&msg[0]), static_cast<int>(msg.size()), NULL, 0);
	if (sizeNeeded == 0) {
		return std::wstring();
	}
	std::wstring result(sizeNeeded, 0);
	MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&msg[0]), static_cast<int>(msg.size()), &result[0], sizeNeeded);
	return result;
}

/// 
/// wstring to string
/// 
std::string ConvertString(const std::wstring& msg) {
	if (msg.empty()) {
		return std::string();
	}

	auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, msg.data(), static_cast<int>(msg.size()), NULL, 0, NULL, NULL);
	if (sizeNeeded == 0) {
		return std::string();
	}
	std::string result(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, msg.data(), static_cast<int>(msg.size()), result.data(), sizeNeeded, NULL, NULL);
	return result;
}
#pragma endregion


/// 
/// �e�평��������
/// 

void Engine::Initalize(int windowWidth, int windowHeight, const std::string& windowName) {
	// Window����
	assert(this->InitalizeWindow(windowWidth, windowHeight, ConvertString(windowName)));

#ifdef _DEBUG
	// DebugLayer�L����
	InitalizeDebugLayer();
#endif

	// Direct3D����
	assert(this->InitalizeDirect3D());

	// DirectX12����
	this->InitalizeDirect12();

	this->InitalizeDraw();
}



/// 
/// Window����
/// 

// window�v���V�[�W��
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	CoInitializeEx(0, COINIT_MULTITHREADED);

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

bool Engine::InitalizeWindow(int windowWidth, int windowHeight, const std::wstring& windowName) {
	clientWidth = windowWidth;
	clientHeight = windowHeight;

	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = WindowProcedure;
	w.lpszClassName = windowName.c_str();
	w.hInstance = GetModuleHandle(nullptr);

	RegisterClassEx(&w);

	RECT wrc = { 0,0,windowWidth, windowHeight };

	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	hwnd = CreateWindow(
		w.lpszClassName,
		windowName.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wrc.right - wrc.left,
		wrc.bottom - wrc.top,
		nullptr,
		nullptr,
		w.hInstance,
		nullptr
	);

	// Window�\��
	return static_cast<bool>(!ShowWindow(hwnd, SW_SHOW));
}




///
/// �f�o�b�O���C���[������
/// 
#ifdef _DEBUG
void Engine::InitalizeDebugLayer() {
	debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		// �f�o�b�O���C���[��L��������
		debugController->EnableDebugLayer();
		// �����GPU���ł��`�F�b�N����悤�ɂ���
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
}
#endif





///
/// Direct3D������
/// 

bool Engine::InitalizeDirect3D() {
	// IDXGIFactory����
	auto hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	assert(SUCCEEDED(hr));
	if (hr != S_OK) {
		return false;
	}

	// �g�p����O���{�̐ݒ�
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

	return true;
}




/// 
/// DirectX12
/// 

ID3D12DescriptorHeap* CreateDescriptorHeap(
	ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderrVisible
) {
	ID3D12DescriptorHeap* descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderrVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	assert(SUCCEEDED(hr));
	return descriptorHeap;
}

bool Engine::InitalizeDirect12() {
	// �R�}���h�L���[���쐬
	commandQueue = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	HRESULT hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
	assert(SUCCEEDED(hr));

	// �R�}���h�A���P�[�^�𐶐�����
	commandAllocator = nullptr;
	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
	assert(SUCCEEDED(hr));

	// �R�}���h���X�g���쐬����
	commandList = nullptr;
	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr, IID_PPV_ARGS(&commandList));
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

	hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue, hwnd, &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(&swapChain));
	assert(SUCCEEDED(hr));


	// �f�X�N���v�^�q�[�v�̍쐬
	rtvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);

	// SRV�p�̃q�[�v
	srvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);

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
		hr = swapChain->GetBuffer(i, IID_PPV_ARGS(&swapChianResource[i]));
		assert(SUCCEEDED(hr));
		rtvHandles[i].ptr = rtvStartHandle.ptr + (i * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
		device->CreateRenderTargetView(swapChianResource[i], &rtvDesc, rtvHandles[i]);
	}

	// ImGui�̏�����
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX12_Init(
		device,
		swapChainDesc.BufferCount,
		rtvDesc.Format,
		srvDescriptorHeap,
		srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart()
	);


	// �����l0��Fence�����
	fence = nullptr;
	fenceVal = 0;
	hr = device->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	assert(SUCCEEDED(hr));

	// Fence��Signal�������߂̃C�x���g���쐬����
	fenceEvent = nullptr;
	fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent != nullptr);
	

	// dxcCompiler��������
	dxcUtils = nullptr;
	dxcCompiler = nullptr;
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(hr));

	includeHandler = nullptr;
	hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
	assert(SUCCEEDED(hr));


	// RootSignature�̐���
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParamater�쐬�B�����ݒ�o����̂Ŕz��
	D3D12_ROOT_PARAMETER roootParamaters[2] = {};
	roootParamaters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	roootParamaters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	roootParamaters[0].Descriptor.ShaderRegister = 0;
	roootParamaters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	roootParamaters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	roootParamaters[1].Descriptor.ShaderRegister = 0;
	descriptionRootSignature.pParameters = roootParamaters;
	descriptionRootSignature.NumParameters = _countof(roootParamaters);

	// �V���A���C�Y���ăo�C�i���ɂ���
	signatureBlob = nullptr;
	errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	// �o�C�i�������Ƃɐ���
	rootSignature = nullptr;
	hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));

	return true;
}




///
/// �`��p
/// 

struct VertexData {
	Vector4 position;
	Vector3D color;
};

ID3D12Resource* CreateBufferResuorce(ID3D12Device* device, size_t sizeInBytes) {
	if (!device) {
		OutputDebugStringA("device is nullptr!!");
		return nullptr;
	}

	// VertexResource�𐶐�����
	// ���_���\�[�X�p�̃q�[�v�̐ݒ�
	D3D12_HEAP_PROPERTIES uploadHeapPropaerties{};
	uploadHeapPropaerties.Type = D3D12_HEAP_TYPE_UPLOAD;
	// ���_���\�[�X�̐ݒ�
	D3D12_RESOURCE_DESC vertexResouceDesc{};
	// �o�b�t�@���\�[�X�B�e�N�X�`���̏ꍇ�͂܂��ʂ̐ݒ�ɂ���
	vertexResouceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResouceDesc.Width = sizeInBytes;
	// �o�b�t�@�̏ꍇ�͂���ɂ��錈�܂�
	vertexResouceDesc.Height = 1;
	vertexResouceDesc.DepthOrArraySize = 1;
	vertexResouceDesc.MipLevels = 1;
	vertexResouceDesc.SampleDesc.Count = 1;
	// �o�b�t�@�̏ꍇ�͂���ɂ��錈�܂�
	vertexResouceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	// ���ۂɒ��_���\�[�X�����
	ID3D12Resource* vertexResuorce = nullptr;
	HRESULT hr = device->CreateCommittedResource(&uploadHeapPropaerties, D3D12_HEAP_FLAG_NONE, &vertexResouceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexResuorce));
	if (!SUCCEEDED(hr)) {
		OutputDebugStringA("CreateCommittedResource Function Failed!!");
		return nullptr;
	}

	return vertexResuorce;
}

ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height) {
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
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));

	return resource;
}

void Engine::InitalizeDraw() {
	// WVP�p�̃��\�[�X�����
	wvpResource = CreateBufferResuorce(device, sizeof(Mat4x4));
	// �f�[�^����������
	wvpData = nullptr;
	// �������ނ��߂̃A�h���X���擾
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	// �P�ʍs�����������ł���
	*wvpData = MakeMatrixIndentity();

	// ���ۂɒ��_���\�[�X�����
	vertexResuorce = CreateBufferResuorce(device, sizeof(VertexData) * 6);
	assert(vertexResuorce);

	// DepthStencilTexture���E�B���h�E�T�C�Y�ō쐬
	depthStencilResource = CreateDepthStencilTextureResource(device, clientWidth, clientHeight);
	assert(depthStencilResource);

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

	dsvHeap = nullptr;
	HRESULT hr = device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));
	assert(SUCCEEDED(hr));


	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	device->CreateDepthStencilView(depthStencilResource, &dsvDesc, dsvHeap->GetCPUDescriptorHandleForHeapStart());
}


IDxcBlob* CompilerShader(
	// Compiler����Shader�t�@�C���ւ̃p�X
	const std::wstring& filePath,
	// Compiler�Ɏg�p����Profile
	const wchar_t* profile,
	// �������Ő����������̂�3��
	IDxcUtils* dxcUtils,
	IDxcCompiler3* dxcCompiler,
	IDxcIncludeHandler* includeHandler)
{
	// 1. hlsl�t�@�C����ǂ�
	// ���ꂩ��V�F�[�_�[���R���p�C������|�����O�ɏo��
	Log(ConvertString(std::format(L"Begin CompilerShader, path:{}, profile:{}\n", filePath, profile)));
	// hlsl�t�@�C����ǂ�
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	// �ǂ߂Ȃ�������~�߂�
	assert(SUCCEEDED(hr));
	// �ǂݍ��񂾃t�@�C���̓��e��ݒ肷��
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;


	// 2. Compile����
	LPCWSTR arguments[] = {
		filePath.c_str(), // �R���p�C���Ώۂ�hlsl�t�@�C����
		L"-E", L"main", // �G���g���[�|�C���g�̎w��B��{�I��main�ȊO�ɂ͂��Ȃ�
		L"-T", profile, // ShaderProfile�̐ݒ�
		L"-Zi", L"-Qembed_debug", // �f�o�b�O�p�̏��𖄂ߍ���
		L"-Od", // �œK�����O���Ă���
		L"-Zpr" // ���������C�A�E�g��D��
	};
	// ���ۂ�Shader���R���p�C������
	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler->Compile(
		&shaderSourceBuffer, // �ǂ݂��񂾃t�@�C��
		arguments,           // �R���p�C���I�v�V����
		_countof(arguments), // �R���p�C���I�v�V�����̐�
		includeHandler,      // include���܂܂ꂽ���X
		IID_PPV_ARGS(&shaderResult) // �R���p�C������
	);
	// �R���p�C���G���[�ł͂Ȃ�dxc���N���ł��Ȃ��Ȃǒv���I�ȏ�
	assert(SUCCEEDED(hr));

	// 3. �x���E�G���[���o�ĂȂ����m�F����
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Log(shaderError->GetStringPointer());
		// �x���E�G���[�_���[�b�^�C
		assert(false);
	}

	// 4. Compile���󂯎���ĕԂ�
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	// �����������O���o��
	Log(ConvertString(std::format(L"Compile Succeeded, path:{}, profile:{}\n", filePath, profile)));
	// �����g��Ȃ����\�[�X�����
	shaderSource->Release();
	shaderResult->Release();
	// ���s�p�o�C�i�������^�[��
	return shaderBlob;
}


void Engine::LoadShader() {
	std::vector<std::filesystem::path> vsShaderFilePath(0);
	std::vector<std::filesystem::path> psShaderFilePath(0);
	for (auto& path : std::filesystem::directory_iterator("Shaders")) {
		if (path.path().filename().string().find(".VS.hlsl") != std::string::npos) {
			vsShaderFilePath.push_back(path.path());
		}
		else if (path.path().filename().string().find(".PS.hlsl") != std::string::npos) {
			psShaderFilePath.push_back(path.path());
		}
	}

	// shader���R���p�C������
	for (auto& vsFileName : vsShaderFilePath) {
		IDxcBlob* vertexShaderBlob = CompilerShader(vsFileName, L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
		assert(vertexShaderBlob != nullptr);
		vertexShaders.insert(std::make_pair<std::string, IDxcBlob*>(vsFileName.filename().generic_string(), std::move(vertexShaderBlob)));
	}

	for (auto& psFileName : psShaderFilePath) {
		IDxcBlob* pixelShaderBlob = CompilerShader(psFileName, L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
		assert(pixelShaderBlob != nullptr);
		pixelShaders.insert(std::make_pair<std::string, IDxcBlob*>(psFileName.filename().generic_string(), std::move(pixelShaderBlob)));
	}


	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "COLOR";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	// BlendState�̐ݒ�
	D3D12_BLEND_DESC blendDec{};
	// �S�Ă̐F�v�f����������
	blendDec.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasterizerState�̐ݒ�
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// ����(���v���)��\�����Ȃ�
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// �O�p�`�̒���h��Ԃ�
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// pso����
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature;
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = {
		vertexShaders["Object3D.VS.hlsl"]->GetBufferPointer(),
		vertexShaders["Object3D.VS.hlsl"]->GetBufferSize()
	};
	graphicsPipelineStateDesc.PS = {
		pixelShaders["Object3D.PS.hlsl"]->GetBufferPointer(),
		pixelShaders["Object3D.PS.hlsl"]->GetBufferSize()
	};
	graphicsPipelineStateDesc.BlendState = blendDec;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
	// ��������RTV�̏��
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// ���p����g�|���W(�`��)�̃^�C�v
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// �ǂ̂悤�ɉ�ʂɑł����ނ��̐ݒ�
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// 
	graphicsPipelineStateDesc.DepthStencilState.DepthEnable = true;
	graphicsPipelineStateDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	graphicsPipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;


	// ���ۂɐ���
	graphicsPipelineState = nullptr;
	HRESULT hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr));
}

void Engine::DrawTriangle(const Vector3D& pos, const Vector3D& size, const Vector3D& color) {
	// vertexbufferView���쐬����
	// ���_�o�b�t�@�r���[���쐬����
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	// ���\�[�X�̐擪�̃A�h���X����g��
	vertexBufferView.BufferLocation = vertexResuorce->GetGPUVirtualAddress();
	// ���p���郊�\�[�X�̃T�C�Y�͒��_3���̃T�C�Y
	vertexBufferView.SizeInBytes = sizeof(VertexData) * 6;
	// 1���_������̃T�C�Y
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	// ���_���\�[�X�Ƀf�[�^����������
	VertexData* vertexData = nullptr;
	// �������ނ��߂̃A�h���X���擾
	vertexResuorce->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	// ����
	vertexData[0].position = { -0.5f, -0.5f, 0.0f, 1.0f };
	vertexData[0].color = color;
	// ��
	vertexData[1].position = { 0.0f, 0.5f,  0.0f, 1.0f };
	vertexData[1].color = Vector3D(1.0f,0.0f,0.0f);
	// �E��
	vertexData[2].position = { 0.5f, -0.5f, 0.0f, 1.0f };
	vertexData[2].color = Vector3D(0.0f, 0.0f, 1.0f);

	// �񖇖�
// ����
	vertexData[3].position = { -0.5f, -0.5f, 0.5f, 1.0f };
	vertexData[3].color = Vector3D(1.0f, 0.0f, 0.0f);
	// ��
	vertexData[4].position = { 0.0f, 0.0f,  0.0f, 1.0f };
	vertexData[4].color = Vector3D(0.0f, 1.0f, 0.0f);
	// �E��
	vertexData[5].position = { 0.5f, -0.5f, -0.5f, 1.0f };
	vertexData[5].color = Vector3D(0.0f, 0.0f, 1.0f);

	Mat4x4 cameraMatrix;
	Mat4x4 viewMatrix;
	Mat4x4 projectionMatrix;
	Vector3D cameraPos{};
	cameraPos.z = -5.0f;

	cameraMatrix = MakeMatrixAffin({ 1.0f,1.0f,1.0f }, Vector3D(), cameraPos);
	viewMatrix = MakeMatrixInverse(cameraMatrix);
	projectionMatrix = MakeMatrixPerspectiveFov(0.45f, static_cast<float>(clientWidth) / static_cast<float>(clientHeight), 0.1f, 100.0f);
	*wvpData = MakeMatrixAffin(size, Vector3D(), pos) * viewMatrix * projectionMatrix;


	// �p�C�v���C���X�e�[�g�̐ݒ�
	commandList->SetPipelineState(graphicsPipelineState);
	// ���[�g�V�O�l�`���̐ݒ�
	commandList->SetGraphicsRootSignature(rootSignature);
	// �v���~�e�B�u�`���ݒ�
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// ���_�o�b�t�@�̐ݒ�
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	// CBV���Z�b�g�i���[���h�s��j
	commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
	// �`��R�}���h
	commandList->DrawInstanced(6, 1, 0, 0);
}








/// 
/// MianLoop�p
/// 

bool Engine::WindowMassage() {
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.message != WM_QUIT;
}

void Engine::FrameStart() {
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// ���ꂩ�珑�����ރo�b�N�o�b�t�@�̃C���f�b�N�X���擾
	UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();
	// TransitionBarrier�̐ݒ�
	D3D12_RESOURCE_BARRIER barrier{};
	// ����̃o���A��Transition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	// None�ɂ��Ă���
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	// �o���A�𒣂�Ώۂ̃��\�[�X�B���݂̃o�b�N�o�b�t�@�ɑ΂��čs��
	barrier.Transition.pResource = swapChianResource[backBufferIndex];
	// �J�ڑO(����)��ResouceState
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	// �J�ڌ��ResouceState
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	// TransitionBarrier�𒣂�
	commandList->ResourceBarrier(1, &barrier);

	// �`����RTV��ݒ肷��
	auto dsvH = dsvHeap->GetCPUDescriptorHandleForHeapStart();
	commandList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, &dsvH);
	// �w�肵���F�ŉ�ʑS�̂��N���A����
	float clearColor[] = { 0.1f, 0.25f, 0.5f, 1.0f };
	commandList->ClearRenderTargetView(rtvHandles[backBufferIndex], clearColor, 0, nullptr);


	ID3D12DescriptorHeap* descriptorHeaps[] = {
		srvDescriptorHeap
	};
	commandList->SetDescriptorHeaps(1, descriptorHeaps);


	// �r���[�|�[�g
	D3D12_VIEWPORT viewport{};
	// �N���C�A���g�̈�̃T�C�Y�ƈꏏ�ɂ��ĉ�ʑS�̂ɕ\��
	viewport.Width = static_cast<float>(clientWidth);
	viewport.Height = static_cast<float>(clientHeight);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	commandList->RSSetViewports(1, &viewport);


	// �V�U�[��`
	D3D12_RECT scissorRect{};
	// ��{�I�Ƀr���[�|�[�g�Ɠ�����`���\�������悤�ɂȂ�
	scissorRect.left = 0;
	scissorRect.right = clientWidth;
	scissorRect.top = 0;
	scissorRect.bottom = clientHeight;
	commandList->RSSetScissorRects(1, &scissorRect);

}

void Engine::FrameEnd() {
	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);


	// ���ꂩ�珑�����ރo�b�N�o�b�t�@�̃C���f�b�N�X���擾
	UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();
	// TransitionBarrier�̐ݒ�
	D3D12_RESOURCE_BARRIER barrier{};
	// ����̃o���A��Transition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	// None�ɂ��Ă���
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	// �o���A�𒣂�Ώۂ̃��\�[�X�B���݂̃o�b�N�o�b�t�@�ɑ΂��čs��
	barrier.Transition.pResource = swapChianResource[backBufferIndex];
	// �J�ڑO(����)��ResouceState
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	// �J�ڌ��ResouceState
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	// TransitionBarrier�𒣂�
	commandList->ResourceBarrier(1, &barrier);

	// �R�}���h���X�g���m�肳����
	HRESULT hr = commandList->Close();
	assert(SUCCEEDED(hr));

	// GPU�ɃR�}���h���X�g�̎��s���s�킹��
	ID3D12CommandList* commandLists[] = { commandList };
	commandQueue->ExecuteCommandLists(1, commandLists);


	// GPU��OS�ɉ�ʂ̌������s���悤�ɒʒm����
	swapChain->Present(1, 0);

	// Fence�̒l���X�V
	fenceVal++;
	// GPU�������܂ł��ǂ蒅�����Ƃ��ɁAFence�̒l���w�肵���l�ɑ������悤��Signal�𑗂�
	commandQueue->Signal(fence, fenceVal);

	// Fence�̒l���w�肵��Signa�l�ɂ��ǂ蒅���Ă��邩�m�F����
	// GetCompletedValue�̏����l��Fence�쐬���ɓn���������l
	if (fence->GetCompletedValue() < fenceVal) {
		// �w�肵��Signal�l�ɂ��ǂ蒅���Ă��Ȃ��̂ŁA���ǂ蒅���܂ő҂悤�ɃC�x���g��ݒ肷��
		fence->SetEventOnCompletion(fenceVal, fenceEvent);
		// �C�x���g��҂�
		WaitForSingleObject(fenceEvent, INFINITE);
	}

	// ���t���[���p�̃R�}���h���X�g������
	hr = commandAllocator->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList->Reset(commandAllocator, nullptr);
	assert(SUCCEEDED(hr));
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
	vertexResuorce->Release();
	wvpResource->Release();
	graphicsPipelineState->Release();
	signatureBlob->Release();
	if (errorBlob) {
		errorBlob->Release();
	}
	rootSignature->Release();
	for (auto& i : pixelShaders) {
		i.second->Release();
	}
	for (auto& i : vertexShaders) {
		i.second->Release();
	}
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
	CloseWindow(hwnd);


	// ���\�[�X���[�N�`�F�b�N
	IDXGIDebug1* debug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		debug->Release();
	}


	CoUninitialize();
}