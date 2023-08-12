#include "TextureManager.h"
#include "Engine/ErrorCheck/ErrorCheck.h"
#include "Engine/Engine.h"
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
	if (instance->load.joinable()) {
		instance->load.join();
	}
	delete instance;
	instance = nullptr;
}

TextureManager::TextureManager() :
	textures(),
	thisFrameLoadFlg(false),
	threadTextureBuff(),
	load(),
	isThreadFinish(false)
{
	// コマンドアロケータを生成する
	commandAllocator = nullptr;
	HRESULT hr = Engine::GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator.GetAddressOf()));
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("TextureManager() : CreateCommandAllocator() Failed", "TextureManager");
		return;
	}

	// コマンドリストを作成する
	commandList = nullptr;
	hr = Engine::GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(commandList.GetAddressOf()));
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("TextureManager() : CreateCommandList() Failed", "TextureManager");
		return;
	}
}

TextureManager::~TextureManager() {
	textures.clear();
}


Texture* TextureManager::LoadTexture(const std::string& fileName) {
	if (textures.empty()) {
		auto tex = std::make_unique<Texture>();
		tex->Load(fileName);
		if (!tex->loadFlg) {
			return nullptr;
		}
		
		textures.insert(std::make_pair(fileName, std::move(tex)));

		thisFrameLoadFlg = true;
	}
	else {
		auto itr = textures.find(fileName);
		if (itr == textures.end()) {
			auto tex = std::make_unique<Texture>();
			tex->Load(fileName);
			if (!tex->loadFlg) {
				return nullptr;
			}

			textures.insert(std::make_pair(fileName, std::move(tex)));

			thisFrameLoadFlg = true;
		}
	}
	
	return textures[fileName].get();
}

Texture* TextureManager::LoadTexture(const std::string& fileName, ID3D12GraphicsCommandList* commandList_) {
	if (textures.empty()) {
		auto tex = std::make_unique<Texture>();
		tex->Load(fileName, commandList_);
		if (!tex->loadFlg) {
			return nullptr;
		}

		textures.insert(std::make_pair(fileName, std::move(tex)));

		thisFrameLoadFlg = true;
	}
	else {
		auto itr = textures.find(fileName);
		if (itr == textures.end()) {
			auto tex = std::make_unique<Texture>();
			tex->Load(fileName, commandList_);
			if (!tex->loadFlg) {
				return nullptr;
			}

			textures.insert(std::make_pair(fileName, std::move(tex)));

			thisFrameLoadFlg = true;
		}
	}

	return textures[fileName].get();
}

void TextureManager::LoadTexture(const std::string& fileName, Texture** texPtr) {
	if (load.joinable() && threadTextureBuff.empty()) {
		load.join();
	}
	
	// コンテナに追加
	threadTextureBuff.push({ fileName, texPtr });
}

void TextureManager::ThreadLoadTexture() {
	if (!load.joinable() && !threadTextureBuff.empty()) {
		auto loadProc = [this]() {
			while (!threadTextureBuff.empty()) {
				auto&& front = threadTextureBuff.front();
				*front.second = LoadTexture(front.first, commandList.Get());
				threadTextureBuff.pop();
			}

			isThreadFinish = true;
		};

		load = std::thread(loadProc);
	}
}

Texture* TextureManager::GetWhiteTex() {
	return instance->LoadTexture("./Resources/white2x2.png");
}

void TextureManager::ReleaseIntermediateResource() {
	if (thisFrameLoadFlg && threadTextureBuff.empty()) {
		for (auto& i : textures) {
			i.second->ReleaseIntermediateResource();
		}

		thisFrameLoadFlg = false;
	}
}

void TextureManager::ResetCommandList() {
	// 次フレーム用のコマンドリストを準備
	auto hr = commandAllocator->Reset();
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("CommandAllocator->Reset() Failed", "Engine");
	}
	hr = commandList->Reset(commandAllocator.Get(), nullptr);
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("CommandList->Reset() Failed", "Engine");
	}

	load.join();

	isThreadFinish = false;
}