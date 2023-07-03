#include "Engine/Engine.h"
#include "externals/imgui/imgui.h"
#include <chrono>
#include <thread>
#include <numbers>
#include "Model/Model.h"
#include <memory>
#include "Engine/WinApp/WinApp.h"
#include "Engine/Gamepad/Gamepad.h"
#include "Engine/KeyInput/KeyInput.h"
#include "PeraRender/PeraRender.h"
#include "Texture2D/Texture2D.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	Engine::Initialize(1280, 720, "Yeah");


	auto model = std::make_unique<Model>();
	model->LoadObj("Cube.obj");
	model->LoadShader("WaveShader/WaveNone.VS.hlsl", "WaveShader/Wave.PS.hlsl", "WaveShader/Wave.GS.hlsl");


	Mat4x4 worldMat = MakeMatrixAffin(Vector3D(1.0f,1.0f,1.0f), Vector3D(), Vector3D());

	Mat4x4 viewMatrix;
	Mat4x4 projectionMatrix;

	Vector3D cameraPos{ 8.24f,9.63f,-20.53f };
	Vector3D cameraRotate{ 0.44f,-0.4f, 0.0f};
	Vector3D cameraScale{1.0f,1.0f,1.0f};

	Vector3D cameraMoveRotate{};

	auto tex = std::make_unique<Texture2D>();
	tex->Initialize("Texture2DShader/Texture2D.VS.hlsl", "Texture2DShader/Texture2DNone.PS.hlsl");
	tex->LoadTexture("./Resources/uvChecker.png");

	/*auto tex1 = std::make_unique<Texture2D>();
	tex1->Initialize("Texture2DShader/Texture2D.VS.hlsl", "Texture2DShader/Texture2DNone.PS.hlsl");
	tex1->LoadTexture("./Resources/uvChecker.png");*/


	auto pera = std::make_unique<PeraRender>();
	pera->Initialize("PostShader/Post.VS.hlsl", "PostShader/Wipe.PS.hlsl");

	/// 
	/// メインループ
	/// 
	while (Engine::WindowMassage()) {
		// 描画開始処理
		Engine::FrameStart();

		// 入力処理
		Gamepad::GetInstans()->Input();
		KeyInput::Input();

		/// 
		/// 更新処理
		/// 
		if ((Gamepad::GetInstans()->getStick(Gamepad::Stick::RIGHT_X) > 10000)) {
			cameraMoveRotate.y += 0.01f;
		}
		else if ((Gamepad::GetInstans()->getStick(Gamepad::Stick::RIGHT_X) < -10000)) {
			cameraMoveRotate.y -= 0.01f;
		}

		if ((Gamepad::GetInstans()->getStick(Gamepad::Stick::RIGHT_Y) > 10000)) {
			cameraMoveRotate.x += 0.01f;
		}
		else if ((Gamepad::GetInstans()->getStick(Gamepad::Stick::RIGHT_Y) < -10000)) {
			cameraMoveRotate.x -= 0.01f;
		}

		/*if (KeyInput::LongPush(VK_W)) {
			cameraPos.z += 0.1f;
		}
		if (KeyInput::LongPush(VK_S)) {
			cameraPos.z -= 0.1f;
		}
		if (KeyInput::LongPush(VK_A)) {
			cameraPos.x -= 0.1f;
		}
		if (KeyInput::LongPush(VK_D)) {
			cameraPos.x += 0.1f;
		}

		if (KeyInput::LongPush(VK_UP)) {
			cameraRotate.x += 0.01f;
		}
		if (KeyInput::LongPush(VK_DOWN)) {
			cameraRotate.x -= 0.01f;
		}
		if (KeyInput::LongPush(VK_LEFT)) {
			cameraRotate.y -= 0.01f;
		}
		if (KeyInput::LongPush(VK_RIGHT)) {
			cameraRotate.y += 0.01f;
		}*/

		/*if (Gamepad::GetInstans()->Released(Gamepad::Button::A)) {
			tex->LoadTexture("./Resources/sakabannbasupisu.png");
		}
		else if (Gamepad::GetInstans()->Released(Gamepad::Button::B)) {
			tex->LoadTexture("./Resources/watame.png");
		}
		else if (Gamepad::GetInstans()->Released(Gamepad::Button::X)) {
			tex->LoadTexture("./Resources/uvChecker.png");
		}*/



		ImGui::Begin("Camera");
		ImGui::DragFloat3("cameraPos", &cameraPos.x, 0.01f);
		ImGui::DragFloat3("cameraRotate", &cameraRotate.x, 0.01f);
		ImGui::DragFloat3("cameraScale", &cameraScale.x, 0.01f);
		ImGui::DragFloat3("cameraMoveRotate", &cameraMoveRotate.x, 0.01f);
		ImGui::End();

		//model->Update();

		///
		/// 更新処理ここまで
		/// 

		///
		/// 描画処理
		/// 
		pera->PreDraw();

		viewMatrix = MakeMatrixInverse(MakeMatrixAffin(cameraScale, cameraRotate, cameraPos) * MakeMatrixAffin(Vector3D(1.0f,1.0f,1.0f), cameraMoveRotate, Vector3D()));
		projectionMatrix = MakeMatrixPerspectiveFov(0.45f, static_cast<float>(Engine::GetInstance()->clientWidth) / static_cast<float>(Engine::GetInstance()->clientHeight), 0.1f, 100.0f);


		//model->Draw(worldMat, viewMatrix,  projectionMatrix, cameraPos);

		//tex1->Draw();
		tex->Draw(Texture2D::Blend::None, MakeMatrixAffin(Vector3D(1280.0f,720.0f,1.0f), Vector3D(), Vector3D()));

		pera->Draw();
		///
		/// 描画処理ここまで
		/// 


		// フレーム終了処理
		Engine::FrameEnd();

		// Escapeが押されたら終了
		if (KeyInput::Releaed(VK_ESCAPE)) {
			break;
		}
	}


	//model.reset();
	//tex1.reset();
	/*tex.reset();
	pera.reset();*/

	Engine::Finalize();

	return 0;
}