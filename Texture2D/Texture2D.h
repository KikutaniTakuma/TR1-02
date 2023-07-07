#pragma once
#include "TextureManager/TextureManager.h"
#include "Engine/Engine.h"
#include "Engine/ConstBuffer/ConstBuffer.h"

#include "Math/Mat4x4/Mat4x4.h"
#include "Math/Vector2/Vector2.h"
#include "Math/Vector3/Vector3.h"
#include "Math/Vector4/Vector4.h"

#include <array>

class Texture2D {
public:
	enum class Blend : uint16_t{
		None,
		Add,
		Multiply,

		BlendModeNum
	};

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
	void CreateDescriptor();

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
		Blend blend = Blend::None,
		const Vector2& uv0 = {0.0f, 1.0f}, const Vector2& uv1 = {1.0f, 1.0f}, 
		const Vector2& uv2 = {1.0f, 0.0f}, const Vector2& uv3 = {0.0f, 0.0f}
	);

private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> SRVHeap;

	D3D12_VERTEX_BUFFER_VIEW vertexView;
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;

	D3D12_INDEX_BUFFER_VIEW indexView;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;

	IDxcBlob* vertexShader = nullptr;
	IDxcBlob* pixelShader = nullptr;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
	std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, size_t(Blend::BlendModeNum)> graphicsPipelineState;

	ConstBuffer<Mat4x4> wvpMat;

	std::shared_ptr<Texture> tex;
};