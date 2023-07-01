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
	for (auto& i : textures) {
		i.second.second.reset();
	}
}


Texture* TextureManager::LoadTexture(const std::string& fileName) {
	if (textures.empty()) {
		auto tex = std::make_unique<Texture>();
		tex->Load(fileName);
		
		textures.insert(std::make_pair(fileName, std::make_pair(0,std::move(tex))));

		thisFrameLoadFlg = true;
	}
	else {
		auto itr = textures.find(fileName);
		if (itr == textures.end()) {
			auto tex = std::make_unique<Texture>();
			tex->Load(fileName);
			
			textures.insert(std::make_pair(fileName, std::make_pair(0, std::move(tex))));

			thisFrameLoadFlg = true;
		}
	}
	// �Q�ƃJ�E���g���C���N�������g
	textures[fileName].first++;
	return textures[fileName].second.get();
}

void TextureManager::UnloadTexture(const std::string& fileName) {
	auto itr = textures.find(fileName);
	assert(!(itr == textures.end()));
	if (itr->second.first == 1) {
		// �Q�ƃJ�E���g��1�̏ꍇ�R���e�i����폜
		textures.erase(itr);
	}
	else {
		// �Q�ƃJ�E���g���f�N�������g
		itr->second.first--;
	}
}

void TextureManager::ReleaseIntermediateResource() {
	if (thisFrameLoadFlg) {
		for (auto& i : textures) {
			i.second.second->ReleaseIntermediateResource();
		}

		thisFrameLoadFlg = false;
	}
}