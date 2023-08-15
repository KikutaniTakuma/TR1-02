#include "Scene.h"
#include <cassert>
#include "externals/imgui/imgui.h"

Scene::Scene(const std::string& sceneName_):
	sceneName(sceneName_),
	player(),
	enemys(0),
	objects(0),
	objects2D(0),
	camera(),
	camera2D(Camera::Mode::Othographic),
	globalVariables(sceneName_ + "/")
{
	globalVariables.LoadFile();
}

bool Scene::InitializePlayer() {
	player = std::make_unique<Player>(&globalVariables);
	assert(player);
	player->SetCamera(&camera);
	return static_cast<bool>(player);
}

void Scene::Editor() {
	ImGui::Begin("SceneEditor");
	if (ImGui::TreeNode("Player")) {
		if (ImGui::Button("Add Player")) {
			InitializePlayer();
		}
		if (ImGui::Button("Delete Player")) {
			player.reset();
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Enemy")) {
		if (ImGui::Button("Add Enemy")) {
			
		}
		if (ImGui::Button("Delete Enemy")) {
			
		}
		ImGui::TreePop();
	}
	

	ImGui::End();
}

void Scene::Update() {
	if (player) {
		player->Update();
	}

	for (auto& enemy : enemys) {
		enemy.Update();
	}

	camera.Update();
	camera2D.Update();
}

void Scene::Draw() {
	if (player) {
		player->Draw();
	}

	for (auto& enemy : enemys) {
		enemy.Draw();
	}

	for (auto& object : objects) {
		object.Draw(camera.GetViewProjection(), camera.pos);
	}
	for (auto& object2D : objects2D) {
		object2D.Draw(camera.GetViewProjection());
	}
}