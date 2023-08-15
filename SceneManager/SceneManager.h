#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include "Scene/Scene.h"

class SceneManager {
private:
	SceneManager() = default;
	SceneManager(const SceneManager&) = delete;
	SceneManager(SceneManager&&) = delete;
	~SceneManager() = default;

	SceneManager& operator=(const SceneManager&) = delete;
	SceneManager& operator=(SceneManager&&) = delete;

public:
	static SceneManager* GetInstance();

public:
	void Update();

private:
	std::unordered_map<std::string, Scene> scenes;
};