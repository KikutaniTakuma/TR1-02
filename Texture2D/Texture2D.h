#pragma once
#include "TextureManager/TextureManager.h"
#include "Engine/Engine.h"
#include "Engine/ConstBuffer/ConstBuffer.h"

#include "Math/Mat4x4/Mat4x4.h"
#include "Math/Vector2D/Vector2D.h"
#include "Math/Vector3D/Vector3D.h"
#include "Math/Vector4/Vector4.h"

class Texture2D {
public:
	struct MatrixData {
		Mat4x4 wvpMat;
	};

	struct VertexData {
		Vector3D position;
		Vector2D uv;
	};

public:
	Texture2D();
	~Texture2D();

public:
	void Initialize(const std::string& vsFileName, const std::string& psFileName);

private:
	void CreateDescriptor();

	void CreateShader(const std::string& vsFileName, const std::string& psFileName);

	void CreateGraphicsPipeline();

public:
	void LoadTexture(const std::string& fileName);

	void Draw(Vector2D uv0 = { 0.0f, 1.0f }, Vector2D uv1 = { 1.0f, 1.0f }, Vector2D uv2 = { 1.0f, 0.0f }, Vector2D uv3 = { 0.0f, 0.0f });

private:
	ID3D12DescriptorHeap* SRVHeap = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexView;
	ID3D12Resource* vertexResource = nullptr;

	IDxcBlob* vertexShader = nullptr;
	IDxcBlob* pixelShader = nullptr;

	ID3D12RootSignature* rootSignature = nullptr;
	ID3D12PipelineState* graphicsPipelineState = nullptr;

	Texture* tex;
};