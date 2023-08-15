#include "SceneManager.h"
#include "externals/imgui/imgui.h"

SceneManager* SceneManager::GetInstance() {
	static SceneManager instance;
	return &instance;
}

void SceneManager::Update() {

}