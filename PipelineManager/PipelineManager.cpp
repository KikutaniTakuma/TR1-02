#include "PipelineManager.h"
#include <cassert>

PipelineManager* PipelineManager::instance = nullptr;

void PipelineManager::Initialize() {
	instance = new PipelineManager();
	assert(instance);
}
void PipelineManager::Finalize() {
	delete instance;
	instance = nullptr;
}

void PipelineManager::CreateRootSgnature(const D3D12_ROOT_PARAMETER& rootParamater_, bool isTexture_) {
	instance->rootParamater = rootParamater_;
	instance->isTexture = isTexture_;
}
void PipelineManager::SetVertexInput(std::string semanticName_, uint32_t semanticIndex_, DXGI_FORMAT format_) {
	instance->vertexInputStates.push_back({ semanticName_, semanticIndex_, format_ });
}
void PipelineManager::SetShader(const Shader& shader_) {
	instance->shader = shader_;
}

void PipelineManager::SetState(
	Pipeline::Blend blend_,
	Pipeline::CullMode cullMode_,
	Pipeline::SolidState solidState_,
	uint32_t numRenderTarget_
) {
	instance->blend = blend_;
	instance->cullMode = cullMode_;
	instance->solidState = solidState_;
	instance->numRenderTarget = numRenderTarget_;
}

Pipeline* PipelineManager::Create() {
	if (instance->pipelines.empty()) {
		auto pipeline = std::make_unique<Pipeline>();
		pipeline->CreateRootSgnature(instance->rootParamater, instance->isTexture);
		pipeline->SetShader(instance->shader);
		for (auto& i : instance->vertexInputStates) {
			pipeline->SetVertexInput(std::get<0>(i), std::get<1>(i), std::get<2>(i));
		}
		pipeline->Create(
			instance->blend,
			instance->cullMode,
			instance->solidState,
			instance->numRenderTarget
		);

		instance->pipelines.push_back(std::move(pipeline));

		// ステータスのリセット
		instance->rootParamater = {};
		instance->isTexture = false;
		instance->shader = { nullptr };
		instance->vertexInputStates.clear();
		instance->blend = {};
		instance->cullMode = {};
		instance->solidState = {};
		instance->numRenderTarget = 0u;

		return instance->pipelines.rbegin()->get();
	}
	else {
		auto IsSmae = [](const std::unique_ptr<Pipeline>& pipeline) {
			return pipeline->IsSame(
				instance->rootParamater,
				instance->isTexture,
				instance->shader,
				instance->blend,
				instance->cullMode,
				instance->solidState,
				instance->numRenderTarget
			);
		};

		auto pipelineItr = std::find_if(instance->pipelines.begin(), instance->pipelines.end(),IsSmae);

		if (pipelineItr == instance->pipelines.end()) {
			auto pipeline = std::make_unique<Pipeline>();
			pipeline->CreateRootSgnature(instance->rootParamater, instance->isTexture);
			pipeline->SetShader(instance->shader);
			for (auto& i : instance->vertexInputStates) {
				pipeline->SetVertexInput(std::get<0>(i), std::get<1>(i), std::get<2>(i));
			}
			pipeline->Create(
				instance->blend,
				instance->cullMode,
				instance->solidState,
				instance->numRenderTarget
			);

			instance->pipelines.push_back(std::move(pipeline));

			// ステータスのリセット
			instance->rootParamater = {};
			instance->isTexture = false;
			instance->shader = { nullptr };
			instance->vertexInputStates.clear();
			instance->blend = {};
			instance->cullMode = {};
			instance->solidState = {};
			instance->numRenderTarget = 0u;

			return instance->pipelines.rbegin()->get();
		}
		else {
			// ステータスのリセット
			instance->rootParamater = {};
			instance->isTexture = false;
			instance->shader = { nullptr };
			instance->vertexInputStates.clear();
			instance->blend = {};
			instance->cullMode = {};
			instance->solidState = {};
			instance->numRenderTarget = 0u;

			return pipelineItr->get();
		}
	}
}

PipelineManager::PipelineManager() :
	pipelines(),
	rootParamater{},
	isTexture(false),
	shader{},
	blend(),
	cullMode(),
	solidState(),
	numRenderTarget(0u),
	vertexInputStates(0)
{}