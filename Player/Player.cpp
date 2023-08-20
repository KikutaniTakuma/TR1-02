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
	freqSpd(std::numbers::pi_v<float>),
	freq(0.0f),
	armFreqSpd(std::numbers::pi_v<float> / 2.0f),
	armFreq(0.0f),
	weapon(),
	behavior(Behavior::Normal),
	attack(0.0f),
	attackSpd(std::numbers::pi_v<float> / 2.0f)
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

	weapon = std::make_unique<Model>();
	weapon->LoadObj("AL_Resouce/Weapon/Hammer.obj");
	weapon->LoadShader();
	weapon->CreateGraphicsPipeline();
	weapon->SetParent(model.begin()->get());

	assert(data_);
	data = data_;
	const std::string groupName = "Player";
	const std::string groupName2 = "PlayerParts";
	data->CreateGroup(groupName);
	data->AddItem(groupName2, "Body Translation", model[0]->pos);
	data->AddItem(groupName2, "Head Translation", model[1]->pos);
	data->AddItem(groupName2, "LeftArm Translation", model[2]->pos);
	data->AddItem(groupName2, "RightArm Translation", model[3]->pos);
	data->AddItem(groupName, "Speed", spd);
	data->AddItem(groupName, "freqSpd", freqSpd);
	data->AddItem(groupName, "armFreqSpd", armFreqSpd);
	data->AddItem(groupName, "attackSpd", attackSpd);
}

void Player::ApplyGlobalVariables() {
	const std::string groupName = "Player";
	const std::string groupName2 = "PlayerParts";
	model[0]->pos = data->GetVector3Value(groupName2, "Body Translation");
	model[1]->pos = data->GetVector3Value(groupName2, "Head Translation");
	model[2]->pos = data->GetVector3Value(groupName2, "LeftArm Translation");
	model[3]->pos = data->GetVector3Value(groupName2, "RightArm Translation");
	spd = data->GetFloatValue(groupName, "Speed");
	freqSpd = data->GetFloatValue(groupName, "freqSpd");
	armFreqSpd = data->GetFloatValue(groupName, "armFreqSpd");
	attackSpd = data->GetFloatValue(groupName, "attackSpd");
}

void Player::Animation() {
	freq += freqSpd * ImGui::GetIO().DeltaTime;
	model[0]->pos.y = std::sin(freq) + 2.5f;

	if (freq > (std::numbers::pi_v<float> *2.0f)) {
		freq = 0.0f;
	}

	switch (behavior)
	{
	case Player::Behavior::Normal:
	default:
		armFreq += armFreqSpd * ImGui::GetIO().DeltaTime;

		if (armFreq > std::numbers::pi_v<float> *2.0f) {
			armFreq = 0.0f;
		}

		model[2]->rotate.y = armFreq;
		model[3]->rotate.y = armFreq;
		break;
	case Player::Behavior::Attack:
		armFreq += attackSpd  * ImGui::GetIO().DeltaTime;

		if (armFreq > (std::numbers::pi_v<float> * 0.5f)) {
			armFreq = 0.0f;
			behavior = Behavior::Normal;

			model[2]->rotate.x = 0.0f;
			model[3]->rotate.x = 0.0f;
			break;
		}

		model[2]->rotate.x = armFreq + std::numbers::pi_v<float>;
		model[3]->rotate.x = armFreq + std::numbers::pi_v<float>;

		weapon->rotate.x = armFreq;
		break;
	}
}

void Player::Update() {
	ApplyGlobalVariables();
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

	if (KeyInput::Pushed(DIK_SPACE)) {
		behavior = Behavior::Attack;

		armFreq = 0.0f;

		model[2]->rotate.y = 0.0f;
		model[3]->rotate.y = 0.0f;
	}
	
	/*Vector2 moveRotate;
	moveRotate.x = moveVec.x;
	moveRotate.y = moveVec.z;

	moveRotate.Rotate(camera->gazePointRotate.x);
	ImGui::Begin("Hoge");
	ImGui::DragFloat2("Hoge", &camera->gazePointRotate.x);
	ImGui::End();
	moveVec.x = moveRotate.x;
	moveVec.z = moveRotate.y;*/

	pos += moveVec.Normalize() * spd * ImGui::GetIO().DeltaTime;

	model[0]->pos += pos;

	if (isMove) {
		Vector2 rotate;
		rotate.x = moveVec.x;
		rotate.y = moveVec.z;
		
		model[0]->rotate.y = rotate.GetRad();
	}
}

void Player::Draw() {
	for (auto& i : model) {
		i->Draw(camera->GetViewProjection(), camera->pos);
	}
	if (behavior == Behavior::Attack) {
		weapon->Draw(camera->GetViewProjection(), camera->pos);
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