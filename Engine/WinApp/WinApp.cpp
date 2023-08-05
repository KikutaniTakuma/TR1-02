#include "WinApp.h"
#include <cassert>
#include "externals/imgui/imgui_impl_win32.h"
#include <string>

extern IMGUI_IMPL_API LRESULT
ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

WinApp::WinApp():
	hwnd{},
	w{},
	windowStyle(0u),
	isFullscreen(false),
	windowRect{},
	windowName(),
	aspectRatio(0.0f)
{}

WinApp::~WinApp() {
	UnregisterClass(w.lpszClassName, w.hInstance);
}

BOOL UnadjustWindowRectEx(LPRECT prc, DWORD dwStyle, BOOL fMenu, DWORD dwExStyle) {
	RECT rc;
	SetRectEmpty(&rc);
	BOOL fRc = AdjustWindowRectEx(&rc, dwStyle, fMenu, dwExStyle);
	if (fRc) {
		prc->left -= rc.left;
		prc->top -= rc.top;
		prc->right -= rc.right;
		prc->bottom -= rc.bottom;
	}
	return fRc;
}

LRESULT WinApp::WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	WinApp* app = reinterpret_cast<WinApp*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}

	switch (msg) {
	case WM_DESTROY:        // ウィンドウが破棄された
		PostQuitMessage(0); // OSに対して、アプリの終了を伝える
		return 0;

	case WM_SIZING: {
		// アスペクト比を変えるサイズ変更を許可しない
		if (app) {
			float aspectRatio = app->aspectRatio;
			float aspectRatioRecp = 1.0f / aspectRatio;
			RECT* rect = reinterpret_cast<RECT*>(lparam);
			UnadjustWindowRectEx(
				rect, GetWindowLong(hwnd, GWL_STYLE), GetMenu(hwnd) != 0,
				GetWindowLong(hwnd, GWL_EXSTYLE));

			switch (wparam) {
			case WMSZ_LEFT:
			case WMSZ_BOTTOMLEFT:
			case WMSZ_RIGHT:
			case WMSZ_BOTTOMRIGHT:
				rect->bottom = rect->top + LONG((rect->right - rect->left) * aspectRatioRecp);
				break;
			case WMSZ_TOP:
			case WMSZ_TOPRIGHT:
			case WMSZ_BOTTOM:
				rect->right = rect->left + LONG((rect->bottom - rect->top) * aspectRatio);
				break;
			case WMSZ_TOPLEFT:
				rect->top = rect->bottom - LONG((rect->right - rect->left) * aspectRatioRecp);
				rect->left = rect->right - LONG((rect->bottom - rect->top) * aspectRatio);
				break;
			}

			AdjustWindowRectEx(
				rect, GetWindowLong(hwnd, GWL_STYLE), GetMenu(hwnd) != 0,
				GetWindowLong(hwnd, GWL_EXSTYLE)
			);
		}
		break;
	}
	}
	return DefWindowProc(hwnd, msg, wparam, lparam); // 標準の処理を行う
}

void WinApp::Create(const std::wstring& windowTitle, int32_t width, int32_t height) {
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	windowName = windowTitle;

	aspectRatio = static_cast<float>(width) / static_cast<float>(height);
	windowStyle = WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME);

	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = WindowProcedure;
	w.lpszClassName = windowTitle.c_str();
	w.hInstance = GetModuleHandle(nullptr);
	w.hCursor = LoadCursor(NULL, IDC_ARROW);

	RegisterClassEx(&w);

	windowRect = { 0,0,width, height };

	AdjustWindowRect(&windowRect, windowStyle, false);

	hwnd = CreateWindow(
		w.lpszClassName,
		windowTitle.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr,
		nullptr,
		w.hInstance,
		nullptr
	);
	SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

	windowStyle &= ~WS_THICKFRAME;

	SetWindowLong(hwnd, GWL_STYLE, windowStyle);
	SetWindowPos(
		hwnd, NULL, 0, 0, 0, 0, (SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED));
	ShowWindow(hwnd, SW_NORMAL);
}

void WinApp::SetFullscreen(bool fullscreen) {

	if (isFullscreen != fullscreen) {
		if (fullscreen) {
			// 元の状態を覚えておく
			GetWindowRect(hwnd, &windowRect);

			// 仮想フルスクリーン化
			SetWindowLong(
				hwnd, GWL_STYLE,
				windowStyle &
				~(WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME)
			);

			RECT fullscreenRect{ 0 };
			HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
			MONITORINFO info;
			info.cbSize = sizeof(info);
			GetMonitorInfo(monitor, &info);
			fullscreenRect.right = info.rcMonitor.right - info.rcMonitor.left;
			fullscreenRect.bottom = info.rcMonitor.bottom - info.rcMonitor.top;

			SetWindowPos(
				hwnd, HWND_TOPMOST, fullscreenRect.left, fullscreenRect.top, fullscreenRect.right,
				fullscreenRect.bottom, SWP_FRAMECHANGED | SWP_NOACTIVATE
			);
			ShowWindow(hwnd, SW_MAXIMIZE);

		}
		else {
			// 通常ウィンドウに戻す
			SetWindowLong(hwnd, GWL_STYLE, windowStyle);

			SetWindowPos(
				hwnd, HWND_NOTOPMOST, windowRect.left, windowRect.top,
				windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
				SWP_FRAMECHANGED | SWP_NOACTIVATE
			);

			ShowWindow(hwnd, SW_NORMAL);
		}
	}

	isFullscreen = fullscreen;
}