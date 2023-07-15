#pragma once
#include "Pipeline/Pipeline.h"
#include "Engine/RootSignature/RootSignature.h"
#include "Engine/ShaderManager/ShaderManager.h"
#include <list>
#include <vector>
#include <memory>
#include <tuple>

class PipelineManager {
private:
	PipelineManager();
	PipelineManager(const PipelineManager&) = delete;
	PipelineManager(PipelineManager&&) = delete;
	~PipelineManager() = default;

	PipelineManager& operator=(const PipelineManager&) = delete;
	PipelineManager& operator=(PipelineManager&&) = delete;

public:
	static void Initialize();
	static void Finalize();

	///
	/// 下の4つの関数を使った後にCreateする
	/// 
	static void CreateRootSgnature(const D3D12_ROOT_PARAMETER& rootParamater_, bool isTexture_);
	static void SetVertexInput(std::string semanticName_, uint32_t semanticIndex_, DXGI_FORMAT format_);
	static void SetShader(const Shader& shader_);
	static void SetState(
		Pipeline::Blend blend_,
		Pipeline::CullMode cullMode_,
		Pipeline::SolidState solidState_,
		uint32_t numRenderTarget_ = 1u
	);

	static Pipeline* Create();

	/// 基本的にCreateした後に使う
	static void StateReset();

private:
	static PipelineManager* instance;

private:
	std::list<std::unique_ptr<Pipeline>> pipelines;
	std::list<std::unique_ptr<RootSignature>> rootSignatures;

	Shader shader;
	Pipeline::Blend blend;
	Pipeline::CullMode cullMode;
	Pipeline::SolidState solidState;
	uint32_t numRenderTarget;

	std::vector<std::tuple<std::string, uint32_t, DXGI_FORMAT>> vertexInputStates;
};