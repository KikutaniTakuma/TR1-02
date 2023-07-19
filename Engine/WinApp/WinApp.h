#pragma once

#include <Windows.h>
#include <stdint.h>
#include <string>

class WinApp {
private:
	WinApp();
	~WinApp();
	WinApp(const WinApp&) = delete;
	const WinApp& operator=(const WinApp&) = delete;

public:
	static inline WinApp* GetInstance() {
		static WinApp instance;
		return &instance;
	}

	static LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:
	void Create(const std::wstring& windowTitle, int32_t width, int32_t height);

	inline HWND GetHwnd() const {
		return hwnd;
	}

	inline const WNDCLASSEX& getWNDCLASSEX() const {
		return w;
	}

	inline std::wstring GetWindowClassName() const {
		return windowName;
	}

	void SetFullscreen(bool isFullscreen_);


private:
	HWND hwnd = nullptr;
	WNDCLASSEX w{};
	UINT windowStyle;
	bool isFullscreen = false;
	RECT windowRect;
	std::wstring windowName;
	float aspectRatio;
};