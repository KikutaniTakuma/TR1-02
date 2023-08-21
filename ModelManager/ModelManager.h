#pragma once
#include <unordered_map>

class ModelManager {
private:
	ModelManager();
	ModelManager(const ModelManager&) = delete;
	ModelManager(ModelManager&&) noexcept = delete;
	~ModelManager();

	ModelManager& operator=(const ModelManager&) = delete;
	ModelManager& operator=(ModelManager&&) noexcept = delete;

public:
	static ModelManager* GetInstance();

	static void Initialize();
	static void Finalize();

private:
	static ModelManager* instance;
};