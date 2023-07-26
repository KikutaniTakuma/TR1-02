#include "Model.h"
#include "Engine/Engine.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <numbers>
#include <filesystem>
#include "Engine/ConvertString/ConvertString.h"
#include "Engine/ShaderManager/ShaderManager.h"


Model::Model() :
	pos(),
	rotate(),
	scale(Vector3::identity),
	color(0xffffffff),
	meshData(),
	shader(),
	pipeline(nullptr),
	loadObjFlg(false),
	loadShaderFlg(false),
	createGPFlg(false),
	wvpData(),
	dirLig(),
	colorBuf(),
	descHeap(16),
	tex(nullptr)
{
	// 単位行列を書き込んでおく
	wvpData->worldMat = MakeMatrixIndentity();
	wvpData->viewProjectoionMat = MakeMatrixIndentity();

	dirLig->ligDirection = { 1.0f,-1.0f,-1.0f };
	dirLig->ligDirection = dirLig->ligDirection.Normalize();
	Vector4 colorTmp = UintToVector4(0xffffadff);
	dirLig->ligColor = colorTmp.GetVector3();

	dirLig->ptPos = { 5.0f,5.0f,5.0f };
	dirLig->ptColor = { 15.0f,15.0f,15.0f };
	dirLig->ptRange = 10.0f;

	*colorBuf = UintToVector4(color);

	descHeap.CreateConstBufferView(wvpData);
	descHeap.CreateConstBufferView(dirLig);
	descHeap.CreateConstBufferView(colorBuf);
}

void Model::LoadObj(const std::string& fileName) {
	if (!loadObjFlg) {
		std::ifstream objFile(fileName);
		assert(objFile);

		std::vector<Vector4> posDatas(0);

		std::vector<Vector3> normalDatas(0);

		std::vector<Vector2> uvDatas(0);

		std::vector<IndexData> indexDatas(0);

		std::string lineBuf;

		while (std::getline(objFile, lineBuf)) {
			std::string identifier;
			std::istringstream line(lineBuf);
			line >> identifier;
			if (identifier == "v") {
				Vector4 buf;
				line >> buf.vec.x >> buf.vec.y >> buf.vec.z;
				buf.vec.x *= -1.0f;
				buf.vec.w = 1.0f;

				posDatas.push_back(buf);
			}
			else if (identifier == "vn") {
				Vector3 buf;
				line >> buf.x >> buf.y >> buf.z;
				buf.x *= -1.0f;
				normalDatas.push_back(buf);
			}
			else if (identifier == "vt") {
				Vector2 buf;
				line >> buf.x >> buf.y;
				buf.y = 1.0f - buf.y;
				uvDatas.push_back(buf);
			}
			else if (identifier == "f") {
				std::string buf;
				std::vector<float> posBuf(0);
				std::array<IndexData, 3> indcoes;
				auto idnexItr = indcoes.rbegin();
				while (std::getline(line, buf, ' '))
				{
					/// 0:vertexNumber 1:textureCoordinate 2:NormalNumber
					std::string num[3];
					int32_t count = 0;
					if (std::any_of(buf.cbegin(), buf.cend(), isdigit)) {
						for (auto ch = buf.begin(); ch != buf.end(); ch++) {
							if (*ch == '/') {
								count++;
							}
							else { num[count] += *ch; }
						}
					}
					if (!num[0].empty()) {
						idnexItr->vertNum = static_cast<uint32_t>(std::stoi(num[0]) - 1);
						idnexItr->uvNum = static_cast<uint32_t>(std::stoi(num[1]) - 1);
						idnexItr->normalNum = static_cast<uint32_t>(std::stoi(num[2]) - 1);
						idnexItr++;
					}
				}
				for (auto& i : indcoes) {
					indexDatas.push_back(i);
				}
			}
			else if (identifier == "mtllib") {
				std::string mtlFileName;
				std::filesystem::path path = fileName;

				line >> mtlFileName;

				LoadMtl(path.parent_path().string() + "/" + mtlFileName);
			}
		}
		objFile.close();


		meshData.vertexBuffer = Engine::CreateBufferResuorce(sizeof(VertData) * indexDatas.size());
		assert(meshData.vertexBuffer);


		// リソースの先頭のアドレスから使う
		meshData.vertexView.BufferLocation = meshData.vertexBuffer->GetGPUVirtualAddress();
		// 使用するリソースのサイズは頂点3つ分のサイズ
		meshData.vertexView.SizeInBytes = static_cast<UINT>(sizeof(VertData) * indexDatas.size());
		// 1頂点当たりのサイズ
		meshData.vertexView.StrideInBytes = sizeof(VertData);

		// 頂点リソースにデータを書き込む
		meshData.vertexMap = nullptr;
		// 書き込むためのアドレスを取得
		meshData.vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&meshData.vertexMap));

		for (int32_t i = 0; i < indexDatas.size(); i++) {
			meshData.vertexMap[i].position = posDatas[indexDatas[i].vertNum];
			meshData.vertexMap[i].normal = normalDatas[indexDatas[i].normalNum];
			meshData.vertexMap[i].uv = uvDatas[indexDatas[i].uvNum];
		}


		meshData.vertNum = static_cast<uint32_t>(indexDatas.size());

		meshData.vertexBuffer->Unmap(0, nullptr);

		loadObjFlg = true;
	}
}

