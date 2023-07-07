#pragma once
#include "Texture/Texture.h"

#include <unordered_map>
#include <string>
#include <memory>

class TextureManager {
private:
	TextureManager();
	~TextureManager();

public:
	static TextureManager* GetInstance();

	static void Initialize();

	static void Finalize();

private:
	static TextureManager* instance;


public:
	std::shared_ptr<Texture> LoadTexture(const std::string& fileName);

	void UnloadTexture(std::shared_ptr<Texture>& tex);

	void ReleaseIntermediateResource();


private:
	/// <summary>
	/// Textureのコンテナ(キー値: ファイルネーム  コンテナデータ型: Texture*)
	/// </summary>
	std::unordered_map<std::string, std::shared_ptr<Texture>> textures;

	bool thisFrameLoadFlg;
};