#include "Line.h"
#include "Engine/Engine.h"
#include <algorithm>

Line::Line(){
	vertexBuffer = Engine::CreateBufferResuorce(sizeof(VertexData) * 2);
	vertexView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexView.SizeInBytes = sizeof(VertexData) * 2;
	vertexView.StrideInBytes = sizeof(VertexData);

	vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&vertexMap));

	heap.CreateConstBufferView(wvpMat);

	shader.vertex = ShaderManager::LoadVertexShader("./LineShader/Line.VS.hlsl");
	shader.pixel = ShaderManager::LoadPixelShader("./LineShader/Line.PS.hlsl");

	PipelineManager::CreateRootSgnature(heap.GetParameter(), false);
	PipelineManager::SetVertexInput("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	PipelineManager::SetVertexInput("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	PipelineManager::SetShader(shader);
	PipelineManager::SetState(Pipeline::None, Pipeline::SolidState::Solid, Pipeline::CullMode::None, true);
	pipline = PipelineManager::Create();
	PipelineManager::StateReset();
}

Line::~Line() {
	vertexBuffer->Release();
}

void Line::Draw(const Mat4x4& viewProjection, const Vector2& start, const Vector2& end, uint32_t color){
	vertexMap[0] = { Vector4(start, 0.1f, 1.0f), UintToVector4(color) };
	vertexMap[1] = { Vector4(end, 0.1f, 1.0f),   UintToVector4(color) };

	*wvpMat = viewProjection;

	pipline->Use();
	heap.Use();
	
	Engine::GetCommandList()->IASetVertexBuffers(0,1,&vertexView);
	Engine::GetCommandList()->DrawInstanced(2, 1, 0, 0);
}

void Line::Draw(const Mat4x4& viewProjection, const Vector3& start, const Vector3& end, uint32_t color) {
	vertexMap[0] = { Vector4(start, 1.0f), UintToVector4(color) };
	vertexMap[1] = { Vector4(end, 1.0f),   UintToVector4(color) };

	*wvpMat = viewProjection;

	pipline->Use();
	heap.Use();
	Engine::GetCommandList()->IASetVertexBuffers(0, 1, &vertexView);
	Engine::GetCommandList()->DrawInstanced(2, 1, 0, 0);
}