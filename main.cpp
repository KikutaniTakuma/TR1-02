#include "Engine/Engine.h"
#include "externals/imgui/imgui.h"
#include <chrono>
#include <thread>
#include "Model/Model.h"
#include <memory>
#include "Engine/WinApp/WinApp.h"
#include "Engine/Gamepad/Gamepad.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	Engine::Initalize(1280, 720, "DirectXGame");
	Gamepad::Initilize();

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
	/// ƒƒCƒ“ƒ‹[ƒv
	/// 
	while (Engine::WindowMassage()) {
		Engine::FrameStart();
		Gamepad::GetInstans()->Input();

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
	}


	model.reset();
	Gamepad::Finalize();

	Engine::Finalize();

	return 0;
}