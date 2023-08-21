#include "ModelManager.h"
#include <cassert>
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