#include "ShaderManager.h"
#include "Engine/Engine.h"
#include "Engine/ConvertString/ConvertString.h"
#include <cassert>
#include <format>

ShaderManager* ShaderManager::instance = nullptr;

ShaderManager::ShaderManager() {
	vertexShader.reserve(0);
	hullShader.reserve(0);
	domainShader.reserve(0);
	geometoryShader.reserve(0);
	pixelShader.reserve(0);

	// dxcCompiler��������
	dxcUtils = nullptr;
	dxcCompiler = nullptr;
	HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(hr));

	includeHandler = nullptr;
	hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
	assert(SUCCEEDED(hr));
}

ShaderManager::~ShaderManager() {
	for (auto& i : vertexShader) {
		i.second->Release();
	}
	for (auto& i : hullShader) {
		i.second->Release();
	}
	for (auto& i : domainShader) {
		i.second->Release();
	}
	for (auto& i : geometoryShader) {
		i.second->Release();
	}
	for (auto& i : pixelShader) {
		i.second->Release();
	}
	includeHandler->Release();
	dxcCompiler->Release();
	dxcUtils->Release();
}

void ShaderManager::Initialize() {
	instance = new ShaderManager();
	assert(instance);
}

void ShaderManager::Finalize() {
	delete instance;
	instance = nullptr;
}

IDxcBlob* ShaderManager::CompilerShader(
	// Compiler����Shader�t�@�C���ւ̃p�X
	const std::wstring& filePath,
	// Compiler�Ɏg�p����Profile
	const wchar_t* profile)
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

IDxcBlob* ShaderManager::LoadVertexShader(const std::string& fileName) {
	if (vertexShader.empty()) {
		IDxcBlob* shader = CompilerShader(ConvertString(fileName), L"vs_6_0");
		assert(shader);
		vertexShader.insert(std::make_pair(fileName, shader));
	}
	else {
		auto itr = vertexShader.find(fileName);
		if (itr == vertexShader.end()) {
			IDxcBlob* shader = CompilerShader(ConvertString(fileName), L"vs_6_0");
			assert(shader);
			vertexShader.insert(std::make_pair(fileName, shader));
		}
	}
	return vertexShader[fileName];
}
IDxcBlob* ShaderManager::LoadHullShader(const std::string& fileName) {
	if (hullShader.empty()) {
		IDxcBlob* shader = CompilerShader(ConvertString(fileName), L"hs_6_0");
		assert(shader);
		hullShader.insert(std::make_pair(fileName, shader));
	}
	else {
		auto itr = hullShader.find(fileName);
		if (itr == hullShader.end()) {
			IDxcBlob* shader = CompilerShader(ConvertString(fileName), L"hs_6_0");
			assert(shader);
			hullShader.insert(std::make_pair(fileName, shader));
		}
	}

	return hullShader[fileName];
}
IDxcBlob* ShaderManager::LoadDomainShader(const std::string& fileName) {
	if (domainShader.empty()) {
		IDxcBlob* shader = CompilerShader(ConvertString(fileName), L"ds_6_0");
		assert(shader);
		domainShader.insert(std::make_pair(fileName, shader));
	}
	else {
		auto itr = domainShader.find(fileName);
		if (itr == domainShader.end()) {
			IDxcBlob* shader = CompilerShader(ConvertString(fileName), L"ds_6_0");
			assert(shader);
			domainShader.insert(std::make_pair(fileName, shader));
		}
	}
	return domainShader[fileName];
}
IDxcBlob* ShaderManager::LoadGeometoryShader(const std::string& fileName) {
	if (geometoryShader.empty()) {
		IDxcBlob* shader = CompilerShader(ConvertString(fileName), L"gs_6_0");
		assert(shader);
		geometoryShader.insert(std::make_pair(fileName, shader));
	}
	else {
		auto itr = geometoryShader.find(fileName);
		if (itr == geometoryShader.end()) {
			IDxcBlob* shader = CompilerShader(ConvertString(fileName), L"gs_6_0");
			assert(shader);
			geometoryShader.insert(std::make_pair(fileName, shader));
		}
	}
	return geometoryShader[fileName];
}
IDxcBlob* ShaderManager::LoadPixelShader(const std::string& fileName) {
	if (!pixelShader.empty()) {
		IDxcBlob* shader = CompilerShader(ConvertString(fileName), L"ps_6_0");
		assert(shader);
		pixelShader.insert(std::make_pair(fileName, shader));
	}
	else {
		auto itr = pixelShader.find(fileName);
		if (itr == pixelShader.end()) {
			IDxcBlob* shader = CompilerShader(ConvertString(fileName), L"ps_6_0");
			assert(shader);
			pixelShader.insert(std::make_pair(fileName, shader));
		}
	}
	return pixelShader[fileName];
}