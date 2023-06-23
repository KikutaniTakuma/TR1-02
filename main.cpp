#include "Engine/Engine.h"
#include "externals/imgui/imgui.h"
#include <chrono>
#include <thread>
#include "Model/Model.h"
#include <memory>
#include "Engine/WinApp/WinApp.h"
#include "Engine/Gamepad/Gamepad.h"
#include "Engine/KeyInput/KeyInput.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	Engine::Initalize(1280, 720, "DirectXGame");

	auto model = std::make_unique<Model>();

	model->LoadObj("Obj/Cube.obj");

	model->LoadShader("Shaders/Object3D.VS.hlsl", "WaveShader/Wave.PS.hlsl", "WaveShader/Wave.GS.hlsl");


	Mat4x4 worldMat = MakeMatrixAffin(Vector3D(1.0f,1.0f,1.0f), Vector3D(), Vector3D());

	Mat4x4 viewMatrix;
	Mat4x4 projectionMatrix;

	Vector3D cameraPos{ 8.24f,9.63f,-20.53f };
	Vector3D cameraRotate{ 0.44f,-0.4f, 0.0f};
	Vector3D cameraScale{1.0f,1.0f,1.0f};

	Vector3D cameraMoveRotate{};

	/// 
	/// メインループ
	/// 
	while (Engine::WindowMassage()) {
		Engine::FrameStart();
		Gamepad::GetInstans()->Input();
		KeyInput::Input();

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

		if (KeyInput::LongPush(VK_W)) {
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
		}



		ImGui::Begin("Camera");
		ImGui::DragFloat3("cameraPos", &cameraPos.x, 0.01f);
		ImGui::DragFloat3("cameraRotate", &cameraRotate.x, 0.01f);
		ImGui::DragFloat3("cameraScale", &cameraScale.x, 0.01f);
		ImGui::DragFloat3("cameraMoveRotate", &cameraMoveRotate.x, 0.01f);
		ImGui::End();

		model->Update();

		viewMatrix = MakeMatrixAffin(Vector3D(1.0f, 1.0f, 1.0f), cameraMoveRotate, Vector3D()) * MakeMatrixInverse(MakeMatrixAffin(cameraScale, cameraRotate, cameraPos));
		projectionMatrix = MakeMatrixPerspectiveFov(0.45f, static_cast<float>(Engine::GetInstance()->clientWidth) / static_cast<float>(Engine::GetInstance()->clientHeight), 0.1f, 100.0f);

		model->Draw(worldMat, viewMatrix * projectionMatrix, cameraPos);

		Engine::FrameEnd();

		if (KeyInput::Releaed(VK_ESCAPE)) {
			break;
		}
	}


	model.reset();

	Engine::Finalize();

	return 0;
}