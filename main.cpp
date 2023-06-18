#include "Engine/Engine.h"
#include "externals/imgui/imgui.h"
#include <chrono>
#include <thread>
#include "Model/Model.h"
#include <memory>

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	Engine::Initalize(1920, 1080, "DirectXGame");

	auto model = std::make_unique<Model>();

	model->LoadObj("Wave.obj");

	model->LoadShader("WaveShader/Wave.VS.hlsl", "WaveShader/Wave.PS.hlsl", "WaveShader/Wave.GS.hlsl");


	Mat4x4 worldMat = MakeMatrixAffin(Vector3D(1.0f,1.0f,1.0f), Vector3D(), Vector3D());

	Mat4x4 viewMatrix;
	Mat4x4 projectionMatrix;

	Vector3D cameraPos{ 5.0f,6.27f,-12.26f };
	Vector3D cameraRotate{ 0.44f,-0.4f, 0.0f};
	Vector3D cameraScale{1.0f,1.0f,1.0f};

	/// 
	/// ƒƒCƒ“ƒ‹[ƒv
	/// 
	while (Engine::WindowMassage()) {
		Engine::FrameStart();
		ImGui::Begin("Camera");
		ImGui::DragFloat3("cameraPos", &cameraPos.x, 0.01f);
		ImGui::DragFloat3("cameraRotate", &cameraRotate.x, 0.01f);
		ImGui::DragFloat3("cameraScale", &cameraScale.x, 0.01f);
		ImGui::End();

		model->Update();

		viewMatrix = MakeMatrixInverse(MakeMatrixAffin(cameraScale, cameraRotate, cameraPos));
		projectionMatrix = MakeMatrixPerspectiveFov(0.45f, static_cast<float>(Engine::GetInstance()->clientWidth) / static_cast<float>(Engine::GetInstance()->clientHeight), 0.1f, 100.0f);

		model->Draw(worldMat, viewMatrix * projectionMatrix, cameraPos);

		Engine::FrameEnd();
	}

	return 0;
}