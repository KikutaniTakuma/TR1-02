#pragma once

#pragma once
#include <list>
#include "Enemy/Enemy.h"
#include "Enemy/Boss/Boss.h"
#include "Player/Player.h"
#include "Camera/Camera.h"
#include "Texture2D/Texture2D.h"
#include "GlobalVariables/GlobalVariables.h"
#include "StringOut/StringOut.h"

class GameScene {
	enum class Scene {
		Title,
		Tutorial,
		Boss,
		Clear,
		Over
	};

	enum class Choose {
		Title,
		Boss
	};

public:
	GameScene();
	~GameScene();

	GameScene& operator=(const GameScene&) = default;
	GameScene& operator=(GameScene&&) noexcept = default;

public:
	void Initialize();

	void Update();

	void Collision();

	void Draw();

private:
	std::unique_ptr<Player> player;
	std::unique_ptr<Boss> boss;
	std::unique_ptr<Model> skyDome;

	std::list<Enemy> enemys;

	std::list<Model> objects;

	std::list<Texture2D> objects2D;

	Camera camera;
	Camera camera2D;

	GameScene::Scene scene;
	GameScene::Scene nextScene;

	Choose choose;
	Texture2D tryAgain;
	Texture2D startScene;


	Texture2D titleMassage;
	Texture2D tutorioalMassage;
	Texture2D gameOverMassage;
	Texture2D gameClearMassage;


	int32_t count;
};