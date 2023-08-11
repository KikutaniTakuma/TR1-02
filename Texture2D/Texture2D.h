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
	Texture2D(const Texture2D&);
	Texture2D(Texture2D&&) noexcept;
	~Texture2D();

	Texture2D& operator=(const Texture2D& right);


public:
	void Initialize(
		const std::string& vsFileName = "Texture2DShader/Texture2D.VS.hlsl",
		const std::string& psFileName = "Texture2DShader/Texture2DNone.PS.hlsl"
	);

private:
	void CreateShader(const std::string& vsFileName, const std::string& psFileName);

	void CreateGraphicsPipeline();

public:
	void LoadTexture(const std::string& fileName);

public:
	void Update();

	void Draw(
		const Mat4x4& viewProjection,
		Pipeline::Blend blend = Pipeline::Blend::None
	);

	void Debug(const std::string& guiName);

	bool Colision(const Vector2& pos2D);
	bool Colision(const Texture2D& tex2D);

public:
	Vector2 scale;
	Vector3 rotate;
	Vector3 pos;

	Vector2 uvPibot;
	Vector2 uvSize;

	std::array<Vector3, 4> worldPos;

private:
	ShaderResourceHeap SRVHeap;

	uint32_t SRVHandle;

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