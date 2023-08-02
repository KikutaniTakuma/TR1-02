#include "Enemy.h"
#include "Engine/KeyInput/KeyInput.h"
#include "externals/imgui/imgui.h"
#include "Engine/Gamepad/Gamepad.h"
#include <numbers>
#include <cmath>

Enemy::Enemy() :
	spd(10.0f),
	moveVec(),
	camera(nullptr),
	model(0),
	freqSpd(std::numbers::pi_v<float>),
	freq(0.0f),
	radius(5.0f)
{
	model.push_back(std::make_unique<Model>());
	model[0]->LoadObj("AL_Resouce/Enemy/enemy/enemy.obj");
	model[0]->LoadShader();
	model[0]->CreateGraphicsPipeline();
}


void Enemy::Update() {
	moveVec = {};

	freq += freqSpd * ImGui::GetIO().DeltaTime;
	if (freq > 2.0f * std::numbers::pi_v<float>) {
		freq = 0.0f;
	}

	moveVec = { -std::sin(freq) * radius * freqSpd * ImGui::GetIO().DeltaTime,
				0.0f,
				std::cos(freq) * radius * freqSpd* ImGui::GetIO().DeltaTime
			  };

	model[0]->pos += moveVec;

	Vector2 rotate;
	rotate.x = moveVec.x;
	rotate.y = moveVec.z;

	model[0]->rotate.y = rotate.GetRad();
}

void Enemy::Draw() {
	for (auto& i : model) {
		i->Draw(camera->GetViewProjection(), camera->pos);
	}
}