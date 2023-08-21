#pragma once
#include "Math/Mat4x4/Mat4x4.h"
#include "Math/Vector2/Vector2.h"
#include "Math/Vector3/Vector3.h"
#include "Math/Vector4/Vector4.h"
#include <string>
#include <array>

#include "Engine/ConstBuffer/ConstBuffer.h"
#include "Engine/Resource/RenderTarget/RenderTarget.h"
#include "PipelineManager/PipelineManager.h"
#include "Engine/ShaderManager/ShaderManager.h"

class PeraRender {
public:
	struct PeraVertexData {
		Vector3 position;
		Vector2 uv;
	};

	struct Wipe {
		Vector2 center;
		float wipeSize;
	};

public:
	PeraRender();
	PeraRender(uint32_t width_, uint32_t height_);
	~PeraRender();

	PeraRender& operator=(const PeraRender&) = default;

public:
	void Initialize(const std::string& vsFileName, const std::string& psFileName);

private:
	void CreateShader(const std::string& vsFileName, const std::string& psFileName);

	void CreateGraphicsPipeline();

public:
	void PreDraw();

	void Draw(Pipeline::Blend blend, PeraRender* pera = nullptr);

private:
	RenderTarget render;

	D3D12_VERTEX_BUFFER_VIEW peraVertexView;
	Microsoft::WRL::ComPtr<ID3D12Resource> peraVertexResource = nullptr;
	Shader shader;

	std::array<Pipeline*, 3> piplines;
};