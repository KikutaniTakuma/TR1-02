#pragma once

#include <Windows.h>
#include <stdint.h>
#include <string>

class WinApp {
private:
	WinApp();
	~WinApp() = default;

public:
	static void Initalize();
	
	static void Finalize();

	static inline WinApp* GetInstance() {
		return instance;
	}

	static LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	static WinApp* instance;

public:
	void Create(const std::wstring& windowTitle, int32_t width, int32_t height);

	inline HWND GetHwnd() const {
		return hwnd;
	}


private:
	WNDCLASSEX w{};
	HWND hwnd{};
};