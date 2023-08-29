#include "Texture2D.h"
#include "Engine/ShaderManager/ShaderManager.h"
#include "externals/imgui/imgui.h"
#include "Engine/ErrorCheck/ErrorCheck.h"
#include <numeric>

Texture2D::Texture2D() :
	scale(Vector2::identity),
	rotate(),
	pos({ 0.0f,0.0f,0.01f }),
	uvPibot(),
	uvSize(Vector2::identity),
	SRVHeap(16),
	SRVHandle{},
	vertexView(),
	vertexResource(nullptr),
	indexView(),
	indexResource(nullptr),
	shader(),
	graphicsPipelineState(),
	tex(nullptr),
	isFirstLoad(true),
	isLoad(false),
	color(std::numeric_limits<uint32_t>::max())
{}

Texture2D::Texture2D(const Texture2D& right) :
	scale(Vector2::identity),
	rotate(),
	pos({ 0.0f,0.0f,0.01f }),
	uvPibot(),
	uvSize(Vector2::identity),
	SRVHeap(16),
	SRVHandle{},
	vertexView(),
	vertexResource(nullptr),
	indexView(),
	indexResource(nullptr),
	shader(),
	graphicsPipelineState(),
	tex(nullptr),
	isFirstLoad(true),
	isLoad(false),
	color(std::numeric_limits<uint32_t>::max())
{
	*this = right;
}
Texture2D::Texture2D(Texture2D&& right) noexcept :
	scale(Vector2::identity),
	rotate(),
	pos({ 0.0f,0.0f,0.01f }),
	uvPibot(),
	uvSize(Vector2::identity),
	SRVHeap(16),
	SRVHandle{},
	vertexView(),
	vertexResource(nullptr),
	indexView(),
	indexResource(nullptr),
	shader(),
	graphicsPipelineState(),
	tex(nullptr),
	isFirstLoad(true),
	isLoad(false),
	color(std::numeric_limits<uint32_t>::max())
{
	*this = right;
}

Texture2D::~Texture2D() {
	if (indexResource)indexResource->Release();
	if (vertexResource)vertexResource->Release();
}

Texture2D& Texture2D::operator=(const Texture2D& right) {
	scale = right.scale;
	rotate = right.rotate;
	pos = right.pos;

	uvPibot = right.uvPibot;
	uvSize = right.uvSize;

	worldPos = right.worldPos;

	SRVHeap.Reset();

	tex = nullptr;

	if (right.isLoad) {
		ThreadLoadTexture(right.tex->GetFileName());
		Initialize();
	}

	*wvpMat = *right.wvpMat;
	*colorBuf = *right.colorBuf;

	return *this;
}

void Texture2D::Initialize(const std::string& vsFileName, const std::string& psFileName) {
	LoadShader(vsFileName, psFileName);

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


	*colorBuf = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
}

void Texture2D::LoadShader(const std::string& vsFileName, const std::string& psFileName) {
	shader.vertex = ShaderManager::LoadVertexShader(vsFileName);
	assert(shader.vertex);
	shader.pixel = ShaderManager::LoadPixelShader(psFileName);
	assert(shader.pixel);
}

void Texture2D::CreateGraphicsPipeline() {
	auto paramaterTmp = SRVHeap.GetParameter();
	PipelineManager::CreateRootSgnature(&paramaterTmp, 1, true);
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
	tex = TextureManager::GetInstance()->LoadTexture(fileName);

	isLoad = false;
}

void Texture2D::ThreadLoadTexture(const std::string& fileName) {
	tex = nullptr;
	TextureManager::GetInstance()->LoadTexture(fileName, &tex);
	isLoad = false;
}

void Texture2D::Update() {
	if (tex && tex->CanUse() && !isLoad) {
		if (isFirstLoad) {
			SRVHeap.Reset();
			SRVHandle = SRVHeap.CreateTxtureView(tex);
			SRVHeap.CreateConstBufferView(wvpMat);
			SRVHeap.CreateConstBufferView(colorBuf);
			CreateGraphicsPipeline();
			isFirstLoad = false;
		}
		else {
			SRVHeap.CreateTxtureView(tex, SRVHandle);
		}
		isLoad = true;
	}

	if (tex && isLoad) {
		std::array<Vector3, 4> pv = {
			Vector3{ -0.5f,  0.5f, 0.1f },
			Vector3{  0.5f,  0.5f, 0.1f },
			Vector3{  0.5f, -0.5f, 0.1f },
			Vector3{ -0.5f, -0.5f, 0.1f },
		};

		std::copy(pv.begin(), pv.end(), worldPos.begin());
		auto&& worldMat =
			HoriMakeMatrixAffin(
				Vector3(scale.x * tex->getSize().x, scale.y * tex->getSize().y, 1.0f),
				rotate,
				pos
			);
		for (auto& i : worldPos) {
			i *= worldMat;
		}
	}
}

