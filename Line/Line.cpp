#include "Line.h"
#include "Engine/Engine.h"
#include <algorithm>

Line::Line(){
	vertexBuffer = Engine::CreateBufferResuorce(sizeof(Vector4) * 2);
	vertexView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexView.SizeInBytes = sizeof(Vector4) * 2;
	vertexView.StrideInBytes = sizeof(Vector4);

	vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&vertexMap));

	heap.CreateConstBufferView(wvpMat);
	heap.CreateConstBufferView(colorBuf);

	shader.vertex = ShaderManager::LoadVertexShader("./LineShader/Line.VS.hlsl");
	shader.pixel = ShaderManager::LoadPixelShader("./LineShader/Line.PS.hlsl");

	PipelineManager::CreateRootSgnature(heap.GetParameter(), false);
	PipelineManager::SetVertexInput("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	PipelineManager::SetShader(shader);
	PipelineManager::SetState(Pipeline::None, Pipeline::SolidState::Solid, Pipeline::CullMode::None);
	pipline = PipelineManager::Create();
	PipelineManager::StateReset();
}

Line::~Line() {
	vertexBuffer->Release();
}

void Line::Draw(const Mat4x4& viewProjection, const Vector2& start, const Vector2& end, uint32_t color){
	Vector3 pos = { std::lerp(start.x, end.x, 0.5f),std::lerp(start.y, end.y, 0.5f), 0.0f };

	vertexMap[0] = Vector4(start, 0.0f, 1.0f);
	vertexMap[1] = Vector4(end, 0.0f, 1.0f);

	*wvpMat = viewProjection * VertMakeMatrixTranslate(pos);

	*colorBuf = UintToVector4(color);

	pipline->Use();
	heap.Use();
	Engine::GetCommandList()->IASetVertexBuffers(0,1,&vertexView);
	Engine::GetCommandList()->DrawInstanced(2, 1, 0, 0);
}

void Line::Draw(const Mat4x4& viewProjection, const Vector3& start, const Vector3& end, uint32_t color) {
	Vector3 pos = { std::lerp(start.x, end.x, 0.5f),std::lerp(start.y, end.y, 0.5f), std::lerp(start.z, end.z, 0.5f) };

	vertexMap[0] = Vector4(start, 1.0f);
	vertexMap[1] = Vector4(end, 1.0f);

	*wvpMat = viewProjection * VertMakeMatrixTranslate(pos);

	*colorBuf = UintToVector4(color);

	pipline->Use();
	heap.Use();
	Engine::GetCommandList()->IASetVertexBuffers(0, 1, &vertexView);
	Engine::GetCommandList()->DrawInstanced(2, 1, 0, 0);
}