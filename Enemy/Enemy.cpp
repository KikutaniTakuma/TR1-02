#include "Enemy.h"
#include "Engine/KeyInput/KeyInput.h"
#include "externals/imgui/imgui.h"
#include "Engine/Gamepad/Gamepad.h"
#include <numbers>
#include <cmath>
#include "Player/Player.h"
#include "Boss/Boss.h"
#include <random>

std::unique_ptr<Model> Enemy::model;

void Enemy::LoadModel() {
	if (!model) {
		model = std::make_unique<Model>(100);
		model->LoadObj("./Resources/Enemy/Enemy.obj");
		model->LoadShader();
		model->CreateGraphicsPipeline();
	}
}
void Enemy::UnloadModel() {
	model.reset();
}

Enemy::Enemy() :
	spd(5.0f),
	moveVec(),
	camera(nullptr),
	freqSpd(std::numbers::pi_v<float>),
	freq(0.0f),
	radius(1.0f),
	type(Type::Bomb),
	pos(),
	scale(Vector3::identity),
	rotate(),
	blastRange(2.0f),
	isDeath(false),
	isRed(false),
	player(nullptr),
	boss(nullptr),
	redFreqMax(300),
	redFreq(0),
	redFreqMin(10),
	redFreqT(0.0f),
	redFreqSpd(0.3f),
	range(20.0f),
	state({
		1.0f,
		10.0f,
		50.0f
	})
{
	if (!model) {
		Enemy::LoadModel();
	}
}

void Enemy::Initialize(Enemy::Type type_, class Boss* boss_, float stateScale) {
	type = type_;

	boss = boss_;

	state.attack *= stateScale;
	state.hp *= stateScale;

	std::random_device rnd;
	float rad = static_cast<float>((rnd() % 36) * 10) * (std::numbers::pi_v<float> / 180.0f);

	switch(type)
	{
		case Enemy::Type::Bomb:
		default:
			pos = boss->GetPos();
			break;
		case Enemy::Type::Recovery:
			pos.x += boss->GetPos().x + (range * std::cos(rad));
			pos.z += boss->GetPos().z + (range * std::sin(rad));

			break;
	}

	attack.func = [this]() {
		switch (type)
		{
		case Enemy::Type::Bomb:
		default:
			moveVec = (player->GetPos() - pos).Normalize() * spd;

			pos += moveVec * ImGui::GetIO().DeltaTime;

			redFreq = static_cast<decltype(redFreq)>(std::lerp(redFreqMax, redFreqMin, redFreqT));
			redFreqT += redFreqSpd * ImGui::GetIO().DeltaTime;
			redFreqT = std::clamp(redFreqT, 0.0f, 1.0f);

			if (isRed) {
				model->color = Vector4ToUint({ 1.0f, 0.0f,0.0f,1.0f });
			}
			else {
				model->color = std::numeric_limits<uint32_t>::max();
			}

			if (attack.GetFrameCount() % redFreq == 0) {
				isRed = !isRed;
			}

			if (attack.GetNowTime().count() > 5000) {
				attack.Stop();
				isDeath = true;
			}
			break;
		case Enemy::Type::Recovery:
			moveVec = (boss->GetPos() - pos).Normalize() * spd;

			pos += moveVec * ImGui::GetIO().DeltaTime;

			rotate.y = Vector3::yIdy.Dot(moveVec.GetRad());
			break;
		}
	};

	standBy.func = [this]() {
		if (standBy.GetNowTime().count() > 5000) {
			standBy.Stop();
			attack.Start();
		}
	};

	attack.Start();
}


void Enemy::Update() {
	moveVec = Vector3::zero;

	attack.Update();
	standBy.Update();

	model->pos = pos;
	model->rotate = rotate;
	model->scale = scale;

	if (state.hp <= 0.0f) {
		isDeath = true;
	}
}

void Enemy::Draw() {
	model->Draw(camera->GetViewProjection(), camera->pos);
}

void Enemy::SetPlayer(class Player* player_) {
	player = player_;
}

bool Enemy::Collision(const Vector3& pos_, float radius_) const {
	if (attack) {
		if ((pos - pos_).Length() < (radius + radius_)) {
			return true;
		}

		return false;
	}

	return false;
}