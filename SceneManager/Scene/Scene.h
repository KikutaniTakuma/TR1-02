#pragma once
#include <list>
#include "Enemy/Enemy.h"
#include "Enemy/Boss/Boss.h"
#include "Player/Player.h"
#include "Camera/Camera.h"
#include "Texture2D/Texture2D.h"
#include "GlobalVariables/GlobalVariables.h"

class Scene {
public:
	Scene(const std::string& sceneName_);
	~Scene() = default;

	Scene& operator=(const Scene&) = default;
	Scene& operator=(Scene&&) noexcept = default;

public:
	bool InitializePlayer();

	void Editor();

	void Update();

	void Draw();

private:
	std::string sceneName;

	std::unique_ptr<Player> player;
	std::unique_ptr<Boss> boss;

	std::list<Enemy> enemys;

	std::list<Model> objects;

	std::list<Texture2D> objects2D;

	Camera camera;
	Camera camera2D;

	// Editor
	GlobalVariables globalVariables;
};