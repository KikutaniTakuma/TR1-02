#include "Player.h"
#include "Engine/KeyInput/KeyInput.h"
#include "externals/imgui/imgui.h"
#include "Engine/Gamepad/Gamepad.h"
#include <numbers>
#include <cmath>
#include "externals/nlohmann/json.hpp"
#include <random>

Player::Player() :
	spd(10.0f),
	moveVec(),
	pos(),
	camera(nullptr),
	bullets(0),
	state({ 100.0f, 20.0f, 10.0f }),
	damageTime(),
	isRed(false),
	redFreq(0.0f),
	redFreqSpd(8.0f),
	radius(1.0f)
{
	model.LoadObj("Resources/Player/Player.obj");
	model.LoadShader();
	model.CreateGraphicsPipeline();
	model.scale = Vector3::identity * 0.3f;

	Bullet::LoadModel();

	pos.z = -10.0f;
}

Player::~Player() {
	Bullet::UnloadModel();
}

void Player::Animation() {
	
}

void Player::Update() {
	moveVec = {};
	//Animation();

	if (KeyInput::LongPush(DIK_W)) {
		moveVec.z += spd;
	}
	if (KeyInput::LongPush(DIK_A)) {
		moveVec.x -= spd;
	}
	if (KeyInput::LongPush(DIK_S)) {
		moveVec.z -= spd;
	}
	if (KeyInput::LongPush(DIK_D)) {
		moveVec.x += spd;
	}

	if (Gamepad::GetStick(Gamepad::Stick::LEFT_X) > 0.2f || Gamepad::GetStick(Gamepad::Stick::LEFT_X) < -0.2f) {
		moveVec.x += spd * Gamepad::GetStick(Gamepad::Stick::LEFT_X);
	}
	if (Gamepad::GetStick(Gamepad::Stick::LEFT_Y) > 0.2f || Gamepad::GetStick(Gamepad::Stick::LEFT_Y) < -0.2f) {
		moveVec.z += spd * Gamepad::GetStick(Gamepad::Stick::LEFT_Y);
	}

	pos += moveVec.Normalize() * spd * ImGui::GetIO().DeltaTime;

	model.pos = pos;

	model.rotate.y = moveVec.GetRad().y;

	if (KeyInput::Pushed(DIK_SPACE) || Gamepad::Pushed(Gamepad::Button::X)) {
 		bullets.push_back(Bullet());
		bullets.back().Initialize(pos, model.rotate);
	}

	std::erase_if(
		bullets, 
		[](const Bullet& num) {
			return num.GetIsDeath();
		}
	);

	for (auto& bullet : bullets) {
		bullet.Update();
	}

	auto nowTime = std::chrono::steady_clock::now();
	auto cameraPos = camera->pos;
	auto timeFromDamge = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - damageTime);
	if (timeFromDamge < std::chrono::milliseconds(1000)) {
		if (timeFromDamge < std::chrono::milliseconds(100)) {
			std::random_device rnd;
			camera->pos += Vector3::identity * static_cast<float>(rnd() % 2);
		}

		if (static_cast<int32_t>(redFreq) % 2 == 0) {
			isRed = !isRed;
		}
		redFreq += redFreqSpd * ImGui::GetIO().DeltaTime;
	}
	else {
		isRed = false;
		redFreq = 0.0f;
	}
	if (timeFromDamge < std::chrono::milliseconds(10000) && state.hp < 100.0f) {
		state.hp += state.recovery * ImGui::GetIO().DeltaTime;
	}

	if (isRed) {
		model.color = 0xff0000ff;
	}



	state.hp = std::clamp(state.hp, 0.0f, 100.0f);

	camera->Update(VertMakeMatrixAffin(model.scale, Vector3::zero, pos));
	model.Update();

	camera->pos = camera->pos;

	ImGui::Begin("Player");
	ImGui::Text("hp : %f", state.hp);
	ImGui::End();
}

void Player::Draw() {
	if (state.hp > 0.0f) {
		model.Draw(camera->GetViewProjection(), camera->pos);
		for (auto& bullet : bullets) {
			bullet.SetColor(Vector4(0.0f, 0.0f, 1.0f, 0.0f));
			bullet.Draw(camera->GetViewProjection(), camera->pos);
		}
	}
}

void Player::Debug() {
	/*model[0]->Debug("Player");
	ImGui::Begin("Player");
	ImGui::DragFloat("spd", &spd);
	if (ImGui::TreeNode("Freq")) {
		ImGui::DragFloat("Freq", &freq);
		ImGui::DragFloat("FreqSpd", &freqSpd);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("armFreq")) {
		ImGui::DragFloat("armFreq", &armFreq);
		ImGui::DragFloat("armFreqSpd", &armFreqSpd);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Attack")) {
		ImGui::DragFloat("attack", &attack);
		ImGui::DragFloat("attackSpd", &attackSpd);
		ImGui::TreePop();
	}
	ImGui::End();*/
}