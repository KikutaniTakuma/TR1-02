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
	tex()
{
}

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

	SRVHeap.CreateConstBufferView(wvpMat);
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

	PipelineManager::SetState(Pipeline::Blend::None,Pipeline::SolidState::Solid);
	graphicsPipelineState[Pipeline::Blend::None] = PipelineManager::Create();

	PipelineManager::SetState(Pipeline::Blend::Noaml, Pipeline::SolidState::Solid);
	graphicsPipelineState[Pipeline::Blend::Noaml] = PipelineManager::Create();

	PipelineManager::SetState(Pipeline::Blend::Add, Pipeline::SolidState::Solid);
	graphicsPipelineState[Pipeline::Blend::Add] = PipelineManager::Create();

	PipelineManager::SetState(Pipeline::Blend::Sub, Pipeline::SolidState::Solid);
	graphicsPipelineState[Pipeline::Blend::Sub] = PipelineManager::Create();

	PipelineManager::SetState(Pipeline::Blend::Mul, Pipeline::SolidState::Solid);
	graphicsPipelineState[Pipeline::Blend::Mul] = PipelineManager::Create();

	PipelineManager::SetState(Pipeline::Blend::Transparent, Pipeline::SolidState::Solid);
	graphicsPipelineState[Pipeline::Blend::Transparent] = PipelineManager::Create();

	PipelineManager::StateReset();
}

void Texture2D::LoadTexture(const std::string& fileName) {
	// Load済みだったらUnload;
	if (tex) {
		TextureManager::GetInstance()->UnloadTexture(tex);
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
	const Vector2& uv0, const Vector2& uv1, 
	const Vector2& uv2, const Vector2& uv3
) {
	ImGui::DragFloat("pibot", &(*colorPibot), 0.01f, 0.0f,1.0f);
	ImGui::DragInt("type", &(*colorType), 1.0f,0,3);
	
	VertexData pv[4] = {
		{{-0.5f,  0.5f, 0.1f }, uv3},
		{{ 0.5f,  0.5f, 0.1f }, uv2},
		{{ 0.5f,  -0.5f, 0.1f }, uv1},
		{{-0.5f,  -0.5f, 0.1f }, uv0},
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

	auto commandlist = Engine::GetCommandList();

	// 各種描画コマンドを積む
	graphicsPipelineState[size_t(blend)]->Use();
	SRVHeap.Use();
	commandlist->IASetVertexBuffers(0, 1, &vertexView);
	commandlist->IASetIndexBuffer(&indexView);
	commandlist->DrawIndexedInstanced(6, 1, 0, 0, 0);
}