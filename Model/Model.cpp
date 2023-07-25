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

		std::vector<Vector3> normalPos(0);
		uint32_t vertPositionNum = 0;
		meshData.indexNum = 0;

		std::string lineBuf;

		while (std::getline(objFile, lineBuf)) {
			std::string identifier;
			std::istringstream line(lineBuf);
			line >> identifier;
			if (identifier == "v") {
				vertPositionNum++;
			}
			else if (identifier == "vn") {
				std::string buff;
				Vector3 posBuf;
				line >> posBuf.x >> posBuf.y >> posBuf.z;
				normalPos.push_back(posBuf);
			}
			else if (identifier == "vt") {

			}
			else if (identifier == "f") {
				std::string buff;
				while (getline(line, buff, ' '))
				{
					meshData.indexNum++;
				}
			}
			else if (identifier == "mtllib") {

			}
		}
		objFile.close();

		meshData.vertexBuffer = Engine::CreateBufferResuorce(sizeof(VertData) * vertPositionNum);
		assert(meshData.vertexBuffer);


		// リソースの先頭のアドレスから使う
		meshData.vertexView.BufferLocation = meshData.vertexBuffer->GetGPUVirtualAddress();
		// 使用するリソースのサイズは頂点3つ分のサイズ
		meshData.vertexView.SizeInBytes = sizeof(VertData) * vertPositionNum;
		// 1頂点当たりのサイズ
		meshData.vertexView.StrideInBytes = sizeof(VertData);

		// 頂点リソースにデータを書き込む
		meshData.vertexMap = nullptr;
		// 書き込むためのアドレスを取得
		meshData.vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&meshData.vertexMap));


		// indexBUffer生成
		meshData.indexBuffer = Engine::CreateBufferResuorce(sizeof(uint32_t) * meshData.indexNum);
		assert(meshData.indexBuffer);
		// リソースの先頭のアドレスから使う
		meshData.indexView.BufferLocation = meshData.indexBuffer->GetGPUVirtualAddress();
		meshData.indexView.SizeInBytes = sizeof(uint32_t) * meshData.indexNum;
		meshData.indexView.Format = DXGI_FORMAT_R32_UINT;

		meshData.indexMap = nullptr;
		meshData.indexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&meshData.indexMap));


		objFile.open(fileName);
		uint32_t i = 0;
		meshData.indexNum = 0;
		while (std::getline(objFile, lineBuf)) {
			std::string identifier;
			std::istringstream line(lineBuf);
			line >> identifier;
			if (identifier == "v") {
				Vector3 posBuf;
				line >> posBuf.x >> posBuf.y >> posBuf.z;
				meshData.vertexMap[i].position = { posBuf, 1.0f };
				i++;
				if (i >= vertPositionNum) {
					i = 0;
				}
			}
			else if (identifier == "vn") {
				
			}
			else if (identifier == "vt") {

			}
			else if (identifier == "f") {
				std::string buff;
				std::vector<float> posBuf(0);
				while (getline(line, buff, ' '))
				{
					/// 0:vertexNumber 1:textureCoordinate 2:NormalNumber
					std::string num[3];
					int32_t count = 0;
					if (std::any_of(buff.cbegin(), buff.cend(), isdigit)) {
						for (auto ch = buff.begin(); ch != buff.end(); ch++) {
							if (*ch == '/') {
								count++;
							}
							else { num[count] += *ch; }
						}
					}
					if (!num[0].empty()) {
						meshData.indexMap[meshData.indexNum] = std::stoi(num[0]) - 1;
						meshData.vertexMap[std::stoi(num[0]) - 1].normal = normalPos[std::stoi(num[2]) - 1];
						meshData.indexNum++;
					}
				}
			}
			else if (identifier == "mtllib") {
				
			}

			/*if (lineBuf.find("#") == std::string::npos && lineBuf.find(".mtl") == std::string::npos) {
				if (lineBuf.find("v") != std::string::npos && lineBuf.find("vn") == std::string::npos && lineBuf.find("vt") == std::string::npos) {
					std::string buff;
					std::vector<float> posBuf(0);
					while (getline(line, buff, ' '))
					{
						if (std::any_of(buff.cbegin(), buff.cend(), isdigit)) {
							posBuf.push_back(std::stof(buff));
						}
					}
					if (posBuf.size() == 3) {
						meshData.vertexMap[i].position = Vector4(posBuf[0], posBuf[1], posBuf[2], 1.0f);
						meshData.vertexMap[i].normal = Vector3(posBuf[0], posBuf[1], posBuf[2]).Normalize();
					}
					i++;
					if (i >= vertPositionNum) {
						i = 0;
					}
				}
				else if (lineBuf.find("f") != std::string::npos) {
					std::string buff;
					std::vector<float> posBuf(0);
					while (getline(line, buff, ' '))
					{
						/// 0:vertexNumber 1:textureCoordinate 2:NormalNumber
						std::string num[3];
						int32_t count = 0;
						if (std::any_of(buff.cbegin(), buff.cend(), isdigit)) {
							for (auto ch = buff.begin(); ch != buff.end(); ch++) {
								if (*ch == '/') {
									count++;
								}
								else { num[count] += *ch; }
							}
						}
						if (!num[0].empty()) {
							meshData.indexMap[meshData.indexNum] = static_cast<int16_t>(std::stoi(num[0]) - 1);
							meshData.vertexMap[std::stoi(num[0]) - 1].normal = normalPos[std::stoi(num[2]) - 1];
							meshData.indexNum++;
						}
					}
				}
			}*/
		}

		meshData.vertexBuffer->Unmap(0, nullptr);
		meshData.indexBuffer->Unmap(0, nullptr);

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
		PipelineManager::SetVertexInput("POSITION", 1u, DXGI_FORMAT_R32G32B32A32_FLOAT);

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
	
	commandlist->IASetVertexBuffers(0,1,&meshData.vertexView);
	commandlist->IASetIndexBuffer(&meshData.indexView);
	
	commandlist->DrawIndexedInstanced(meshData.indexNum, 1, 0, 0, 0);
}

Model::~Model() {
	if (meshData.indexBuffer) {
		meshData.indexBuffer->Release();
	}
	if (meshData.vertexBuffer) {
		meshData.vertexBuffer->Release();
	}
}