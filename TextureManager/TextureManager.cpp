#include "TextureManager.h"
#include <cassert>

TextureManager* TextureManager::instance = nullptr;

TextureManager* TextureManager::GetInstance() {
	return instance;
}

void TextureManager::Initialize() {
	instance = new TextureManager();
	assert(instance);
}

void TextureManager::Finalize() {
	delete instance;
	instance = nullptr;
}

TextureManager::TextureManager() :
	textures(),
	thisFrameLoadFlg(false)
{

}

TextureManager::~TextureManager() {
	textures.clear();
}


std::shared_ptr<Texture> TextureManager::LoadTexture(const std::string& fileName) {
	if (textures.empty()) {
		auto tex = std::make_shared<Texture>();
		tex->Load(fileName);
		
		textures.insert(std::make_pair(fileName, std::move(tex)));

		thisFrameLoadFlg = true;
	}
	else {
		auto itr = textures.find(fileName);
		if (itr == textures.end()) {
			auto tex = std::make_shared<Texture>();
			tex->Load(fileName);

			textures.insert(std::make_pair(fileName, std::move(tex)));

			thisFrameLoadFlg = true;
		}
	}
	
	return textures[fileName];
}

void TextureManager::UnloadTexture(std::shared_ptr<Texture>& tex) {
	auto itr = textures.find(tex->fileName);
	assert(!(itr == textures.end()));
	if (itr->second.use_count() == 1) {
		// 参照カウントが1の場合コンテナから削除
		textures.erase(itr);
	}
	else {
		tex.reset();
	}
}

void TextureManager::ReleaseIntermediateResource() {
	if (thisFrameLoadFlg) {
		for (auto& i : textures) {
			i.second->ReleaseIntermediateResource();
		}

		thisFrameLoadFlg = false;
	}
}