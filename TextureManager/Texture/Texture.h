#pragma once

#include <d3d12.h>

#include "externals//DirectXTex/DirectXTex.h"
#include "externals//DirectXTex/d3dx12.h"
#include <wrl.h>

#include <string>

#include "Math/Vector2/Vector2.h"

class Texture {
	friend class TextureManager;

public:
	Texture();
	~Texture();

	Texture(const Texture&) = delete;
	Texture(Texture&& tex) noexcept;

	Texture& operator=(const Texture&) = delete;
	Texture& operator=(Texture&& tex) noexcept;

public:
	inline explicit operator bool() const noexcept {
		return loadFlg;
	}

	inline bool operator!() const noexcept {
		return !loadFlg;
	}


/// <summary>
/// 画像読み込み用
/// </summary>
private:
	void Load(const std::string& filePath);
	void ThreadLoad(const std::string& filePath);
	void Unload();

	DirectX::ScratchImage LoadTexture(const std::string& filePath);
	ID3D12Resource* CreateTextureResource(const DirectX::TexMetadata& metaData);
	[[nodiscard]]
	ID3D12Resource* UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);

/// <summary>
/// View作成関数
/// </summary>
public:
	void CreateSRVView(D3D12_CPU_DESCRIPTOR_HANDLE descHeapHandle);

/// <summary>
/// CPU側のリソースを解放
/// </summary>
private:
	void ReleaseIntermediateResource();


/// <summary>
/// メンバ関数
/// </summary>
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResouce;
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;

	bool loadFlg;
	bool threadLoadFlg;

	Vector2 size;
public:
	inline const Vector2& getSize() const {
		return size;
	}

private:
	std::string fileName;
public:
	inline const std::string& GetFileName() const {
		return fileName;
	}
};