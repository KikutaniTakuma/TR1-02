#include "Texture.h"
#include "Engine/ConvertString/ConvertString.h"
#include "Engine/Engine.h"
#include <cassert>

Texture::Texture():
	textureResouce(nullptr),
	intermediateResource(nullptr),
	srvDesc(),
	loadFlg(false),
	threadLoadFlg(false),
	fileName()
{}

Texture::~Texture() {
	if (intermediateResource) {
		intermediateResource->Release();
		intermediateResource.Reset();
	}
	if (textureResouce) {
		textureResouce->Release();
		textureResouce.Reset();
	}
}


Texture::Texture(Texture&& tex) noexcept {
	*this = std::move(tex);
}
Texture& Texture::operator=(Texture&& tex) noexcept {
	this->textureResouce = std::move(tex.textureResouce);
	this->intermediateResource = std::move(tex.intermediateResource);
	this->srvDesc = tex.srvDesc;
	loadFlg = tex.loadFlg;
	fileName = tex.fileName;

	return *this;
}

void Texture::Load(const std::string& filePath) {
	if (!loadFlg && !threadLoadFlg) {
		this->fileName = filePath;

		DirectX::ScratchImage mipImages = LoadTexture(filePath);
		const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
		textureResouce = CreateTextureResource(metadata);
		intermediateResource = UploadTextureData(textureResouce.Get(), mipImages);

		srvDesc.Format = metadata.format;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

		// load�ς�
		loadFlg = true;
	}
}

void Texture::ThreadLoad(const std::string& filePath) {
	if (!loadFlg && !threadLoadFlg) {
		threadLoadFlg = true;

		this->fileName = filePath;

		DirectX::ScratchImage mipImages = LoadTexture(filePath);
		const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
		textureResouce = CreateTextureResource(metadata);
		intermediateResource = UploadTextureData(textureResouce.Get(), mipImages);

		srvDesc.Format = metadata.format;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);
	}
}

void Texture::Unload() {
	if (loadFlg) {
		srvDesc = {};

		if (intermediateResource) {
			intermediateResource->Release();
			intermediateResource.Reset();
		}
		if (textureResouce) {
			textureResouce->Release();
			textureResouce.Reset();
		}

		// Unload�ς�
		loadFlg = false;
	}
}


DirectX::ScratchImage Texture::LoadTexture(const std::string& filePath) {
	// �e�N�X�`���t�@�C����ǂݍ���Ńv���O������������悤�ɂ���
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	// �~�b�v�}�b�v�̍쐬
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));


	// �~�b�v�}�b�v�t���̃f�[�^��Ԃ�
	return mipImages;
}

ID3D12Resource* Texture::CreateTextureResource(const DirectX::TexMetadata& metaData) {
	// metadata�����Resource�̐ݒ�
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metaData.width);
	resourceDesc.Height = UINT(metaData.height);
	resourceDesc.MipLevels = UINT16(metaData.mipLevels);
	resourceDesc.DepthOrArraySize = UINT16(metaData.arraySize);
	resourceDesc.Format = metaData.format;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metaData.dimension);

	// ���p����Heap�̐ݒ�B���ɓ���ȉ^�p
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	// Resouce�̐���
	ID3D12Resource* resource = nullptr;
	HRESULT hr = Engine::GetDevice()->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&resource)
	);
	assert(SUCCEEDED(hr));
	return resource;
}

[[nodiscard]]
ID3D12Resource* Texture::UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages) {
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	DirectX::PrepareUpload(Engine::GetDevice(), mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresources.size()));
	ID3D12Resource* resource = Engine::CreateBufferResuorce(intermediateSize);
	UpdateSubresources(Engine::GetCommandList(), texture, resource, 0, 0, UINT(subresources.size()), subresources.data());
	// Texture�ւ̓]����͗��p�ł���悤�AD3D12_STATE_COPY_DEST����D3D12_RESOURCE_STATE_GENERIC_READ��ResouceState��ύX����
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	Engine::GetCommandList()->ResourceBarrier(1, &barrier);
	return resource;
}



void Texture::CreateSRVView(D3D12_CPU_DESCRIPTOR_HANDLE descHeapHandle) {
	Engine::GetDevice()->CreateShaderResourceView(textureResouce.Get(), &srvDesc, descHeapHandle);
}


void Texture::ReleaseIntermediateResource() {
	if (intermediateResource) {
		intermediateResource->Release();
		intermediateResource.Reset();
	}
}