#pragma once
#include <unordered_map>
#include <variant>
#include <string>
#include "Math/Vector3/Vector3.h"

class GlobalVariables {
public:
	using Item = std::variant<int32_t, float, Vector3>;
	using Group = std::unordered_map<std::string, Item>;

private:
	GlobalVariables() = default;
	GlobalVariables(const GlobalVariables&) = delete;
	GlobalVariables(GlobalVariables&&) noexcept = delete;
	~GlobalVariables() = default;

	GlobalVariables& operator=(const GlobalVariables&) = delete;
	GlobalVariables& operator=(GlobalVariables&&) noexcept = delete;

public:
	static GlobalVariables* GetInstance();

public:
	void CreateGroup(const std::string& groupName);

	void SetValue(const std::string& groupName, const std::string& key, int32_t value);
	void SetValue(const std::string& groupName, const std::string& key, float value);
	void SetValue(const std::string& groupName, const std::string& key, const Vector3& value);

	void Update();

	void SaveFile(const std::string& groupName);

private:
	std::unordered_map<std::string, Group> datas;
};