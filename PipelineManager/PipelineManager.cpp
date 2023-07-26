#include "PipelineManager.h"
#include <cassert>
#include <algorithm>

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
	if (instance->rootSignatures.empty()) {
		auto rootSignature = std::make_unique<RootSignature>();

		rootSignature->Create(rootParamater_, isTexture_);

		instance->rootSignature = rootSignature.get();

		instance->rootSignatures.push_back(std::move(rootSignature));
	}
	else {
		auto IsSame = [&rootParamater_, &isTexture_](const std::unique_ptr<RootSignature>& rootSignature_) {
			return rootSignature_->IsSame(rootParamater_, isTexture_);
		};

		auto rootSignatureItr = std::find_if(instance->rootSignatures.begin(), instance->rootSignatures.end(), IsSame);

		if (rootSignatureItr == instance->rootSignatures.end()) {
			auto rootSignature = std::make_unique<RootSignature>();

			rootSignature->Create(rootParamater_, isTexture_);

			instance->rootSignature = rootSignature.get();

			instance->rootSignatures.push_back(std::move(rootSignature));
		}
		else {
			instance->rootSignature = rootSignatureItr->get();
		}
	}
}
void PipelineManager::SetRootSgnature(RootSignature* rootSignature) {
	instance->rootSignature = rootSignature;
}

void PipelineManager::SetVertexInput(std::string semanticName_, uint32_t semanticIndex_, DXGI_FORMAT format_) {
	instance->vertexInputStates.push_back({ semanticName_, semanticIndex_, format_ });
}
void PipelineManager::SetShader(const Shader& shader_) {
	instance->shader = shader_;
}

void PipelineManager::SetState(
	Pipeline::Blend blend_,
	Pipeline::SolidState solidState_,
	Pipeline::CullMode cullMode_,
	bool isLine_,
	uint32_t numRenderTarget_
) {
	instance->blend = std::clamp(blend_, Pipeline::Blend::None, Pipeline::Blend(Pipeline::Blend::BlendTypeNum - 1));
	instance->cullMode = cullMode_;
	instance->solidState = solidState_;
	instance->isLine = isLine_;
	instance->numRenderTarget = numRenderTarget_;
}

Pipeline* PipelineManager::Create() {
	if (instance->pipelines.empty()) {
		auto pipeline = std::make_unique<Pipeline>();
		pipeline->SetShader(instance->shader);
		for (auto& i : instance->vertexInputStates) {
			pipeline->SetVertexInput(std::get<0>(i), std::get<1>(i), std::get<2>(i));
		}
		pipeline->Create(
			*instance->rootSignature,
			instance->blend,
			instance->cullMode,
			instance->solidState,
			instance->isLine,
			instance->numRenderTarget
		);

		instance->pipelines.push_back(std::move(pipeline));

		return instance->pipelines.rbegin()->get();
	}
	else {
		auto IsSmae = [](const std::unique_ptr<Pipeline>& pipeline) {
			bool issame = pipeline->IsSame(
				instance->shader,
				instance->blend,
				instance->cullMode,
				instance->solidState,
				instance->isLine,
				instance->numRenderTarget,
				instance->rootSignature->Get()
			);

			return issame;
		};

		auto pipelineItr = std::find_if(instance->pipelines.begin(), instance->pipelines.end(),IsSmae);

		if (pipelineItr == instance->pipelines.end()) {
			auto pipeline = std::make_unique<Pipeline>();
			pipeline->SetShader(instance->shader);
			for (auto& i : instance->vertexInputStates) {
				pipeline->SetVertexInput(std::get<0>(i), std::get<1>(i), std::get<2>(i));
			}
			pipeline->Create(
				*instance->rootSignature,
				instance->blend,
				instance->cullMode,
				instance->solidState,
				instance->isLine,
				instance->numRenderTarget
			);

			instance->pipelines.push_back(std::move(pipeline));

			return instance->pipelines.rbegin()->get();
		}
		else {
			return pipelineItr->get();
		}
	}
}

void PipelineManager::StateReset() {
	instance->rootSignature = nullptr;
	instance->shader = { nullptr };
	instance->vertexInputStates.clear();
	instance->blend = {};
	instance->cullMode = {};
	instance->solidState = {};
	instance->isLine = false;
	instance->numRenderTarget = 0u;
}

PipelineManager::PipelineManager() :
	pipelines(),
	rootSignatures(),
	rootSignature(nullptr),
	shader{},
	blend(),
	cullMode(),
	solidState(),
	isLine(false),
	numRenderTarget(0u),
	vertexInputStates(0)
{}