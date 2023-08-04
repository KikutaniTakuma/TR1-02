#pragma once
#include "Texture/Texture.h"

#include <unordered_map>
#include <string>
#include <memory>

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

	Texture* GetWhiteTex();

	void ReleaseIntermediateResource();


private:
	/// <summary>
	/// Textureのコンテナ(キー値: ファイルネーム  コンテナデータ型: Texture*)
	/// </summary>
	std::unordered_map<std::string, std::unique_ptr<Texture>> textures;

	bool thisFrameLoadFlg;
};