#include "ErrorCheck.h"
#include <fstream>
#include <filesystem>
#include <chrono>
#include <format>
#include <cassert>
#include <Windows.h>
#include "Engine/WinApp/WinApp.h"

ErrorCheck* ErrorCheck::GetInstance() {
	static ErrorCheck instance;
	return &instance;
}

ErrorCheck::ErrorCheck() :
	isError(false)
{
	std::filesystem::path directoryPath = "./ErrorLog/";
	if (!std::filesystem::exists(directoryPath)) {
		std::filesystem::create_directory(directoryPath);
	}
	std::ofstream file(directoryPath.string() + "ErrorLog.txt", std::ios::app);
	assert(file);

	auto now = std::chrono::system_clock::now();
	auto nowSec = std::chrono::floor<std::chrono::seconds>(now);
	std::chrono::zoned_time zt{"Asia/Tokyo", nowSec};

	file << std::endl << std::format("{:%Y/%m/%d %H:%M:%S %Z} : {}", zt, "ErrorCheck Start") << std::endl;
}

ErrorCheck::~ErrorCheck() {
	if (isError) {
		std::filesystem::path directoryPath = "./ErrorLog/";
		if (!std::filesystem::exists(directoryPath)) {
			std::filesystem::create_directory(directoryPath);
		}
		std::ofstream file(directoryPath.string() + "ErrorLog.txt", std::ios::app);
		assert(file);

		auto now = std::chrono::system_clock::now();
		auto nowSec = std::chrono::floor<std::chrono::seconds>(now);
		std::chrono::zoned_time zt{"Asia/Tokyo", nowSec};

		file << std::format("{:%Y/%m/%d %H:%M:%S %Z} : {}", zt, "There was Error!!!!") << std::endl;
	}
	else {
		std::filesystem::path directoryPath = "./ErrorLog/";
		if (!std::filesystem::exists(directoryPath)) {
			std::filesystem::create_directory(directoryPath);
		}
		std::ofstream file(directoryPath.string() + "ErrorLog.txt", std::ios::app);
		assert(file);

		auto now = std::chrono::system_clock::now();
		auto nowSec = std::chrono::floor<std::chrono::seconds>(now);
		std::chrono::zoned_time zt{"Asia/Tokyo", nowSec};

		file << std::format("{:%Y/%m/%d %H:%M:%S %Z} : {}", zt, "ErrorCheck End") << std::endl;
	}
}

void ErrorCheck::ErrorTextBox(const std::string& text, const std::string& boxName) {
	ErrorLog(text, boxName);

	if (boxName == "Error") {
		MessageBoxA(
			WinApp::GetInstance()->GetHwnd(), 
			text.c_str(), boxName.c_str(), 
			MB_OK | MB_SYSTEMMODAL | MB_ICONERROR
		);
	}
	else {
		MessageBoxA(
			WinApp::GetInstance()->GetHwnd(), 
			text.c_str(), std::string("Error : " + boxName).c_str(), 
			MB_OK | MB_SYSTEMMODAL| MB_ICONERROR
		);
	}
	isError = true;
}

void ErrorCheck::ErrorLog(const std::string& text, const std::string& boxName) {
	std::filesystem::path directoryPath = "./ErrorLog/";
	if (!std::filesystem::exists(directoryPath)) {
		std::filesystem::create_directory(directoryPath);
	}

	std::ofstream file(directoryPath.string() + "ErrorLog.txt", std::ios::app);
	assert(file);

	auto now = std::chrono::system_clock::now();
	auto nowSec = std::chrono::floor<std::chrono::seconds>(now);
	std::chrono::zoned_time zt{"Asia/Tokyo", nowSec};

	file << std::format("{:%Y/%m/%d %H:%M:%S} : {} / {}", zt, boxName, text) << std::endl;
	file.close();
}