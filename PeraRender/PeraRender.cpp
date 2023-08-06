#include "PeraRender.h"
#include "Engine/Engine.h"
#include <cassert>
#include "Engine/ConvertString/ConvertString.h"
#include "Engine/ShaderManager/ShaderManager.h"
#include "externals/imgui/imgui.h"

PeraRender::PeraRender():
	render(),
	peraVertexResource(nullptr),
	peraVertexView(),
	shader{},
	graphicsPipelineState(nullptr)
{}

PeraRender::~PeraRender() {
	peraVertexResource->Release();
}

void PeraRender::Initialize(const std::string& vsFileName, const std::string& psFileName) {
	CreateShader(vsFileName, psFileName);

	CreateGraphicsPipeline();

	std::array<PeraVertexData, 4> pv = {
		Vector3{ -1.0f,-1.0f, 0.1f }, Vector2{ 0.0f, 1.0f },
		Vector3{ -1.0f, 1.0f, 0.1f }, Vector2{ 0.0f, 0.0f },
		Vector3{  1.0f,-1.0f, 0.1f }, Vector2{ 1.0f, 1.0f },
		Vector3{  1.0f, 1.0f, 0.1f }, Vector2{ 1.0f, 0.0f }
	};

	peraVertexResource = Engine::CreateBufferResuorce(sizeof(pv));

	peraVertexView.BufferLocation = peraVertexResource->GetGPUVirtualAddress();
	peraVertexView.SizeInBytes = sizeof(pv);
	peraVertexView.StrideInBytes = sizeof(PeraVertexData);

	PeraVertexData* mappedData = nullptr;
	peraVertexResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedData));
	std::copy(pv.begin(), pv.end(), mappedData);
	peraVertexResource->Unmap(0, nullptr);
}

void PeraRender::CreateShader(const std::string& vsFileName, const std::string& psFileName) {
	shader.vertex = ShaderManager::LoadVertexShader(vsFileName);
	assert(shader.vertex);
	shader.pixel = ShaderManager::LoadPixelShader(psFileName);
	assert(shader.pixel);
}

void PeraRender::CreateGraphicsPipeline() {
	// RootSignatureの生成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER roootParamater[1] = {};
	roootParamater[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	roootParamater[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	roootParamater[0].DescriptorTable.pDescriptorRanges = descriptorRange;
	roootParamater[0].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);
	
	PipelineManager::CreateRootSgnature(roootParamater[0], true);

	PipelineManager::SetVertexInput("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT);
	PipelineManager::SetVertexInput("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);

	PipelineManager::SetShader(shader);
	PipelineManager::SetState(Pipeline::None, Pipeline::SolidState::Solid);

	PipelineManager::IsDepth(false);

	graphicsPipelineState = PipelineManager::Create();

	PipelineManager::StateReset();
}

void PeraRender::PreDraw() {
	render.SetThisRenderTarget();
}

void PeraRender::Draw() {
	// 描画先をメインレンダーターゲットに変更
	render.SetMainRenderTarget();

	// 各種描画コマンドを積む
	graphicsPipelineState->Use();
	Engine::GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	Engine::GetCommandList()->IASetVertexBuffers(0, 1, &peraVertexView);
	render.UseThisRenderTargetShaderResource();
	Engine::GetCommandList()->DrawInstanced(4, 1, 0, 0);
}