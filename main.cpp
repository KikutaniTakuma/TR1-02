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
#include "Engine/Mouse/Mouse.h"
#include "PeraRender/PeraRender.h"
#include "Texture2D/Texture2D.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	Engine::Initialize(1280, 720, "DirectXGame");


	/*auto model = std::make_unique<Model>();
	model->LoadObj("./Resources/cube.obj");
	model->LoadShader("WaveShader/WaveNone.VS.hlsl", "WaveShader/Wave.PS.hlsl", "WaveShader/Wave.GS.hlsl");*/


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


	auto pera = std::make_unique<PeraRender>();
	pera->Initialize("PostShader/Post.VS.hlsl", "PostShader/PostNone.PS.hlsl");

	bool mouseFlg = true;

	/// 
	/// ���C�����[�v
	/// 
	while (Engine::WindowMassage()) {
		// �`��J�n����
		Engine::FrameStart();

		// fps
		ImGui::Begin("Control panel");
		ImGui::Text("Frame rate: %6.2f fps", ImGui::GetIO().Framerate);
		ImGui::End();

		// ���͏���
		Gamepad::GetInstans()->Input();
		KeyInput::Input();
		Mouse::Input();

		/// 
		/// �X�V����
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
			cameraPos.z += 0.1f;
		}
		if (KeyInput::LongPush(DIK_S)) {
			cameraPos.z -= 0.1f;
		}
		if (KeyInput::LongPush(DIK_A)) {
			cameraPos.x -= 0.1f;
		}
		if (KeyInput::LongPush(DIK_D)) {
			cameraPos.x += 0.1f;
		}

		if (KeyInput::LongPush(DIK_UP)) {
			cameraRotate.x += 0.01f;
		}
		if (KeyInput::LongPush(DIK_DOWN)) {
			cameraRotate.x -= 0.01f;
		}
		if (KeyInput::LongPush(DIK_LEFT)) {
			cameraRotate.y -= 0.01f;
		}
		if (KeyInput::LongPush(DIK_RIGHT)) {
			cameraRotate.y += 0.01f;
		}

		if (Gamepad::GetInstans()->Released(Gamepad::Button::A)) {
			tex->LoadTexture("./Resources/sakabannbasupisu.png");
		}
		else if (Gamepad::GetInstans()->Released(Gamepad::Button::B)) {
			tex->LoadTexture("./Resources/watame.png");
		}
		else if (Gamepad::GetInstans()->Released(Gamepad::Button::X)) {
			tex->LoadTexture("./Resources/uvChecker.png");
		}



		ImGui::Begin("Camera");
		ImGui::DragFloat3("cameraPos", &cameraPos.x, 0.01f);
		ImGui::DragFloat3("cameraRotate", &cameraRotate.x, 0.01f);
		ImGui::DragFloat3("cameraScale", &cameraScale.x, 0.01f);
		ImGui::DragFloat3("cameraMoveRotate", &cameraMoveRotate.x, 0.01f);
		ImGui::End();

		if (Mouse::Pushed(Mouse::Button::Left)) {
			mouseFlg = true;
		}
		else if(Mouse::Pushed(Mouse::Button::Right)) {
			mouseFlg = false;
		}

		Mouse::Show(mouseFlg);

		//model->Update();

		///
		/// �X�V���������܂�
		/// 

		///
		/// �`�揈��
		/// 
		pera->PreDraw();

		viewMatrix = MakeMatrixInverse(MakeMatrixAffin(cameraScale, cameraRotate, cameraPos) * MakeMatrixAffin(Vector3D(1.0f,1.0f,1.0f), cameraMoveRotate, Vector3D()));
		projectionMatrix = MakeMatrixPerspectiveFov(0.45f, static_cast<float>(Engine::GetInstance()->clientWidth) / static_cast<float>(Engine::GetInstance()->clientHeight), 0.1f, 100.0f);


		//model->Draw(worldMat, viewMatrix,  projectionMatrix, cameraPos);

		tex->Draw(Texture2D::Blend::None, MakeMatrixAffin(Vector3D(1280.0f,720.0f,1.0f), Vector3D(), Vector3D()));

		pera->Draw();
		///
		/// �`�揈�������܂�
		/// 


		// �t���[���I������
		Engine::FrameEnd();

		// Escape�������ꂽ��I��
		if (KeyInput::Releaed(DIK_ESCAPE)) {
			break;
		}
	}

	Engine::Finalize();

	return 0;
}