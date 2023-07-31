#include "TextureManager.h"
#include <cassert>

TextureManager* TextureManager::instance = nullptr;

TextureManager* TextureManager::GetInstance() {
	return instance;
}

void TextureManager::Initialize() {
	instance = new TextureManager();
	assert(instance);
	instance->LoadTexture("./Resources/white2x2.png");
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


Texture* TextureManager::LoadTexture(const std::string& fileName) {
	if (textures.empty()) {
		auto tex = std::make_unique<Texture>();
		tex->Load(fileName);
		
		textures.insert(std::make_pair(fileName, std::move(tex)));

		thisFrameLoadFlg = true;
	}
	else {
		auto itr = textures.find(fileName);
		if (itr == textures.end()) {
			auto tex = std::make_unique<Texture>();
			tex->Load(fileName);

			textures.insert(std::make_pair(fileName, std::move(tex)));

			thisFrameLoadFlg = true;
		}
	}
	
	return textures[fileName].get();
}

Texture* TextureManager::GetWhiteTex() {
	return instance->LoadTexture("./Resources/white2x2.png");
}

void TextureManager::ReleaseIntermediateResource() {
	if (thisFrameLoadFlg) {
		for (auto& i : textures) {
			i.second->ReleaseIntermediateResource();
		}

		thisFrameLoadFlg = false;
	}
}