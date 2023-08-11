#pragma once
#include <list>
#include "Enemy/Enemy.h"
#include "Player/Player.h"
#include "Camera/Camera.h"
#include "Texture2D/Texture2D.h"

class Scene {
public:
	Scene();
	~Scene() = default;

	Scene& operator=(const Scene&) = default;
	Scene& operator=(Scene&&) noexcept = default;

public:
	//bool InitializePlayer();

	void Update();

	void Draw();

private:
	std::string sceneName;

	//std::unique_ptr<Player> player;

	std::list<std::unique_ptr<Enemy>> enemys;

	std::list<std::unique_ptr<Model>> objects;

	std::list<std::unique_ptr<Texture2D>> objects2D;

	Camera camera;
	Camera camera2D;
};