#include "Player.h"
#include "Engine/KeyInput/KeyInput.h"
#include "externals/imgui/imgui.h"
#include "Engine/Gamepad/Gamepad.h"
#include <numbers>
#include <cmath>

Player::Player():
	spd(5.0f),
	moveVec(),
	camera(nullptr)
{
	model = std::make_unique<Model>();
	model->LoadObj("AL_Resouce/Player.obj");
	model->LoadShader();
	model->CreateGraphicsPipeline();
}

void Player::Update() {
	moveVec = {};
	bool isMove = false;

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
	

	model->pos += moveVec * ImGui::GetIO().DeltaTime;

	if (isMove) {
		Vector2 rotate;
		rotate.x = moveVec.x;
		rotate.y = moveVec.z;
		
		
		model->rotate.y = rotate.GetRad();
	}

	ImGui::Begin("Rad");
	ImGui::Text("%f", model->rotate.y);
	ImGui::End();
}

void Player::Draw() {
	model->Draw(camera->GetViewProjection(), camera->pos);
}