void Model::LoadMtl(const std::string fileName) {
	std::ifstream file(fileName);
	assert(file);

	std::string lineBuf;
	while (std::getline(file, lineBuf)) {
		std::string identifier;
		std::istringstream line(lineBuf);
		line >> identifier;
		if (identifier == "map_Kd") {
			std::string texName;
			std::filesystem::path path = fileName;

			line >> texName;

			tex = TextureManager::GetInstance()->LoadTexture(path.parent_path().string() + "/" + texName);

			descHeap.CreateTxtureView(tex);
		}
	}
}

void Model::LoadShader(
	const std::string& vertex,
	const std::string& pixel,
	const std::string& geometory,
	const std::string& hull,
	const std::string& domain
) {
	if (!loadShaderFlg) {
		shader.vertex = ShaderManager::LoadVertexShader(vertex);
		shader.pixel = ShaderManager::LoadPixelShader(pixel);
		if (geometory.size() != 0LLU) {
			shader.geometory = ShaderManager::LoadGeometoryShader(geometory);
		}
		if (hull.size() != 0LLU && geometory.size() != 0LLU) {
			shader.hull = ShaderManager::LoadHullShader(hull);
			shader.domain = ShaderManager::LoadHullShader(domain);
		}
		loadShaderFlg = true;
	}
}

void Model::CreateGraphicsPipeline() {
	if (loadShaderFlg && loadObjFlg) {
		PipelineManager::CreateRootSgnature(descHeap.GetParameter(), !!tex);

		PipelineManager::SetShader(shader);
		
		PipelineManager::SetVertexInput("POSITION", 0u, DXGI_FORMAT_R32G32B32A32_FLOAT);
		PipelineManager::SetVertexInput("NORMAL", 0u, DXGI_FORMAT_R32G32B32_FLOAT);
		PipelineManager::SetVertexInput("TEXCOORD", 0u, DXGI_FORMAT_R32G32_FLOAT);

		PipelineManager::SetState(Pipeline::Blend::None,  Pipeline::SolidState::Solid);

		pipeline = PipelineManager::Create();

		PipelineManager::StateReset();

		createGPFlg = true;
	}
}

void Model::Update() {
	
}

void Model::Draw(const Mat4x4& viewProjectionMat, const Vector3& cameraPos) {
	assert(createGPFlg);
	
	wvpData->worldMat.VertAffin(scale, rotate, pos);
	wvpData->viewProjectoionMat = viewProjectionMat;

	*colorBuf = UintToVector4(color);

	dirLig->eyePos = cameraPos;

	auto commandlist = Engine::GetCommandList();
	pipeline->Use();
	descHeap.Use();
	
	commandlist->IASetVertexBuffers(0, 1, &meshData.vertexView);
	
	commandlist->DrawInstanced(meshData.vertNum, 1,  0, 0);
}

Model::~Model() {
	if (meshData.vertexBuffer) {
		meshData.vertexBuffer->Release();
	}
}