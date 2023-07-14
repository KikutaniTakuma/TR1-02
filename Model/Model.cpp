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
	meshData(),
	vertexShaderBlob(nullptr),
	pixelShaderBlob(nullptr),
	rootSignature(),
	pipeline(),
	loadObjFlg(false),
	loadShaderFlg(false),
	createGPFlg(false),
	waveCountSpd(0.01f),
	wvpData(),
	dirLig(),
	color(),
	descHeap(16)
{
	// 単位行列を書き込んでおく
	wvpData->worldMat = MakeMatrixIndentity();
	wvpData->viewProjectoionMat = MakeMatrixIndentity();

	dirLig->ligDirection = { 1.0f,-1.0f,-1.0f };
	dirLig->ligDirection = dirLig->ligDirection.Normalize();
	Vector4 colorTmp = Engine::UintToVector4(0xffffadff);
	dirLig->ligColor = colorTmp.GetVector3();

	dirLig->ptPos = { 5.0f,5.0f,5.0f };
	dirLig->ptColor = { 15.0f,15.0f,15.0f };
	dirLig->ptRange = 10.0f;

	*color = Engine::UintToVector4(0xff0000ff);
	color.OffWright();

	descHeap.CreateConstBufferView(wvpData);
	descHeap.CreateConstBufferView(dirLig);
	descHeap.CreateConstBufferView(color);
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
			std::istringstream line(lineBuf);
			if (lineBuf.find("#") == std::string::npos && lineBuf.find(".mtl") == std::string::npos) {
				if (lineBuf.find("f") != std::string::npos) {
					std::string buff;
					while (getline(line, buff, ' '))
					{
						meshData.indexNum++;
					}
				}
				else if (lineBuf.find("vn") != std::string::npos) {
					std::string buff;
					std::vector<float> posBuf(0);
					while (getline(line, buff, ' '))
					{
						if (std::any_of(buff.begin(), buff.end(), isdigit)) {
							posBuf.push_back(std::stof(buff));
						}
					}
					if (posBuf.size() == 3) {
						normalPos.push_back({ posBuf[0], posBuf[1], posBuf[2]});
					}

				}
				else if (lineBuf.find("v") != std::string::npos && lineBuf.find("vn") == std::string::npos && lineBuf.find("vt") == std::string::npos) {
					vertPositionNum++;
				}
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
		meshData.indexBuffer = Engine::CreateBufferResuorce(sizeof(uint16_t) * meshData.indexNum);
		assert(meshData.indexBuffer);
		// リソースの先頭のアドレスから使う
		meshData.indexView.BufferLocation = meshData.indexBuffer->GetGPUVirtualAddress();
		meshData.indexView.SizeInBytes = sizeof(uint16_t) * meshData.indexNum;
		meshData.indexView.Format = DXGI_FORMAT_R16_UINT;

		meshData.indexMap = nullptr;
		meshData.indexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&meshData.indexMap));


		objFile.open(fileName);
		uint32_t i = 0;
		meshData.indexNum = 0;
		while (std::getline(objFile, lineBuf)) {
			std::istringstream line(lineBuf);
			if (lineBuf.find("#") == std::string::npos && lineBuf.find(".mtl") == std::string::npos) {
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
			}
		}

		meshData.vertexBuffer->Unmap(0, nullptr);
		meshData.indexBuffer->Unmap(0, nullptr);

		loadObjFlg = true;
	}
}

void Model::LoadShader(const std::string& vertexFileName, const std::string& pixelFileName, const std::string& geometoryFileName) {
	if (!loadShaderFlg) {
		vertexShaderBlob = ShaderManager::GetInstance()->LoadVertexShader(vertexFileName);
		assert(vertexShaderBlob != nullptr);
		pixelShaderBlob = ShaderManager::GetInstance()->LoadPixelShader(pixelFileName);
		assert(pixelShaderBlob != nullptr);
		geometoryShaderBlob = ShaderManager::GetInstance()->LoadGeometoryShader(geometoryFileName);
		assert(geometoryShaderBlob != nullptr);
	/*	hullShaderBlob = Engine::CompilerShader(ConvertString("WaveShader/Wave.HS.hlsl"), L"hs_6_0");
		assert(hullShaderBlob != nullptr);
		domainShaderBlob = Engine::CompilerShader(ConvertString("WaveShader/Wave.DS.hlsl"), L"ds_6_0");
		assert(domainShaderBlob != nullptr);*/

		loadShaderFlg = true;
	}
}

void Model::CreateGraphicsPipeline() {
	if (loadShaderFlg && loadObjFlg) {
		rootSignature.Cretate(descHeap.GetParameter(), false);
		
		pipeline.SetShader(vertexShaderBlob, pixelShaderBlob, geometoryShaderBlob);
		pipeline.SetVertexInput("POSITION", 0u, DXGI_FORMAT_R32G32B32A32_FLOAT);
		pipeline.SetVertexInput("NORMAL", 0u, DXGI_FORMAT_R32G32B32_FLOAT);
		pipeline.SetVertexInput("POSITION", 1u, DXGI_FORMAT_R32G32B32A32_FLOAT);

		pipeline.Create(rootSignature.get(), Pipeline::Blend::None, Pipeline::CullMode::Back, Pipeline::SolidState::Solid);

		createGPFlg = true;
	}
}

void Model::Update() {
	
}

void Model::Draw(const Mat4x4& worldMat, const Mat4x4& viewProjectionMat, const Vector3& cameraPos) {
	if (!createGPFlg) {
		CreateGraphicsPipeline();
	}
	assert(createGPFlg);

	wvpData->worldMat = worldMat;
	wvpData->viewProjectoionMat = viewProjectionMat;

	dirLig->eyePos = cameraPos;

	auto commandlist = Engine::GetCommandList();
	pipeline.Use();
	descHeap.Use();
	
	//commandlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	commandlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
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