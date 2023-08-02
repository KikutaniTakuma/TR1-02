#include "GlobalVariables/GlobalVariables.h"
#include "externals/imgui/imgui.h"
#include "externals/nlohmann/json.hpp"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <Windows.h> 

GlobalVariables* GlobalVariables::GetInstance() {
	static GlobalVariables instance;
	return &instance;
}

void GlobalVariables::CreateGroup(const std::string& groupName) {
	datas[groupName];
}

void GlobalVariables::SetValue(const std::string& groupName, const std::string& key, int32_t value) {
	auto& gruop = datas[groupName];
	Item item = value;
	gruop[key] = value;
}
void GlobalVariables::SetValue(const std::string& groupName, const std::string& key, float value) {
	auto& gruop = datas[groupName];
	Item item = value;
	gruop[key] = value;
}
void GlobalVariables::SetValue(const std::string& groupName, const std::string& key, const Vector3& value) {
	auto& gruop = datas[groupName];
	Item item = value;
	gruop[key] = value;
}

void GlobalVariables::Update() {
	if (!ImGui::Begin(" Global Variables", nullptr, ImGuiWindowFlags_MenuBar)) {
		ImGui::End();
		return;
	}
	if (!ImGui::BeginMenuBar()) {
		return;
	}

	for (auto itrGroup = datas.begin(); itrGroup != datas.end(); itrGroup++) {
		const std::string& groupName = itrGroup->first;
		auto& group = itrGroup->second;

		if (!ImGui::BeginMenu(groupName.c_str())) {
			continue;
		}
		for (auto& itemitr : group) {
			const std::string& itemName = itemitr.first;

			auto& item = itemitr.second;

			if (std::holds_alternative<int32_t>(item)) {
				int32_t* ptr = std::get_if<int32_t>(&item);
				ImGui::SliderInt(itemName.c_str(), ptr, 0, 100);
			}
			else if (std::holds_alternative<float>(item)) {
				float* ptr = std::get_if<float>(&item);
				ImGui::SliderFloat(itemName.c_str(), ptr, -10.0f, 10.0f);
			}
			else if (std::holds_alternative<Vector3>(item)) {
				Vector3 ptr = std::get<Vector3>(item);
				ImGui::SliderFloat3(itemName.c_str(), &ptr.x, -10.0f, 10.0f);
			}
		}

		ImGui::Text("\n");

		if (ImGui::Button("Save")) {
			SaveFile(groupName);
			std::string message = std::format("{}.json saved", groupName);
			MessageBoxA(nullptr,message.c_str(), "GlobalVariables", 0);
		}

		ImGui::EndMenu();
	}

	ImGui::EndMenuBar();
	ImGui::End();
}

void GlobalVariables::SaveFile(const std::string& groupName) {
	auto itrGroup = datas.find(groupName);
	assert(itrGroup != datas.end());

	nlohmann::json root;

	root = nlohmann::json::object();

	root[groupName] = nlohmann::json::object();

	for (auto itemItr = itrGroup->second.begin(); itemItr != itrGroup->second.end(); itemItr++) {
		const std::string& itemName = itemItr->first;

		auto& item = itemItr->second;

		if (std::holds_alternative<int32_t>(item)) {
			root[groupName][itemName] = std::get<int32_t>(item);
		}
		else if (std::holds_alternative<float>(item)) {
			root[groupName][itemName] = std::get<float>(item);
		}
		else if (std::holds_alternative<Vector3>(item)) {
			auto tmp = std::get<Vector3>(item);
			root[groupName][itemName] = nlohmann::json::array({ tmp.x, tmp.y, tmp.z });
		}
	}

	const std::filesystem::path kDirectoryPath = "./AL_Resouce/ GlobalVariables/";

	if (!std::filesystem::exists(kDirectoryPath)) {
		std::filesystem::create_directory(kDirectoryPath);
	}

	auto filePath = kDirectoryPath.string() + groupName + ".json";

	std::ofstream file(filePath);

	if (file.fail()) {
		assert(!"fileSaveFailed");
		return;
	}

	file << std::setw(4) << root << std::endl;

	file.close();
}