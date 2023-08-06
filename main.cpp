#include "Engine/Engine.h"
#include "externals/imgui/imgui.h"
#include <chrono>
#include <thread>
#include <numbers>
#include <memory>
#include "Model/Model.h"
#include "Engine/WinApp/WinApp.h"
#include "Engine/Gamepad/Gamepad.h"
#include "Engine/KeyInput/KeyInput.h"
#include "Engine/Mouse/Mouse.h"
#include "PeraRender/PeraRender.h"
#include "Texture2D/Texture2D.h"
#include "AudioManager/AudioManager.h"
#include "Camera/Camera.h"
#include "StringOut/StringOut.h"
#include "Line/Line.h"
#include "Editor/Node/Node.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ライブラリ初期化
	if(!Engine::Initialize("DirectXGame")){
		ErrorCheck::GetInstance()->ErrorTextBox("InitializeDirect3D() : Engine::Initialize() Failed", "WinMain");
		return -1;
	}


	// フォントロード
	Engine::LoadFont("Font/JapaneseGothic.spritefont");

	Camera camera;
	camera.pos = { 0.0f,0.0f,-10.0f };
#if _DEBUG
	camera.isDebug = true;
#endif

	Camera camera2D(Camera::Mode::Othographic);

	auto pera = std::make_unique<PeraRender>();
	pera->Initialize("PostShader/Post.VS.hlsl", "PostShader/PostNone.PS.hlsl");

	bool fullscreen = false;

	auto player = std::make_unique<Model>();
	player->LoadObj("Resources/Watame/Watame.obj");
	player->LoadShader();
	player->CreateGraphicsPipeline();


	/// 
	/// メインループ
	/// 
	while (Engine::WindowMassage()) {
		// 描画開始処理
		Engine::FrameStart();

		// fps
		ImGui::SetNextWindowPos({});
		ImGui::SetNextWindowSizeConstraints({ 150,50 }, { 150,50 });
		ImGui::SetNextWindowCollapsed(false);
		ImGui::Begin("fps");
		ImGui::Text("Frame rate: %3.0f fps", ImGui::GetIO().Framerate);
		ImGui::End();

		// 入力処理
		Gamepad::Input();
		KeyInput::Input();
		Mouse::Input();

		/// 
		/// 更新処理
		/// 

		if (KeyInput::Releaed(DIK_F11) || ((KeyInput::LongPush(DIK_LALT) || KeyInput::LongPush(DIK_RALT)) && KeyInput::Releaed(DIK_RETURN))) {
			fullscreen = !fullscreen;
			WinApp::GetInstance()->SetFullscreen(fullscreen);
		}


		if (camera2D.isDebug) {
			camera.isDebug = !camera2D.isDebug;
		}
		ImGui::Begin("Camera");
		ImGui::Checkbox("Debug", &camera.isDebug);
		ImGui::DragFloat3("cameraPos", &camera.pos.x, 0.01f);
		ImGui::DragFloat3("cameraRotate", &camera.rotate.x, 0.01f);
		ImGui::DragFloat3("cameraScale", &camera.scale.x, 0.01f);
		ImGui::DragFloat("cameraFoV", &camera.fov, 0.01f);
		ImGui::End();

		if (camera.isDebug) {
			camera2D.isDebug = !camera.isDebug;
		}
		ImGui::Begin("Camera2D");
		ImGui::Checkbox("Debug", &camera2D.isDebug);
		ImGui::DragFloat3("cameraPos", &camera2D.pos.x, 0.01f);
		ImGui::DragFloat3("cameraRotate", &camera2D.rotate.x, 0.01f);
		ImGui::DragFloat3("cameraScale", &camera2D.scale.x, 0.01f);
		ImGui::DragFloat("cameraFoV", &camera2D.fov, 0.01f);
		ImGui::End();

		if (KeyInput::Pushed(DIK_1)) {
			Engine::SetResolution(Engine::Resolution::HDTV);
		}
		if (KeyInput::Pushed(DIK_2)) {
			Engine::SetResolution(Engine::Resolution::FHD);
		}
		if (KeyInput::Pushed(DIK_3)) {
			Engine::SetResolution(Engine::Resolution::UHD);
		}
		if (KeyInput::Pushed(DIK_4)) {
			Engine::SetResolution(Engine::Resolution::SHV);
		}

		camera.Update(Vector3());
		camera2D.Update();

		///
		/// 更新処理ここまで
		/// 

		///
		/// 描画処理
		/// 
		//pera->PreDraw();

		player->Draw(camera.GetViewProjection(), camera.pos);

		//pera->Draw();
		///
		/// 描画処理ここまで
		/// 


		// フレーム終了処理
		Engine::FrameEnd();

		// Escapeが押されたら終了
		if (KeyInput::Releaed(DIK_ESCAPE)) {
			break;
		}
	}

	Engine::Finalize();

	return 0;
}