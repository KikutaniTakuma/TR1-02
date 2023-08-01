#include "Player.h"
#include "Engine/KeyInput/KeyInput.h"
#include "externals/imgui/imgui.h"
#include "Engine/Gamepad/Gamepad.h"
#include <numbers>
#include <cmath>

Player::Player():
	spd(10.0f),
	moveVec(),
	camera(nullptr),
	freqSpd(std::numbers::pi_v<float>),
	freq(0.0f),
	armFreqSpd(std::numbers::pi_v<float> / 2.0f),
	armFreq(0.0f)
{
	model.push_back(std::make_unique<Model>());
	auto itr = model.rbegin();
	(*itr)->LoadObj("AL_Resouce/PlayerDivision/PlayerBody.obj");
	(*itr)->LoadShader();
	(*itr)->CreateGraphicsPipeline();

	model.push_back(std::make_unique<Model>());
	itr = model.rbegin();
	(*itr)->LoadObj("AL_Resouce/PlayerDivision/PlayerHead.obj");
	(*itr)->LoadShader();
	(*itr)->CreateGraphicsPipeline();
	(*itr)->SetParent(model.begin()->get());
	

	model.push_back(std::make_unique<Model>());
	itr = model.rbegin();
	(*itr)->LoadObj("AL_Resouce/PlayerDivision/PlayerLeftArm.obj");
	(*itr)->LoadShader();
	(*itr)->CreateGraphicsPipeline();
	(*itr)->SetParent(model.begin()->get());

	model.push_back(std::make_unique<Model>());
	itr = model.rbegin();
	(*itr)->LoadObj("AL_Resouce/PlayerDivision/PlayerRightArm.obj");
	(*itr)->LoadShader();
	(*itr)->CreateGraphicsPipeline();
	(*itr)->SetParent(model.begin()->get());
}

void Player::Animation() {
	freq += freqSpd * ImGui::GetIO().DeltaTime;
	model[0]->pos.y = std::sin(freq) + 0.5f;

	if (freq > std::numbers::pi_v<float> * 2.0f) {
		freq = 0.0f;
	}

	armFreq -= armFreqSpd * ImGui::GetIO().DeltaTime;

	if (armFreq > std::numbers::pi_v<float> *2.0f) {
		armFreq = 0.0f;
	}

	model[2]->pos.y = std::sin(armFreq);
	model[3]->pos.y = std::sin(armFreq);
}

void Player::Update() {
	moveVec = {};
	bool isMove = false;
	Animation();

	if (KeyInput::LongPush(DIK_W)) {
		moveVec.z += spd;
		isMove = true;
	}
	if (KeyInput::LongPush(DIK_A)) {
		moveVec.x -= spd;
		isMove = true;
	}
	if (KeyInput::LongPush(DIK_S)) {
		moveVec.z -= spd;
		isMove = true;
	}
	if (KeyInput::LongPush(DIK_D)) {
		moveVec.x += spd;
		isMove = true;
	}

	if (Gamepad::GetStick(Gamepad::Stick::LEFT_X) > 0.15f || Gamepad::GetStick(Gamepad::Stick::LEFT_X) < -0.15f) {
		moveVec.x += spd * Gamepad::GetStick(Gamepad::Stick::LEFT_X);
		isMove = true;
	}
	if (Gamepad::GetStick(Gamepad::Stick::LEFT_Y) > 0.15f || Gamepad::GetStick(Gamepad::Stick::LEFT_Y) < -0.15f) {
		moveVec.z += spd * Gamepad::GetStick(Gamepad::Stick::LEFT_Y);
		isMove = true;
	}
	
	/*Vector2 moveRotate;
	moveRotate.x = camera->GetPos().x;
	moveRotate.y = camera->GetPos().z;

	moveRotate = Vector2{ model->pos.x,model->pos.z } - moveRotate;*/
	

	model[0]->pos += moveVec * ImGui::GetIO().DeltaTime;

	if (isMove) {
		Vector2 rotate;
		rotate.x = moveVec.x;
		rotate.y = moveVec.z;
		
		
		model[0]->rotate.y = rotate.GetRad();
	}

	ImGui::Begin("Rad");
	ImGui::Text("%f", model[0]->rotate.y);
	ImGui::End();
}

void Player::Draw() {
	for (auto& i : model) {
		i->Draw(camera->GetViewProjection(), camera->pos);
	}
}