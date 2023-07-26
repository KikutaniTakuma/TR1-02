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
	Engine::Initialize(1280, 720, "DirectXGame");

	// fontLoad
	Engine::LoadFont("Font/JapaneseGothic.spritefont");

	Camera camera;
	camera.pos = { 8.24f,9.63f,-20.53f };
	camera.rotate = { 0.44f,-0.4f, 0.0f };

	Camera camera2D(Camera::Mode::Othographic);


	auto model = std::make_unique<Model>();
	model->LoadObj("./evaluationTaskResources/resources/bunny.obj");
	model->LoadShader("ModelShader/Model.VS.hlsl", "ModelShader/ModelUseTex.PS.hlsl", "ModelShader/Model.GS.hlsl");
	model->CreateGraphicsPipeline();

	Vector3 cameraMoveRotate{};

	auto tex = std::make_unique<Texture2D>();
	tex->LoadTexture("./Resources/uvChecker.png");
	tex->Initialize("Texture2DShader/Texture2D.VS.hlsl", "Texture2DShader/Texture2DNone.PS.hlsl");

	auto texDefault = std::make_unique<Texture2D>();
	texDefault->LoadTexture("./Resources/uvChecker.png");
	texDefault->Initialize("Texture2DShader/Texture2D.VS.hlsl", "Texture2DShader/Texture2DColorChoose.PS.hlsl");


	auto pera = std::make_unique<PeraRender>();
	pera->Initialize("PostShader/Post.VS.hlsl", "PostShader/PostNone.PS.hlsl");

	StringOut text("Font/JapaneseGothic.spritefont");

	bool fullscreen = false;

	Node node;
	node.nodeName = "test";

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
		if ((Gamepad::GetStick(Gamepad::Stick::RIGHT_X) > 0.1f)) {
			cameraMoveRotate.y += 0.1f;
		}
		else if ((Gamepad::GetStick(Gamepad::Stick::RIGHT_X) < -0.1f)) {
			cameraMoveRotate.y -= 0.1f;
		}

		if ((Gamepad::GetStick(Gamepad::Stick::RIGHT_Y) > 0.1f)) {
			cameraMoveRotate.x += 0.1f;
		}
		else if ((Gamepad::GetStick(Gamepad::Stick::RIGHT_Y) < -0.1f)) {
			cameraMoveRotate.x -= 0.1f;
		}

		if (KeyInput::Releaed(DIK_F11)) {
			fullscreen = !fullscreen;
			WinApp::GetInstance()->SetFullscreen(fullscreen);
		}



		ImGui::Begin("Camera");
		ImGui::DragFloat3("cameraPos", &camera.pos.x, 0.01f);
		ImGui::DragFloat3("cameraRotate", &camera.rotate.x, 0.01f);
		ImGui::DragFloat3("cameraScale", &camera.scale.x, 0.01f);
		ImGui::DragFloat3("cameraMoveRotate", &cameraMoveRotate.x, 0.01f);
		ImGui::DragFloat("cameraFoV", &camera.fov, 0.01f);
		ImGui::End();

		camera.Update(Vector3(), cameraMoveRotate);
		camera2D.Update();

		node.Update();

		//model->Update();

		///
		/// 更新処理ここまで
		/// 

		///
		/// 描画処理
		/// 
		pera->PreDraw();

		//node.Draw(camera2D.GetViewOthographics());

		model->Draw(camera.GetViewProjection(), camera.pos);

		//tex->Draw(Vector2::identity, texRotate, texPos, camera2D.GetViewOthographics(), Pipeline::Blend::Noaml);

		//texDefault->Draw(Vector2::identity, texRotate, texDefaultPos, camera2D.GetViewOthographics(), Pipeline::Blend::Noaml);

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