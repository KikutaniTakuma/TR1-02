#include "Texture2D.h"
#include "Engine/ShaderManager/ShaderManager.h"
#include "externals/imgui/imgui.h"

Texture2D::Texture2D():
	SRVHeap(16),
	vertexView(),
	vertexResource(nullptr),
	indexView(),
	indexResource(nullptr),
	shader(),
	graphicsPipelineState(),
	tex(nullptr)
{}

Texture2D::~Texture2D() {
	if(indexResource)indexResource->Release();
	if(vertexResource)vertexResource->Release();
}

void Texture2D::Initialize(const std::string& vsFileName, const std::string& psFileName) {
	CreateShader(vsFileName, psFileName);

	vertexResource = Engine::CreateBufferResuorce(sizeof(VertexData) * 4);

	vertexView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexView.SizeInBytes = sizeof(VertexData) * 4;
	vertexView.StrideInBytes = sizeof(VertexData);


	uint16_t indices[] = {
			0,1,3, 1,2,3
	};
	indexResource = Engine::CreateBufferResuorce(sizeof(indices));
	indexView.BufferLocation = indexResource->GetGPUVirtualAddress();
	indexView.SizeInBytes = sizeof(indices);
	indexView.Format = DXGI_FORMAT_R16_UINT;
	uint16_t* indexMap = nullptr;
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexMap));
	for (int32_t i = 0; i < _countof(indices); i++) {
		indexMap[i] = indices[i];
	}
	indexResource->Unmap(0, nullptr);

	*colorPibot = 0.6f;

	*colorType = 0;

	*color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	SRVHeap.CreateConstBufferView(wvpMat);
	SRVHeap.CreateConstBufferView(color);
	SRVHeap.CreateConstBufferView(colorPibot);
	SRVHeap.CreateConstBufferView(colorType);


	CreateGraphicsPipeline();
}

void Texture2D::CreateShader(const std::string& vsFileName, const std::string& psFileName) {
	shader.vertex = ShaderManager::LoadVertexShader(vsFileName);
	assert(shader.vertex);
	shader.pixel = ShaderManager::LoadPixelShader(psFileName);
	assert(shader.pixel);
}

void Texture2D::CreateGraphicsPipeline() {
	PipelineManager::CreateRootSgnature(SRVHeap.GetParameter(), true);
	PipelineManager::SetShader(shader);
	PipelineManager::SetVertexInput("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT);
	PipelineManager::SetVertexInput("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);

	for (int32_t i = Pipeline::Blend::None; i < Pipeline::Blend::BlendTypeNum; i++) {
		PipelineManager::SetState(Pipeline::Blend(i), Pipeline::SolidState::Solid);
		graphicsPipelineState[i] = PipelineManager::Create();
	}

	PipelineManager::StateReset();
}

void Texture2D::LoadTexture(const std::string& fileName) {
	if (tex) {

	}

	if (!tex) {
		tex = TextureManager::GetInstance()->LoadTexture(fileName);

		SRVHeap.CreateTxtureView(tex);
	}
}

void Texture2D::Draw(
	const Vector2& scale,
	float rotate,
	const Vector2& pos,
	const Mat4x4& viewProjection, 
	Pipeline::Blend blend,
	const Vector2& pibot, const Vector2& size
) {
	ImGui::DragFloat("pibot", &(*colorPibot), 0.01f, 0.0f,1.0f);
	ImGui::DragInt("type", &(*colorType), 1.0f,0,3);

	const Vector2& uv0 = { pibot.x, pibot.y + size.y }; const Vector2& uv1 = size;
	const Vector2& uv2 = { pibot.x + size.x, pibot.y }; const Vector2& uv3 = pibot;
	
	VertexData pv[4] = {
		{ { -0.5f,  0.5f, 0.1f }, uv3 },
		{ {  0.5f,  0.5f, 0.1f }, uv2 },
		{ {  0.5f, -0.5f, 0.1f }, uv1 },
		{ { -0.5f, -0.5f, 0.1f }, uv0 },
	};

	VertexData* mappedData = nullptr;
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedData));
	for (int32_t i = 0; i < _countof(pv); i++) {
		mappedData[i] = pv[i];
	}
	vertexResource->Unmap(0, nullptr);

	*wvpMat = viewProjection * 
		VertMakeMatrixAffin(
		Vector3(scale.x * tex->getSize().x, scale.y * tex->getSize().y, 1.0f), 
		Vector3(0.0f, 0.0f, rotate), 
		Vector3(pos.x, pos.y, 0.01f)
	);

	ImGui::Begin("tex color");
	ImGui::DragFloat4("Color", color->m.data(), 0.01f, 0.0f, 1.0f);
	ImGui::End();

	auto commandlist = Engine::GetCommandList();

	// 各種描画コマンドを積む
	graphicsPipelineState[blend]->Use();
	SRVHeap.Use();
	commandlist->IASetVertexBuffers(0, 1, &vertexView);
	commandlist->IASetIndexBuffer(&indexView);
	commandlist->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Texture2D::Draw(
	const Vector2& scale,
	const Vector3& rotate,
	const Vector3& pos,
	const Mat4x4& viewProjection,
	Pipeline::Blend blend,
	const Vector2& pibot, const Vector2& size
) {
	ImGui::DragFloat("pibot", &(*colorPibot), 0.01f, 0.0f, 1.0f);
	ImGui::DragInt("type", &(*colorType), 1.0f, 0, 3);

	const Vector2& uv0 = { pibot.x, pibot.y + size.y }; const Vector2& uv1 = size;
	const Vector2& uv2 = { pibot.x + size.x, pibot.y }; const Vector2& uv3 = pibot;

	VertexData pv[4] = {
		{ { -0.5f,  0.5f, 0.1f }, uv3 },
		{ {  0.5f,  0.5f, 0.1f }, uv2 },
		{ {  0.5f, -0.5f, 0.1f }, uv1 },
		{ { -0.5f, -0.5f, 0.1f }, uv0 },
	};

	VertexData* mappedData = nullptr;
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedData));
	for (int32_t i = 0; i < _countof(pv); i++) {
		mappedData[i] = pv[i];
	}
	vertexResource->Unmap(0, nullptr);

	*wvpMat = viewProjection *
		VertMakeMatrixAffin(
			Vector3(scale.x * tex->getSize().x, scale.y * tex->getSize().y, 1.0f),
			rotate,
			pos
		);

	ImGui::Begin("tex color");
	ImGui::DragFloat4("Color", color->m.data(), 0.01f, 0.0f, 1.0f);
	ImGui::End();

	auto commandlist = Engine::GetCommandList();

	// 各種描画コマンドを積む
	graphicsPipelineState[blend]->Use();
	SRVHeap.Use();
	commandlist->IASetVertexBuffers(0, 1, &vertexView);
	commandlist->IASetIndexBuffer(&indexView);
	commandlist->DrawIndexedInstanced(6, 1, 0, 0, 0);
}