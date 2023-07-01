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

	// dxcCompilerを初期化
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
	// CompilerするShaderファイルへのパス
	const std::wstring& filePath,
	// Compilerに使用するProfile
	const wchar_t* profile)
{
	// 1. hlslファイルを読む
	// これからシェーダーをコンパイルする旨をログに出す
	Log(ConvertString(std::format(L"Begin CompilerShader, path:{}, profile:{}\n", filePath, profile)));
	// hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	// 読めなかったら止める
	assert(SUCCEEDED(hr));
	// 読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;


	// 2. Compileする
	LPCWSTR arguments[] = {
		filePath.c_str(), // コンパイル対象のhlslファイル名
		L"-E", L"main", // エントリーポイントの指定。基本的にmain以外にはしない
		L"-T", profile, // ShaderProfileの設定
		L"-Zi", L"-Qembed_debug", // デバッグ用の情報を埋め込む
		L"-Od", // 最適化を外しておく
		L"-Zpr" // メモリレイアウトを優先
	};
	// 実際にShaderをコンパイルする
	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler->Compile(
		&shaderSourceBuffer, // 読みこんだファイル
		arguments,           // コンパイルオプション
		_countof(arguments), // コンパイルオプションの数
		includeHandler,      // includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult) // コンパイル結果
	);

	// コンパイルエラーではなくdxcが起動できないなど致命的な状況
	assert(SUCCEEDED(hr));

	// 3. 警告・エラーが出てないか確認する
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Log(shaderError->GetStringPointer());
		// 警告・エラーダメゼッタイ
		assert(false);
	}

	// 4. Compileを受け取って返す
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	// 成功したログを出す
	Log(ConvertString(std::format(L"Compile Succeeded, path:{}, profile:{}\n", filePath, profile)));
	// もう使わないリソースを解放
	shaderSource->Release();
	shaderResult->Release();
	// 実行用バイナリをリターン
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