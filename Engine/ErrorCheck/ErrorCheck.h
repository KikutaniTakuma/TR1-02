#pragma once
#include <string>

class ErrorCheck {
private:
	ErrorCheck();
	ErrorCheck(const ErrorCheck&) = delete;
	ErrorCheck(ErrorCheck&&) noexcept = delete;
	~ErrorCheck() = default;

	ErrorCheck& operator=(const ErrorCheck&) = delete;
	ErrorCheck& operator=(ErrorCheck&&) noexcept = delete;

public:
	static ErrorCheck* GetInstance();

public:
	void ErrorTextBox(const std::string& text, const std::string& boxName = "Error");
	inline bool GetError() const {
		return isError;
	}

private:
	void ErrorLog(const std::string& text);

private:
	bool isError;
};