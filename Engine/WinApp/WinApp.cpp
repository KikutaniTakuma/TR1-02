#include "WinApp.h"
#include <cassert>
#include "externals/imgui/imgui_impl_win32.h"
#include <string>

extern IMGUI_IMPL_API LRESULT
ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

WinApp::WinApp():
	w({}),
	hwnd()
{}

WinApp* WinApp::instance = nullptr;

void WinApp::Initalize() {
	instance = new WinApp();
	assert(instance);
}

void WinApp::Finalize() {
	// ウィンドウクラスを登録解除
	UnregisterClass(instance->w.lpszClassName, instance->w.hInstance);

	// instanceを解放
	delete instance;
	instance = nullptr;
}

LRESULT WinApp::WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	assert(SUCCEEDED(CoInitializeEx(0, COINIT_MULTITHREADED)));

	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}

	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

void WinApp::Create(const std::wstring& windowTitle, int32_t width, int32_t height) {
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = WindowProcedure;
	w.lpszClassName = windowTitle.c_str();
	w.hInstance = GetModuleHandle(nullptr);

	RegisterClassEx(&w);

	RECT wrc = { 0,0,width, height };

	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	hwnd = CreateWindow(
		w.lpszClassName,
		windowTitle.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wrc.right - wrc.left,
		wrc.bottom - wrc.top,
		nullptr,
		nullptr,
		w.hInstance,
		nullptr
	);

	ShowWindow(hwnd, SW_SHOW);
}