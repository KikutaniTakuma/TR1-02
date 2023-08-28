#include "ModelManager.h"
#include <cassert>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "Engine/ErrorCheck/ErrorCheck.h"

ModelManager* ModelManager::instance = nullptr;

ModelManager* ModelManager::GetInstance() {
	return instance;
}


void ModelManager::Initialize() {
	instance = new ModelManager;
	assert(instance);
	if (!instance) {
		ErrorCheck::GetInstance()->ErrorTextBox("Initialize() : instance failed", "ModelManager");
	}
}
void ModelManager::Finalize() {
	delete instance;
	instance = nullptr;
}

void ModelManager::SetVertData(
	const std::string& vertName,
	std::initializer_list<Model::VertData> vertData) 
{
	instance->models[vertName];
	instance->models[vertName].resize(vertData.size());
	std::copy(vertData.begin(), vertData.end(), instance->models[vertName].begin());
}

std::unordered_map<std::string, std::deque<Model::VertData>>::iterator 
	ModelManager::GetItr(const std::string& vertName) 
{
	instance->itr = instance->models.find(vertName);
	return instance->itr;
}

bool ModelManager::IsExistence(
	std::unordered_map<std::string, std::deque<Model::VertData>>::iterator itr
) {
	return instance->itr != instance->models.end();
}

//void ModelManager::LoadObj(const std::string fileName) {
//	std::ifstream objFile(fileName);
//	assert(objFile);
//	if (!objFile) {
//		ErrorCheck::GetInstance()->ErrorTextBox("LoadObj() : Not found objFile : " + fileName, "Model");
//		return;
//	}
//
//	std::vector<Vector4> posDatas(0);
//
//	std::vector<Vector3> normalDatas(0);
//
//	std::vector<Vector2> uvDatas(0);
//
//	std::unordered_map<std::string, std::vector<Model::IndexData>> indexDatas(0);
//	std::unordered_map<std::string, std::vector<Model::IndexData>>::iterator indicesItr;
//
//	std::string lineBuf;
//
//	while (std::getline(objFile, lineBuf)) {
//		std::string identifier;
//		std::istringstream line(lineBuf);
//		line >> identifier;
//		if (identifier == "v") {
//			Vector4 buf;
//			line >> buf.vec.x >> buf.vec.y >> buf.vec.z;
//			buf.vec.x *= -1.0f;
//			buf.vec.w = 1.0f;
//
//			posDatas.push_back(buf);
//		}
//		else if (identifier == "vn") {
//			Vector3 buf;
//			line >> buf.x >> buf.y >> buf.z;
//			buf.x *= -1.0f;
//			normalDatas.push_back(buf);
//		}
//		else if (identifier == "vt") {
//			Vector2 buf;
//			line >> buf.x >> buf.y;
//			buf.y = 1.0f - buf.y;
//			uvDatas.push_back(buf);
//		}
//		else if (identifier == "f") {
//			std::string buf;
//			std::vector<float> posBuf(0);
//			std::array<Model::IndexData, 3> indcoes;
//			auto idnexItr = indcoes.rbegin();
//			while (std::getline(line, buf, ' '))
//			{
//				/// 0:vertexNumber 1:textureCoordinate 2:NormalNumber
//				std::string num[3];
//				int32_t count = 0;
//				if (std::any_of(buf.cbegin(), buf.cend(), isdigit)) {
//					for (auto& ch : buf) {
//						if (ch == '/') {
//							count++;
//						}
//						else { num[count] += ch; }
//					}
//				}
//
//				// エラーチェック
//				if (idnexItr == indcoes.rend()) {
//					//assert(!"Obj Load Error : Cannot Load Rectangles or more");
//					ErrorCheck::GetInstance()->ErrorTextBox("LoadObj() : Not supported for rectangles or more", "Model");
//					objFile.close();
//					return;
//				}
//
//				if (count == 2) {
//					idnexItr->vertNum = static_cast<uint32_t>(std::stoi(num[0]) - 1);
//					idnexItr->uvNum = static_cast<uint32_t>(std::stoi(num[1]) - 1);
//					idnexItr->normalNum = static_cast<uint32_t>(std::stoi(num[2]) - 1);
//					idnexItr++;
//				}
//				else if (count == 1) {
//					idnexItr->vertNum = static_cast<uint32_t>(std::stoi(num[0]) - 1);
//					idnexItr->normalNum = static_cast<uint32_t>(std::stoi(num[1]) - 1);
//					idnexItr++;
//				}
//			}
//			for (auto& i : indcoes) {
//				indicesItr->second.push_back(i);
//			}
//		}
//		else if (identifier == "usemtl") {
//			std::string useMtlName;
//			line >> useMtlName;
//			indexDatas.insert({ useMtlName,std::vector<Model::IndexData>(0) });
//			indicesItr = indexDatas.find(useMtlName);
//		}
//		else if (identifier == "mtllib") {
//			std::string mtlFileName;
//			std::filesystem::path path = fileName;
//
//			line >> mtlFileName;
//
//			LoadMtl(path.parent_path().string() + "/" + mtlFileName);
//		}
//	}
//
//	instance->useMtl[fileName];
//	for (auto i : indexDatas) {
//		instance->useMtl[fileName].push_back(i.first);
//
//		// キー値を生成
//		std::string modelsKeyName = fileName + i.first;
//		// modelsに追加
//		instance->models[modelsKeyName];
//		// 追加したものをresize
//		instance->models[i.first].resize(indexDatas[i.first].size());
//
//		for (int32_t j = 0; j < indexDatas[i.first].size(); j++) {
//			instance->models[i.first][j].position = posDatas[indexDatas[i.first][j].vertNum];
//			instance->models[i.first][j].normal = normalDatas[indexDatas[i.first][j].normalNum];
//			if (!uvDatas.empty()) {
//				instance->models[i.first][j].uv = uvDatas[indexDatas[i.first][j].uvNum];
//			}
//		}
//	}
//	objFile.close();
//}
//
//void ModelManager::LoadMtl(const std::string fileName){
//	std::ifstream file(fileName);
//	assert(file);
//	if (!file) { ErrorCheck::GetInstance()->ErrorTextBox("LoadMtl() : Not Found mtlFile", "Model"); }
//
//	std::string lineBuf;
//	
//	instance->map_Kd[fileName];
//	instance->newmtl[fileName];
//
//	std::string useMtlName;
//	while (std::getline(file, lineBuf)) {
//		std::string identifier;
//		std::istringstream line(lineBuf);
//
//		line >> identifier;
//		if (identifier == "map_Kd") {
//			std::string texName;
//			std::filesystem::path path = fileName;
//
//			line >> texName;
//
//			instance->map_Kd[fileName].push_back(texName);
//			texItr->second = TextureManager::GetInstance()->LoadTexture(path.parent_path().string() + "/" + texName);
//			hepaItr->second.CreateTxtureView(texItr->second);
//		}
//		else if (identifier == "newmtl") {
//			line >> useMtlName;
//			tex.insert({ useMtlName, nullptr });
//			texItr = tex.find(useMtlName);
//			SRVHeap.insert({ useMtlName , ShaderResourceHeap() });
//			hepaItr = SRVHeap.find(useMtlName);
//			hepaItr->second.InitializeReset(16);
//		}
//	}
//}