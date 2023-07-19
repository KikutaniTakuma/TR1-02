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
	model->LoadObj("./Resources/Cube.obj");
	model->LoadShader("WaveShader/WaveNone.VS.hlsl", "WaveShader/Wave.PS.hlsl", "WaveShader/Wave.GS.hlsl");
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

	Vector2 texPos = { 312.0f, 0.0f };
	Vector2 texDefaultPos = { -312.0f, 0.0f };
	float texRotate = 0.0f;

	auto testAudio = AudioManager::GetInstance()->LoadWav("Alarm01.wav",true);

	StringOut text("Font/JapaneseGothic.spritefont");

	bool fullscreen = false;

	Line line;

	Vector2 startPos;
	Vector2 endPos;

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

		/*if (KeyInput::LongPush(DIK_W)) {
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
		}*/

		if (Gamepad::Pushed(Gamepad::Button::A)) {
			testAudio->Start(1.0f);
		}
		else if (Gamepad::Pushed(Gamepad::Button::B)) {
			testAudio->Pause();
		}
		else if (Gamepad::Pushed(Gamepad::Button::X)) {
			testAudio->Stop();
		}

		if (KeyInput::Releaed(DIK_F11)) {
			fullscreen = !fullscreen;
			WinApp::GetInstance()->SetFullscreen(fullscreen);
		}



		ImGui::Begin("Camera");
		ImGui::DragFloat3("cameraPos", &camera.pos.x, 0.01f);
		ImGui::DragFloat3("cameraRotate", &camera.rotate.x, 0.01f);
		ImGui::DragFloat3("cameraScale", &camera.scale.x, 0.01f);
		ImGui::DragFloat("cameraFoV", &camera.fov, 0.01f);
		ImGui::End();

		ImGui::Begin("Texture");
		ImGui::DragFloat2("tex pos", &texPos.x, 1.0f);
		ImGui::DragFloat("tex rotate", &texRotate, 0.01f);
		ImGui::DragFloat2("texDefaultPos", &texDefaultPos.x, 1.0f);
		ImGui::End();

		camera.Update();
		camera2D.Update();

		ImGui::Begin("Text");
		static std::string hoge;
		hoge.reserve(0x400);
		hoge.resize(0x400);
		ImGui::InputText("DrawText", hoge.data(), hoge.size());
		text.str = ConvertString(hoge);
		ImGui::DragFloat2("TextPos", &text.pos.x);
		ImGui::DragFloat("TextRotate", &text.rotation, 0.01f);
		ImGui::DragFloat2("TextScale", &text.scale.x, 0.01f);
		ImGui::End();

		ImGui::Begin("Line");
		ImGui::DragFloat2("start", &startPos.x);
		ImGui::DragFloat2("end", &endPos.x);
		ImGui::End();

		//model->Update();

		///
		/// 更新処理ここまで
		/// 

		///
		/// 描画処理
		/// 
		pera->PreDraw();

		//model->Draw(camera.GetViewProjection(), camera.pos);

		//tex->Draw(Vector2::identity, texRotate, texPos, camera2D.GetViewOthographics(), Pipeline::Blend::Noaml);

		//texDefault->Draw(Vector2::identity, texRotate, texDefaultPos, camera2D.GetViewOthographics(), Pipeline::Blend::Noaml);

		line.Draw(camera2D.GetViewOthographics(), startPos, endPos, 0xff0000ff);

		pera->Draw();

		text.Draw();

		text.WideDraw();
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