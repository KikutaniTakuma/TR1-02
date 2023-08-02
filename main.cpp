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
#include "Player/Player.h"
#include "Enemy/Enemy.h"
#include "GlobalVariables/GlobalVariables.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ライブラリ初期化
#if _DEBUG
	Engine::Initialize(1280, 720, "DirectXGame");
#else 
	// 画面解像度取得
	int32_t width = GetSystemMetrics(SM_CXSCREEN);
	int32_t height = GetSystemMetrics(SM_CYSCREEN);

	// ライブラリ初期化
	Engine::Initialize(width, height, "DirectXGame");
	// フルスクリーン化
	WinApp::GetInstance()->SetFullscreen(true);
#endif

	// フォントロード
	Engine::LoadFont("Font/JapaneseGothic.spritefont");

	Camera camera;
	camera.farClip = 5000.0f;
	camera.pos = { 0.0f,0.0f,-10.0f };

	camera.isDebug = true;

	Camera camera2D(Camera::Mode::Othographic);

	auto pera = std::make_unique<PeraRender>();
	pera->Initialize("PostShader/Post.VS.hlsl", "PostShader/PostNone.PS.hlsl");

	bool fullscreen = false;

	auto player = std::make_unique<Player>();
	player->SetCamera(&camera);

	auto enemy = std::make_unique<Enemy>();
	enemy->SetCamera(&camera);

	auto skyDome = std::make_unique<Model>();
	skyDome->LoadObj("AL_Resouce/skydome/skydome.obj");
	skyDome->LoadShader("ModelShader/Model.VS.hlsl", "ModelShader/ModelNone.PS.hlsl");
	skyDome->CreateGraphicsPipeline();
	skyDome->scale *= 1000.0f;

	auto ground = std::make_unique<Texture2D>();
	ground->LoadTexture("AL_Resouce/ground.png");
	ground->Initialize();
	ground->scale *= 1000.0f;
	ground->rotate.x = std::numbers::pi_v<float> * 0.5f;
	ground->uvSize *= 10000.0f;

	GlobalVariables::GetInstance()->LoadFile();

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

		
		GlobalVariables::GetInstance()->Update();

		if (camera2D.isDebug) {
			camera.isDebug = !camera2D.isDebug;
		}
		ImGui::Begin("Camera");
		ImGui::Checkbox("Debug", &camera.isDebug);
		static auto cameraPos =  camera.GetPos();
		cameraPos =  camera.GetPos();
		ImGui::Text("cameraPos : %.0f, %.0f, %.0f", cameraPos.x, cameraPos.y, cameraPos.z);
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

		player->Debug();

		auto playerPos = player->GetPos();
		playerPos.y = 0.0f;
		camera.Update(playerPos);
		camera2D.Update();

		player->Update();
		enemy->Update();

		if (KeyInput::Pushed(DIK_P)) {
			GlobalVariables::GetInstance()->SaveFile("Player");
		}

		///
		/// 更新処理ここまで
		/// 

		///
		/// 描画処理
		/// 
		pera->PreDraw();

		player->Draw();
		enemy->Draw();
		skyDome->Draw(camera.GetViewProjection(), camera.pos);
		ground->Draw(camera.GetViewProjection(),Pipeline::Normal);

		pera->Draw();
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