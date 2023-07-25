#include "Model.h"
#include "Engine/Engine.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <numbers>
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
	waveCountSpd(0.01f),
	wvpData(),
	dirLig(),
	colorBuf(),
	descHeap(16)
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
		//uint32_t vertPositionNum = 0;
		meshData.indexNum = 0;

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
				buf.vec.w = 1.0f;

				posDatas.push_back(buf);
			}
			else if (identifier == "vn") {
				Vector3 buf;
				line >> buf.x >> buf.y >> buf.z;
				normalDatas.push_back(buf);
			}
			else if (identifier == "vt") {
				Vector2 buf;
				line >> buf.x >> buf.y;
				uvDatas.push_back(buf);
			}
			else if (identifier == "f") {
				std::string buf;
				std::vector<float> posBuf(0);
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
						IndexData index;

						index.vertNum = static_cast<uint32_t>(std::stoi(num[0]) - 1);
						index.texNum = static_cast<uint32_t>(std::stoi(num[1]) - 1);
						index.normalNum = static_cast<uint32_t>(std::stoi(num[2]) - 1);

						indexDatas.push_back(index);

						meshData.indexNum++;
					}
				}
			}
			else if (identifier == "mtllib") {

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

		for (size_t i = 0; i < indexDatas.size(); i++) {
			meshData.vertexMap[i].position = posDatas[indexDatas[i].vertNum];
			meshData.vertexMap[i].normal = normalDatas[indexDatas[i].normalNum];
			meshData.vertexMap[i].uv = uvDatas[indexDatas[i].texNum];
			meshData.vertexMap[i].pad = 0.0f;
		}

		meshData.vertNum = static_cast<uint32_t>(indexDatas.size());

		//// indexBUffer生成
		//meshData.indexBuffer = Engine::CreateBufferResuorce(sizeof(uint32_t) * meshData.indexNum);
		//assert(meshData.indexBuffer);
		//// リソースの先頭のアドレスから使う
		//meshData.indexView.BufferLocation = meshData.indexBuffer->GetGPUVirtualAddress();
		//meshData.indexView.SizeInBytes = sizeof(uint32_t) * meshData.indexNum;
		//meshData.indexView.Format = DXGI_FORMAT_R32_UINT;

		//meshData.indexMap = nullptr;
		//meshData.indexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&meshData.indexMap));

		meshData.vertexBuffer->Unmap(0, nullptr);
		//meshData.indexBuffer->Unmap(0, nullptr);

		loadObjFlg = true;
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
		PipelineManager::CreateRootSgnature(descHeap.GetParameter(), false);

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
	//commandlist->IASetIndexBuffer(&meshData.indexView);
	
	//commandlist->DrawIndexedInstanced(meshData.indexNum, 1, 0, 0, 0);
	commandlist->DrawInstanced(meshData.vertNum, 1,  0, 0);
}

Model::~Model() {
	if (meshData.indexBuffer) {
		meshData.indexBuffer->Release();
	}
	if (meshData.vertexBuffer) {
		meshData.vertexBuffer->Release();
	}
}