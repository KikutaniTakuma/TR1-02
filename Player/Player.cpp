#include "Player.h"
#include "Engine/KeyInput/KeyInput.h"
#include "externals/imgui/imgui.h"
#include "Engine/Gamepad/Gamepad.h"
#include <numbers>
#include <cmath>
#include "externals/nlohmann/json.hpp"
#include "GlobalVariables/GlobalVariables.h"

Player::Player(GlobalVariables* data_):
	spd(10.0f),
	moveVec(),
	pos(),
	camera(nullptr),
	bullets(0)
{
	data = data_;
	model = std::make_unique<Model>();
	model->LoadObj("Resources/Cube.obj");
	model->LoadShader();
	model->CreateGraphicsPipeline();

	Bullet::LoadModel();
}

Player::~Player() {
	Bullet::UnloadModel();
}

void Player::ApplyGlobalVariables() {
	const std::string groupName = "Player";
	const std::string groupName2 = "PlayerParts";
	model->pos = data->GetVector3Value(groupName2, "Body Translation");
	spd = data->GetFloatValue(groupName, "Speed");
}

void Player::Animation() {
	
}

void Player::Update() {
	//ApplyGlobalVariables();
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

	model->pos = pos;

	if (KeyInput::Pushed(DIK_SPACE) || Gamepad::Pushed(Gamepad::Button::X)) {
 		bullets.push_back(Bullet());
		bullets.back().Initialize(pos, moveVec.GetRad());
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

	camera->Update(VertMakeMatrixAffin(model->scale, model->rotate, pos));
	model->Update();
}

void Player::Draw() {
	model->Draw(camera->GetViewProjection(), camera->pos);
	for (auto& bullet : bullets) {
		bullet.Draw(camera->GetViewProjection(), camera->pos);
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