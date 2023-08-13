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
//#include "Player/Player.h"
#include "Enemy/Enemy.h"
#include "GlobalVariables/GlobalVariables.h"
#include "SceneManager/Scene/Scene.h"
#include "UIeditor/UIeditor.h"
#include <filesystem>

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ライブラリ初期化
	if(!Engine::Initialize("DirectXGame",Engine::Resolution::HD)){
		ErrorCheck::GetInstance()->ErrorTextBox(
			"InitializeDirect3D() : Engine::Initialize() Failed", 
			"WinMain"
		);
		return -1;
	}


	// フォントロード
	Engine::LoadFont("Font/JapaneseGothic.spritefont");

	Camera camera;
	camera.farClip = 5000.0f;
	camera.pos = { 0.0f,0.0f,-10.0f };

	camera.isDebug = true;

	Camera camera2D(Camera::Mode::Othographic);

	//// シーンをレンダリング
	//auto pera = std::make_unique<PeraRender>();
	//pera->Initialize(
	//	"PostShader/Post.VS.hlsl", 
	//	"PostShader/PostNone.PS.hlsl"
	//);
	//// 輝度を抽出
	//auto luminance = std::make_unique<PeraRender>();
	//luminance->Initialize(
	//	"PostShader/Post.VS.hlsl",
	//	"PostShader/PostLuminance.PS.hlsl"
	//);
	//// ブラーをかける
	//auto averaging = std::make_unique<PeraRender>();
	//averaging->Initialize(
	//	"PostShader/Post.VS.hlsl",
	//	"PostShader/PostAveraging.PS.hlsl"
	//);
	//// ブラーをかける
	//auto averaging2 = std::make_unique<PeraRender>();
	//averaging2->Initialize(
	//	"PostShader/Post.VS.hlsl",
	//	"PostShader/PostAveraging.PS.hlsl"
	//);
	////ブラーをかけたものと普通に描画したものを合成する
	//auto add = std::make_unique<PeraRender>();
	//add->Initialize(
	//	"PostShader/Post.VS.hlsl",
	//	"PostShader/PostNone.PS.hlsl"
	//);

	bool fullscreen = false;

	//auto watame = std::make_unique<Model>();
	//watame->LoadObj("Resources/Watame/Watame.obj");
	//watame->LoadShader();
	//watame->CreateGraphicsPipeline();

	//UIeditor::GetInstance()->LoadFile();

	//UIeditor::GetInstance()->Add("./Resources/uvChecker.png");
	Texture2D tex;
	tex.LoadTexture("./Resources/uvChecker.png");
	tex.Initialize();

	std::list<Texture2D> texs;

	for (auto i : std::filesystem::directory_iterator("./Resources/")) {
		if (i.path().extension() == ".png") {
			texs.push_back(Texture2D());
			texs.back().Initialize();
			texs.back().ThreadLoadTexture(i.path().string());
		}
	}

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

		if (KeyInput::Releaed(DIK_F11) || 
			((KeyInput::LongPush(DIK_LALT) || KeyInput::LongPush(DIK_RALT)) && KeyInput::Releaed(DIK_RETURN))) {
			fullscreen = !fullscreen;
			WinApp::GetInstance()->SetFullscreen(fullscreen);
		}

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

		camera.Update(Vector3());
		camera2D.Update();
		
		if (KeyInput::Pushed(DIK_SPACE)) {
			tex.ThreadLoadTexture("./Resources/watame4k.png");
		}

		tex.Update();

		for (auto& i : texs) {
			i.Update();
		}

		//UIeditor::GetInstance()->Update(camera2D.GetViewOthographicsVp());

		//tex2.Update();

		///
		/// 更新処理ここまで
		/// 

		///
		/// 描画処理
		/// 
		//watame->Draw(camera.GetViewProjection(), camera.pos);
		//pera->PreDraw();
		
		//watame->Draw(camera.GetViewProjection(), camera.pos);

		//// peraに描画されたやつから輝度を抽出するレンダーに描画
		//pera->Draw(Pipeline::None, luminance.get());

		//// 輝度を抽出したものをブラーをかけるものに描画
		//luminance->Draw(Pipeline::None, averaging.get());

		//// 平均化ブラーを更に平均化ブラーをかけるものに描画
		//averaging->Draw(Pipeline::None, averaging2.get());

		//// 平均化ブラーしたものを加算合成
		//averaging2->Draw(Pipeline::Add);



		//UIeditor::GetInstance()->Draw(camera2D.GetViewOthographics());
		//tex.Draw(camera2D.GetViewOthographics());
		//Gamepad::Debug();

		for (auto& i : texs) {
			i.Draw(camera2D.GetViewOthographics());
		}

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