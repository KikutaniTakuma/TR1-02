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
	Texture* LoadTexture(const std::string& fileName);

	void UnloadTexture(const std::string& fileName);

	void ReleaseIntermediateResource();


private:
	/// <summary>
	/// Texture�̃R���e�i(�L�[�l: �t�@�C���l�[��  �R���e�i�f�[�^�^: �Q�ƃJ�E���g Texture*)
	/// </summary>
	std::unordered_map<std::string, std::pair<uint32_t,std::unique_ptr<Texture>>> textures;

	bool thisFrameLoadFlg;
};