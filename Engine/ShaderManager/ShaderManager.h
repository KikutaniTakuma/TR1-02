#pragma once
#include <Windows.h>
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#include <dxgi1_6.h>
#pragma comment(lib, "dxgi.lib")
#include <dxcapi.h>
#pragma comment(lib, "dxcompiler.lib")

#include <unordered_map>
#include <string>

class ShaderManager {
private:
	ShaderManager();
	~ShaderManager();

public:
	static void Initialize();

	static void Finalize();

	static inline ShaderManager* GetInstance() {
		return instance;
	}

private:
	static ShaderManager* instance;


public:
	IDxcBlob* LoadVertexShader(const std::string& fileName);
	IDxcBlob* LoadHullShader(const std::string& fileName);
	IDxcBlob* LoadDomainShader(const std::string& fileName);
	IDxcBlob* LoadGeometoryShader(const std::string& fileName);
	IDxcBlob* LoadPixelShader(const std::string& fileName);

private:
	IDxcBlob* CompilerShader(
		// CompilerするShaderファイルへのパス
		const std::wstring& filePath,
		// Compilerに使用するProfile
		const wchar_t* profile);

private:
	IDxcUtils* dxcUtils = nullptr;
	IDxcCompiler3* dxcCompiler = nullptr;
	IDxcIncludeHandler* includeHandler = nullptr;

private:
	std::unordered_map<std::string, IDxcBlob*> vertexShader;
	std::unordered_map<std::string, IDxcBlob*> hullShader;
	std::unordered_map<std::string, IDxcBlob*> domainShader;
	std::unordered_map<std::string, IDxcBlob*> geometoryShader;
	std::unordered_map<std::string, IDxcBlob*> pixelShader;
};