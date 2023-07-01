#pragma once

#include <d3d12.h>

#include "externals//DirectXTex/DirectXTex.h"
#include "externals//DirectXTex/d3dx12.h"

#include <string>

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
	inline explicit operator bool() {
		return loadFlg;
	}

	inline bool operator!() {
		return !loadFlg;
	}


/// <summary>
/// âÊëúì«Ç›çûÇ›óp
/// </summary>
private:
	void Load(const std::string& filePath);
	void Unload();

	DirectX::ScratchImage LoadTexture(const std::string& filePath);
	ID3D12Resource* CreateTextureResource(const DirectX::TexMetadata& metaData);
	[[nodiscard]]
	ID3D12Resource* UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);

/// <summary>
/// ViewçÏê¨ä÷êî
/// </summary>
public:
	void CreateSRVView(D3D12_CPU_DESCRIPTOR_HANDLE descHeapHandle);

private:
	void ReleaseIntermediateResource();


/// <summary>
/// ÉÅÉìÉoä÷êî
/// </summary>
private:
	ID3D12Resource* textureResouce = nullptr;
	ID3D12Resource* intermediateResource = nullptr;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;

	bool loadFlg;

private:
	std::string fileName;
public:
	inline const std::string& GetFileName() const {
		return fileName;
	}
};