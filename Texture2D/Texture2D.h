#pragma once
#include "TextureManager/TextureManager.h"
#include "Engine/Engine.h"
#include "Engine/ConstBuffer/ConstBuffer.h"
#include "Engine/Resource/ShaderResource/ShaderResourceHeap.h"
#include "PipelineManager/PipelineManager.h"

#include "Math/Mat4x4/Mat4x4.h"
#include "Math/Vector2/Vector2.h"
#include "Math/Vector3/Vector3.h"
#include "Math/Vector4/Vector4.h"

#include <array>

class Texture2D {
public:
	struct MatrixData {
		Mat4x4 wvpMat;
	};

	struct VertexData {
		Vector3 position;
		Vector2 uv;
	};

public:
	Texture2D();
	~Texture2D();

public:
	void Initialize(const std::string& vsFileName, const std::string& psFileName);

private:
	void CreateShader(const std::string& vsFileName, const std::string& psFileName);

	void CreateGraphicsPipeline();

public:
	void LoadTexture(const std::string& fileName);

public:
	void Draw(
		const Vector2& scale,
		float rotate,
		const Vector2& pos,
		const Mat4x4& viewProjection,
		Pipeline::Blend blend = Pipeline::Blend::None,
		const Vector2& pibot = {0.0f, 0.0f}, const Vector2& size = {1.0f, 1.0f}
	);

	void Draw(
		const Vector2& scale,
		const Vector3& rotate,
		const Vector3& pos,
		const Mat4x4& viewProjection,
		Pipeline::Blend blend = Pipeline::Blend::None,
		const Vector2& pibot = { 0.0f, 0.0f }, const Vector2& size = { 1.0f, 1.0f }
	);

private:
	ShaderResourceHeap SRVHeap;

	D3D12_CPU_DESCRIPTOR_HANDLE SRVHandle;

	D3D12_VERTEX_BUFFER_VIEW vertexView;
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;

	D3D12_INDEX_BUFFER_VIEW indexView;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;

	Shader shader;

	std::array<Pipeline*, size_t(Pipeline::Blend::BlendTypeNum)> graphicsPipelineState;

	ConstBuffer<Mat4x4> wvpMat;
	ConstBuffer<Vector4> color;

	Texture* tex;
};