void Texture2D::Draw(
	const Mat4x4& viewProjection,
	Pipeline::Blend blend
) {
	if (tex && isLoad) {
		const Vector2& uv0 = { uvPibot.x, uvPibot.y + uvSize.y }; const Vector2& uv1 = uvSize + uvPibot;
		const Vector2& uv2 = { uvPibot.x + uvSize.x, uvPibot.y }; const Vector2& uv3 = uvPibot;

		std::array<VertexData, 4> pv = {
			worldPos[0], uv3,
			worldPos[1], uv2,
			worldPos[2], uv1,
			worldPos[3], uv0,
		};

		VertexData* mappedData = nullptr;
		vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedData));
		std::copy(pv.begin(), pv.end(), mappedData);
		vertexResource->Unmap(0, nullptr);

		*wvpMat = viewProjection;

		auto commandlist = Engine::GetCommandList();

		for (auto& i : graphicsPipelineState) {
			if (!i) {
				ErrorCheck::GetInstance()->ErrorTextBox("pipeline is nullptr", "Model");
				return;
			}
		}

		*colorBuf = UintToVector4(color);

		// 各種描画コマンドを積む
		graphicsPipelineState[blend]->Use();
		SRVHeap.Use();
		commandlist->IASetVertexBuffers(0, 1, &vertexView);
		commandlist->IASetIndexBuffer(&indexView);
		commandlist->DrawIndexedInstanced(6, 1, 0, 0, 0);
	}
}

void Texture2D::Debug(const std::string& guiName) {
	ImGui::Begin(guiName.c_str());
	ImGui::DragFloat2("scale", &scale.x, 1.0f);
	ImGui::DragFloat3("rotate", &rotate.x, 0.01f);
	ImGui::DragFloat3("pos", &pos.x, 1.0f);
	ImGui::DragFloat2("uvPibot", &uvPibot.x, 0.01f);
	ImGui::DragFloat2("uvSize", &uvSize.x, 0.01f);
	ImGui::ColorEdit4("SphereColor", &colorBuf->color.r);
	color = Vector4ToUint(*colorBuf);
	ImGui::End();
}

bool Texture2D::Colision(const Vector2& pos2D) {
	Vector2 max;
	Vector2 min;
	max.x = std::max_element(worldPos.begin(), worldPos.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.x < right.x;
		})->x;
	max.y = std::max_element(worldPos.begin(), worldPos.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.y < right.y;
		})->y;
	min.x = std::min_element(worldPos.begin(), worldPos.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.x < right.x;
		})->x;
	min.y = std::min_element(worldPos.begin(), worldPos.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.y < right.y;
		})->y;

	if (min.x < pos2D.x && pos2D.x < max.x) {
		if (min.y < pos2D.y && pos2D.y < max.y) {
			return true;
		}
	}

	return false;
}

bool Texture2D::Colision(const Texture2D& tex2D) {
	Vector3 max;
	Vector3 min;
	max.x = std::max_element(worldPos.begin(), worldPos.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.x < right.x;
		})->x;
	max.y = std::max_element(worldPos.begin(), worldPos.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.y < right.y;
		})->y;
	max.z = std::max_element(worldPos.begin(), worldPos.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.z < right.z;
		})->z;
	min.x = std::min_element(worldPos.begin(), worldPos.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.x < right.x;
		})->x;
	min.y = std::min_element(worldPos.begin(), worldPos.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.y < right.y;
		})->y;
	min.z = std::min_element(worldPos.begin(), worldPos.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.z < right.z;
		})->z;


	for (auto& i : tex2D.worldPos) {
		if (min.x < i.x && i.x < max.x) {
			if (min.y < i.y && i.y < max.y) {
				if (min.z < i.z && i.z < max.z) {
					return true;
				}
			}
		}
	}

	return false;
}