#include "SceneManger/GameScene/GameScene.h"
#include "Engine/Gamepad/Gamepad.h"
#include "Engine/KeyInput/KeyInput.h"
#include "Engine/Mouse/Mouse.h"
#include "externals/imgui/imgui.h"

GameScene::GameScene():
	player(),
	boss(),
	enemys(),
	objects(),
	objects2D(),
	camera(Camera::Mode::Projecction),
	camera2D(Camera::Mode::Othographic),
	scene(GameScene::Scene::Title),
	nextScene(GameScene::Scene::Title),
	choose(Choose::Boss),
	count(5)
{}

GameScene::~GameScene() {
	Enemy::UnloadModel();
}

void GameScene::Initialize() {
	player = std::make_unique<Player>();
	player->SetCamera(&camera);

	boss = std::make_unique<Boss>();
	boss->Initialize();
	boss->SetCamera(&camera);
	boss->SetPLayer(player.get());

	skyDome = std::make_unique<Model>();
	skyDome->LoadObj("./Resources/skydome/skydome.obj");
	skyDome->LoadShader();
	skyDome->CreateGraphicsPipeline();
	skyDome->scale = Vector3::identity * 1000.0f;


	Enemy::LoadModel();

	objects2D.push_back(Texture2D());
	objects2D.back().LoadTexture("./Resources/Hud/Retry.png");
	objects2D.back().Initialize();

	titleMassage.LoadTexture("./Resources/Hud/PushToXButton.png");
	titleMassage.Initialize();
	titleMassage.scale *= 1.2f;
	titleMassage.pos.y -= 50.0f;

	tutorioalMassage.LoadTexture("./Resources/Hud/AttackToX.png");
	tutorioalMassage.Initialize();
	titleMassage.scale *= 1.2f;

	gameOverMassage.LoadTexture("./Resources/Hud/GameOver.png");
	gameOverMassage.Initialize();
	titleMassage.scale *= 1.2f;

	gameClearMassage.LoadTexture("./Resources/Hud/GameClear.png");
	gameClearMassage.Initialize();
	titleMassage.scale *= 1.2f;

	camera.farClip = 5000.0f;
	camera.pos = { 0.0f,3.0f,-17.0f };
}

void GameScene::Update() {
	scene = nextScene;

	camera2D.Update();

	skyDome->Update();

	switch (scene)
	{
	case GameScene::Scene::Title:
		if (KeyInput::Pushed(DIK_SPACE) || KeyInput::Pushed(DIK_RETURN)
			|| Gamepad::Pushed(Gamepad::Button::X)) 
		{
			nextScene = GameScene::Scene::Tutorial;
		}

		count = 5;

		camera.Update();

		titleMassage.Update();
		break;
	case GameScene::Scene::Tutorial:
		if (Gamepad::Pushed(Gamepad::Button::X)) {
			count--;
		}

		tutorioalMassage.Update();

		if (count <= 0) {
			nextScene = GameScene::Scene::Boss;
			boss->Start();
		}
		player->Update();
		break;
	case GameScene::Scene::Boss:
		if (boss) {
			if (boss->GetHp() <= 0.0f) {
				nextScene = GameScene::Scene::Clear;
				choose = Choose::Title;
			}
		}
		if (player) {
			if (player->GetHp() <= 0.0f) {
				nextScene = GameScene::Scene::Over;
				choose = Choose:: Boss;
			}
		}

		player->Update();
		boss->Update();

		break;
	case GameScene::Scene::Clear:
		for (auto& i : objects2D) {
			i.Update();
		}

		if (KeyInput::Pushed(DIK_SPACE) || KeyInput::Pushed(DIK_RETURN)
			|| Gamepad::Pushed(Gamepad::Button::X))
		{
			player.reset();
			boss.reset();

			nextScene = GameScene::Scene::Title;
			player = std::make_unique<Player>();
			player->SetCamera(&camera);

			boss = std::make_unique<Boss>();
			boss->Initialize();
			boss->SetCamera(&camera);
			boss->SetPLayer(player.get());
		}
		
		break;
	case GameScene::Scene::Over:

		for (auto& i : objects2D) {
			i.Update();
		}

		if (KeyInput::Pushed(DIK_SPACE) || KeyInput::Pushed(DIK_RETURN)
			|| Gamepad::Pushed(Gamepad::Button::X))
		{
			player.reset();
			boss.reset();

			nextScene = GameScene::Scene::Title;
			player = std::make_unique<Player>();
			player->SetCamera(&camera);

			boss = std::make_unique<Boss>();
			boss->Initialize();
			boss->SetCamera(&camera);
			boss->SetPLayer(player.get());
		}
		break;
	}
}

void GameScene::Collision() {
	switch (scene)
	{
	case GameScene::Scene::Title:
		break;
	case GameScene::Scene::Tutorial:
		for (auto& i : boss->GetBulletList()) {
			i.Collision(player->GetPos(), player->GetRadius());
		}

		break;
	case GameScene::Scene::Boss:
		for (auto& i : player->GetBulletList()) {
			if (i.Collision(boss->GetPos(), boss->GetRadius())) {
				boss->Damage(player->GetAttack());
			}
		}
		for (auto& i : boss->GetBulletList()) {
			i.Collision(player->GetPos(), player->GetRadius());
		}
		break;
	case GameScene::Scene::Clear:
		break;
	case GameScene::Scene::Over:
		break;
	}
}

void GameScene::Draw() {
	skyDome->Draw(camera.GetViewProjection(), camera.GetPos());

	switch (scene)
	{
	case GameScene::Scene::Title:
		titleMassage.Draw(camera2D.GetViewOthographics(), Pipeline::Normal);
		break;
	case GameScene::Scene::Tutorial:
		tutorioalMassage.Draw(camera2D.GetViewOthographics(), Pipeline::Normal);
		player->Draw();
		boss->Draw();
		break;
	case GameScene::Scene::Boss:
		player->Draw();
		boss->Draw();
		break;
	case GameScene::Scene::Clear:
		/*for (auto& i : objects2D) {
			i.Draw(camera2D.GetViewOthographics(), Pipeline::Normal);
		}*/

		gameClearMassage.Draw(camera2D.GetViewOthographics(), Pipeline::Normal);
		titleMassage.Draw(camera2D.GetViewOthographics(), Pipeline::Normal);
		break;
	case GameScene::Scene::Over:
		/*for (auto& i : objects2D) {
			i.Draw(camera2D.GetViewOthographics(), Pipeline::Normal);
		}*/
		gameOverMassage.Draw(camera2D.GetViewOthographics(), Pipeline::Normal);
		titleMassage.Draw(camera2D.GetViewOthographics(), Pipeline::Normal);
		break;
	}
}