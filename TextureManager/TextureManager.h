#pragma once
#include "Texture/Texture.h"

#include <unordered_map>
#include <string>
#include <memory>
#include <queue>
#include <thread>

class TextureManager {
private:
	TextureManager();
	TextureManager(const TextureManager&) = delete;
	TextureManager(TextureManager&&) noexcept = delete;
	~TextureManager();

	TextureManager& operator=(const TextureManager&) = delete;
	TextureManager& operator=(TextureManager&&) noexcept = delete;

public:
	static TextureManager* GetInstance();

	static void Initialize();

	static void Finalize();

private:
	static TextureManager* instance;


public:
	Texture* LoadTexture(const std::string& fileName);

private:
	Texture* LoadTexture(const std::string& fileName, ID3D12GraphicsCommandList* commandList_);

public:
	/// <summary>
	/// スレッドを分けてロードする用
	/// </summary>
	/// <param name="fileName"></param>
	/// <param name="texPtr"></param>
	void LoadTexture(const std::string& fileName, Texture** texPtr);

	void ThreadLoadTexture();

	Texture* GetWhiteTex();

	void ReleaseIntermediateResource();

	bool ThreadLoadFinish() const {
		return isThreadFinish;
	}

	inline ID3D12GraphicsCommandList* GetCommandList() const {
		return commandList.Get();
	}

	void ResetCommandList();


private:
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;

	/// <summary>
	/// Textureのコンテナ(キー値: ファイルネーム  コンテナデータ型: Texture*)
	/// </summary>
	std::unordered_map<std::string, std::unique_ptr<Texture>> textures;
	bool thisFrameLoadFlg;

	std::queue<std::pair<std::string, Texture**>> threadTextureBuff;
	std::thread load;
	bool isThreadFinish;
};