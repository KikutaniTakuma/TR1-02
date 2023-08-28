#pragma once
#include <unordered_map>
#include <deque>
#include <Model/Model.h>
#include <initializer_list>

class ModelManager {
private:
	ModelManager();
	ModelManager(const ModelManager&) = delete;
	ModelManager(ModelManager&&) noexcept = delete;
	~ModelManager() = default;

	ModelManager& operator=(const ModelManager&) = delete;
	ModelManager& operator=(ModelManager&&) noexcept = delete;

public:
	static ModelManager* GetInstance();

	static void Initialize();
	static void Finalize();

	static void SetVertData(
		const std::string& vertName, 
		std::initializer_list<Model::VertData> vertData);

	/*static void LoadObj(const std::string fileName);
	static void LoadMtl(const std::string fileName);*/

	static std::unordered_map<std::string, std::deque<Model::VertData>>::iterator GetItr(const std::string& vertName);
	static bool IsExistence(
		std::unordered_map<std::string, std::deque<Model::VertData>>::iterator itr
	);

private:
	static ModelManager* instance;

private:
	std::unordered_map<std::string, std::deque<Model::VertData>> models;
	std::unordered_map<std::string, std::deque<std::string>> useMtl;
	decltype(models)::iterator itr;

	std::unordered_map<std::string, std::deque<std::string>> map_Kd;
	std::unordered_map<std::string, std::deque<std::string>> newmtl;
};