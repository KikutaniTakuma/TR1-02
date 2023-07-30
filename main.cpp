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
	// 画面解像度取得
	int32_t width = GetSystemMetrics(SM_CXSCREEN);
	int32_t height = GetSystemMetrics(SM_CYSCREEN);

	// ライブラリ初期化
	Engine::Initialize(width, height, "DirectXGame");

	// フルスクリーン化
	WinApp::GetInstance()->SetFullscreen(true);

	// fontLoad
	Engine::LoadFont("Font/JapaneseGothic.spritefont");

	Camera camera;
	camera.pos = { 0.0f,0.0f,-10.0f };
	//camera.pos = { 8.24f,9.63f,-20.53f };
	//camera.rotate = { 0.44f,-0.4f, 0.0f };
#if _DEBUG
	camera.isDebug = true;
#endif

	Camera camera2D(Camera::Mode::Othographic);

	auto watame = std::make_unique<Model>();
	watame->LoadObj("./Resources/Watame/Watame.obj");
	watame->LoadShader("ModelShader/Model.VS.hlsl", "ModelShader/ModelUseTex.PS.hlsl", "ModelShader/ModelNone.GS.hlsl");
	watame->CreateGraphicsPipeline();

	auto multiMaterial = std::make_unique<Model>();
	multiMaterial->LoadObj("./evaluationTaskResources/resources/multiMaterial.obj");
	multiMaterial->LoadShader("ModelShader/Model.VS.hlsl", "ModelShader/ModelUseTex.PS.hlsl", "ModelShader/ModelNone.GS.hlsl");
	multiMaterial->CreateGraphicsPipeline();

	auto bunny = std::make_unique<Model>();
	bunny->LoadObj("./evaluationTaskResources/resources/bunny.obj");
	bunny->LoadShader("ModelShader/Model.VS.hlsl", "ModelShader/ModelUseTex.PS.hlsl", "ModelShader/ModelNone.GS.hlsl");
	bunny->CreateGraphicsPipeline();

	auto teapot = std::make_unique<Model>();
	teapot->LoadObj("./evaluationTaskResources/resources/teapot.obj");
	teapot->LoadShader("ModelShader/Model.VS.hlsl", "ModelShader/ModelUseTex.PS.hlsl", "ModelShader/ModelNone.GS.hlsl");
	teapot->CreateGraphicsPipeline();

	auto suzanne = std::make_unique<Model>();
	suzanne->LoadObj("./evaluationTaskResources/resources/suzanne.obj");
	suzanne->LoadShader("ModelShader/Model.VS.hlsl", "ModelShader/Model.PS.hlsl", "ModelShader/ModelNone.GS.hlsl");
	suzanne->CreateGraphicsPipeline();


	auto tex = std::make_unique<Texture2D>();
	tex->LoadTexture("./Resources/uvChecker.png");
	tex->Initialize("Texture2DShader/Texture2D.VS.hlsl", "Texture2DShader/Texture2DNone.PS.hlsl");

	auto pera = std::make_unique<PeraRender>();
	pera->Initialize("PostShader/Post.VS.hlsl", "PostShader/PostNone.PS.hlsl");

	bool fullscreen = false;

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

		camera.Update(Vector3());
		camera2D.Update();

		watame->Debug("watame");
		multiMaterial->Debug("multiMaterial");
		bunny->Debug("bunny");
		teapot->Debug("teapot");
		suzanne->Debug("suzanne");
		tex->Debug("tex");

		//model->Update();

		///
		/// 更新処理ここまで
		/// 

		///
		/// 描画処理
		/// 
		pera->PreDraw();

		watame->Draw(camera.GetViewProjection(), camera.pos);
		multiMaterial->Draw(camera.GetViewProjection(), camera.pos);
		bunny->Draw(camera.GetViewProjection(), camera.pos);
		teapot->Draw(camera.GetViewProjection(), camera.pos);
		suzanne->Draw(camera.GetViewProjection(), camera.pos);

		tex->Draw(camera2D.GetViewOthographics(), Pipeline::Blend::Noaml);

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