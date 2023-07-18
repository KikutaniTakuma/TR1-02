#pragma once
#include <string>
#include "Math/Vector3/Vector3.h"
#include "Math/Mat4x4/Mat4x4.h"
#include "Math/Vector2/Vector2.h"
#include "Math/Vector4/Vector4.h"
#include "Engine/ConvertString/ConvertString.h"


class StringOut {
public:
	StringOut();
	StringOut(const StringOut& right);
	StringOut(StringOut&& right) noexcept;
	StringOut(const std::string& formatName);
	StringOut(const std::wstring& formatName);
	~StringOut() = default;

public:
	StringOut& operator=(const StringOut& right);
	StringOut& operator=(StringOut&& right) noexcept;

public:
	void Draw();

	void WideDraw();

private:
	std::string format;
public:
	std::string str;
	std::wstring wstr;

	Vector2 pos;
	float rotation = 0.0f;
	Vector2 scale = Vector2::identity;
	uint32_t color = 0xffffffff;
	bool isHorizontal = false;

public:
	inline void SetFormat(const std::string& formatName) {
		format = formatName;
	}
	inline void SetFormat(const std::wstring& formatName) {
		format = ConvertString(formatName);
	}
};