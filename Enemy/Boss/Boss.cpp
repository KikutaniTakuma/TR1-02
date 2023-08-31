#include "Boss.h"
#include <cmath>
#include <numbers>
#include "externals/imgui/imgui.h"
#include "Player/Player.h"
#include "Engine/KeyInput/KeyInput.h"

Boss::Boss() :
	model(),
	spd(5.0f),
	moveVec(),
	camera(nullptr),
	radius(10.0f),
	pos(),
	scale(Vector3::identity * 10.0f),
	rotate(),
	barrage(),
	enemyGenerate(),
	standBy(),
	roar(),
	state({
	1000.0f,
	10.0f
	}),
	player(nullptr)
{}

void Boss::SetPLayer(Player* player_) {
	player = player_;
}

void Boss::Initialize() {
	model.LoadObj("./Resources/Enemy/Enemy.obj");
	model.LoadShader();
	model.CreateGraphicsPipeline();

	model.rotate.y += std::numbers::pi_v<float> * 1.0f;

	barrage.func = [this]() {
		if (barrage.GetNowTime().count() % 300 < 50) {
			bullets.push_back(Bullet());
			bullets.back().Initialize(pos, Vector3::zero);
			bullets.push_back(Bullet());
			bullets.back().Initialize(pos, Vector3(0.0f, std::numbers::pi_v<float> * 0.5f, 0.0f));
			bullets.push_back(Bullet());
			bullets.back().Initialize(pos, Vector3(0.0f, std::numbers::pi_v<float> * 1.0f, 0.0f));
			bullets.push_back(Bullet());
			bullets.back().Initialize(pos, Vector3(0.0f, std::numbers::pi_v<float> * 1.5f, 0.0f));
		}

		if (barrage.GetNowTime().count() > 2000) {
			barrage.Stop();
			barrage2.Start();
		}
	};
	barrage2.func = [this]() {
		constexpr float rotateSpd = std::numbers::pi_v<float> * 1.0f;

		if (barrage2.GetNowTime().count() % 300 < 50) {
			bullets.push_back(Bullet());
			bullets.back().Initialize(pos, Vector3(0.0f, std::numbers::pi_v<float> * 0.25f + rotate.y, 0.0f));
			bullets.push_back(Bullet());
			bullets.back().Initialize(pos, Vector3(0.0f, std::numbers::pi_v<float> * 0.75f + rotate.y, 0.0f));
			bullets.push_back(Bullet());
			bullets.back().Initialize(pos, Vector3(0.0f, std::numbers::pi_v<float> * 1.25f + rotate.y, 0.0f));
			bullets.push_back(Bullet());
			bullets.back().Initialize(pos, Vector3(0.0f, std::numbers::pi_v<float> * 1.75f + rotate.y, 0.0f));
		}

		rotate.y += rotateSpd * ImGui::GetIO().DeltaTime;
		model.rotate = rotate;

		if (barrage2.GetNowTime().count() > 2000) {
			barrage2.Stop();
			barrage.Start();
		}
	};
	enemyGenerate.func = [this]() {
		if (enemyGenerate.GetNowTime().count() % 500 < 50) {
			enemys.push_back(Enemy());
			enemys.back().Initialize(Enemy::Type::Bomb, this);
			enemys.back().SetCamera(camera);
			enemys.back().SetPlayer(player);
		}

		if (enemyGenerate.GetNowTime().count() > 10000) {
			enemyGenerate.Stop();
			standBy.Start();
		}
	};
	standBy.func = [this]() {
		if (standBy.GetNowTime().count() > 5000) {
			standBy.Stop();
			barrage.Start();
		}
	};
	roar.func = [this]() {

	};
}

void Boss::Start() {
	barrage.Start();
}

void Boss::Update() {

	barrage.Update();
	barrage2.Update();
	enemyGenerate.Update();
	standBy.Update();
	roar.Update();

	std::erase_if(
		bullets,
		[](const Bullet& num) {
			return num.GetIsDeath();
		}
	);

	for (auto& i : bullets) {
		i.Update();
	}

	/*std::erase_if(
		enemys,
		[](const Enemy& num) {
			return num.GetIsDeath();
		}
	);*/
	for (auto&  i : enemys) {
		i.Update();
	}

	ImGui::Begin("Boss");
	ImGui::Text("hp : %f", state.hp);
	ImGui::End();
}

void Boss::Draw() {
	auto&& vpmat = camera->GetViewProjection();
	auto&& cameraPos = camera->GetPos();
	for (auto& i : bullets) {
		i.SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		i.Draw(vpmat, cameraPos);
	}

	model.Draw(vpmat, cameraPos);

	for (auto& i : enemys) {
		i.Draw();
	}
}