#include "Boss.h"
#include <cmath>
#include <numbers>
#include "externals/imgui/imgui.h"
#include "Engine/KeyInput/KeyInput.h"

Boss::Boss() :
	model(),
	spd(5.0f),
	moveVec(),
	camera(nullptr),
	radius(10.0f),
	pos(),
	scale(Vector3::identity),
	rotate(),
	barrage(),
	enemyGenerate(),
	standBy(),
	roar()/*,
	enemys()*/
{}

void Boss::Initialize() {
	model.LoadObj("./Resources/Cube.obj");
	model.LoadShader();
	model.CreateGraphicsPipeline();

	barrage.func = [this]() {
		if (barrage.GetNowTime().count() % 300 < 10) {
			bullets.push_back(Bullet());
			bullets.back().Initialize(pos, Vector3::zero);
			bullets.push_back(Bullet());
			bullets.back().Initialize(pos, Vector3(0.0f, std::numbers::pi_v<float> * 0.5f, 0.0f));
			bullets.push_back(Bullet());
			bullets.back().Initialize(pos, Vector3(0.0f, std::numbers::pi_v<float> * 1.0f, 0.0f));
			bullets.push_back(Bullet());
			bullets.back().Initialize(pos, Vector3(0.0f, std::numbers::pi_v<float> * 1.5f, 0.0f));
		}

		if (barrage.GetNowTime().count() > 1000) {
			barrage.Stop();
			standBy.Start();
		}
	};
	barrage2.func = [this]() {
		constexpr float rotateSpd = std::numbers::pi_v<float> * 1.0f;

		if (barrage2.GetNowTime().count() % 300 < 10) {
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
			standBy.Start();
		}
	};
	enemyGenerate.func = [this]() {
		
	};
	standBy.func = [this]() {

	};
	roar.func = [this]() {

	};

	standBy.Start();
}

void Boss::Update() {
	if (KeyInput::Pushed(DIK_P)) {
		barrage.Start();
	}
	if (KeyInput::Pushed(DIK_L)) {
		barrage2.Start();
	}

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
}

void Boss::Draw() {
	auto&& vpmat = camera->GetViewProjection();
	auto&& cameraPos = camera->GetPos();
	for (auto& i : bullets) {
		i.Draw(vpmat, cameraPos);
	}

	model.Draw(vpmat, cameraPos);
}