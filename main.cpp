#include "Engine/Engine.h"
#include "externals/imgui/imgui.h"
#include <chrono>
#include <thread>

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	Engine enigne;

	enigne.Initalize(1920, 1080, "DirectXGame");

	enigne.LoadShader();

	/// 
	/// メインループ
	/// 
	while (enigne.WindowMassage()) {
		enigne.FrameStart();

		enigne.DrawTriangle({0.0f,0.0f,0.0f}, { 1.0f,1.0f,1.0f }, 0xff0000aa);
		
		enigne.FrameEnd();
	}

	return 0;
}