#include "Scene.h"
#include <cassert>

Scene::Scene():
	sceneName("Scene"),
	//player(),
	enemys(0),
	objects(0),
	objects2D(0),
	camera(),
	camera2D(Camera::Mode::Othographic)
{}

//bool Scene::InitializePlayer() {
//	player = std::make_unique<Player>();
//	assert(player);
//	player->SetCamera(&camera);
//	return static_cast<bool>(player);
//}

void Scene::Update() {
	/*if (player) {
		player->Update();
	}*/

	for (auto& enemy : enemys) {
		enemy->Update();
	}

	camera.Update();
	camera2D.Update();
}

void Scene::Draw() {
	/*if (player) {
		player->Draw();
	}*/

	for (auto& enemy : enemys) {
		enemy->Draw();
	}

	for (auto& object : objects) {
		object->Draw(camera.GetViewProjection(), camera.pos);
	}
	for (auto& object2D : objects2D) {
		object2D->Draw(camera.GetViewProjection());
	}
}