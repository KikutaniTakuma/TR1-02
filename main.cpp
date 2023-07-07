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

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	Engine::Initialize(1280, 720, "DirectXGame");


	auto model = std::make_unique<Model>();
	model->LoadObj("./Resources/cube.obj");
	model->LoadShader("WaveShader/WaveNone.VS.hlsl", "WaveShader/Wave.PS.hlsl", "WaveShader/Wave.GS.hlsl");


	Mat4x4 worldMat = MakeMatrixAffin(Vector3::identity, Vector3(), Vector3());

	Camera camera;
	camera.fov = 0.45f;
	camera.pos ={ 8.24f,9.63f,-20.53f };
	camera.rotate = { 0.44f,-0.4f, 0.0f };

	Camera camera2D;

	Vector3 cameraMoveRotate{};

	auto tex = std::make_unique<Texture2D>();
	tex->Initialize("Texture2DShader/Texture2D.VS.hlsl", "Texture2DShader/Texture2DNone.PS.hlsl");
	tex->LoadTexture("./Resources/uvChecker.png");


	auto pera = std::make_unique<PeraRender>();
	pera->Initialize("PostShader/Post.VS.hlsl", "PostShader/PostNone.PS.hlsl");

	/// 
	/// メインループ
	/// 
	while (Engine::WindowMassage()) {
		// 描画開始処理
		Engine::FrameStart();

		// fps
		ImGui::SetNextWindowPos({});
		ImGui::SetNextWindowSize({150,50});
		ImGui::Begin("fps");
		ImGui::Text("Frame rate: %3.0f fps", ImGui::GetIO().Framerate);
		ImGui::End();

		// 入力処理
		Gamepad::GetInstans()->Input();
		KeyInput::Input();
		Mouse::Input();

		/// 
		/// 更新処理
		/// 
		if ((Gamepad::GetInstans()->getStick(Gamepad::Stick::RIGHT_X) > 10000)) {
			cameraMoveRotate.y += 0.1f;
		}
		else if ((Gamepad::GetInstans()->getStick(Gamepad::Stick::RIGHT_X) < -10000)) {
			cameraMoveRotate.y -= 0.1f;
		}

		if ((Gamepad::GetInstans()->getStick(Gamepad::Stick::RIGHT_Y) > 10000)) {
			cameraMoveRotate.x += 0.1f;
		}
		else if ((Gamepad::GetInstans()->getStick(Gamepad::Stick::RIGHT_Y) < -10000)) {
			cameraMoveRotate.x -= 0.1f;
		}

		if (KeyInput::LongPush(DIK_W)) {
			camera.pos.z += 0.1f;
		}
		if (KeyInput::LongPush(DIK_S)) {
			camera.pos.z -= 0.1f;
		}
		if (KeyInput::LongPush(DIK_A)) {
			camera.pos.x -= 0.1f;
		}
		if (KeyInput::LongPush(DIK_D)) {
			camera.pos.x += 0.1f;
		}

		if (KeyInput::LongPush(DIK_UP)) {
			camera.rotate.x += 0.01f;
		}
		if (KeyInput::LongPush(DIK_DOWN)) {
			camera.rotate.x -= 0.01f;
		}
		if (KeyInput::LongPush(DIK_LEFT)) {
			camera.rotate.y -= 0.01f;
		}
		if (KeyInput::LongPush(DIK_RIGHT)) {
			camera.rotate.y += 0.01f;
		}



		ImGui::Begin("Camera");
		ImGui::DragFloat3("cameraPos", &camera.pos.x, 0.01f);
		ImGui::DragFloat3("cameraRotate", &camera.rotate.x, 0.01f);
		ImGui::DragFloat3("cameraScale", &camera.scale.x, 0.01f);
		ImGui::DragFloat3("cameraMoveRotate", &cameraMoveRotate.x, 0.01f);
		ImGui::End();

		camera.Update();
		camera2D.Update();

		model->Update();

		///
		/// 更新処理ここまで
		/// 

		///
		/// 描画処理
		/// 
		pera->PreDraw();

		model->Draw(worldMat, camera.getViewProjection(), camera.pos);

		tex->Draw(Vector2(2.5f, 1.40625f), 0.0f, Vector2(), camera2D.getViewOthographics());

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