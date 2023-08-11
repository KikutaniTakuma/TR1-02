#include "Mouse.h"
#include "Engine/Engine.h"
#include "Engine/WinApp/WinApp.h"
#include "externals/imgui/imgui.h"
#include "Engine/ErrorCheck/ErrorCheck.h"


Mouse* Mouse::instance = nullptr;

void Mouse::Initialize() {
	instance = new Mouse();
	assert(instance);
	if (!instance) {
		ErrorCheck::GetInstance()->ErrorTextBox("Initialize() : instance failed", "Mouse");
		return;
	}
}


void Mouse::Finalize() {
	delete instance;
	instance = nullptr;
}

Mouse::Mouse() :
	mouse(),
	mosueState(),
	preMosueState(),
	wheel(0),
	initalizeSucceeded(false)
{
	HRESULT hr = Engine::GetDirectInput()->CreateDevice(GUID_SysMouse, mouse.GetAddressOf(), NULL);
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("CreateDevice failed", "Mouse");
		return;
	}

	hr = mouse->SetDataFormat(&c_dfDIMouse2);
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("SetDataFormat", "Mouse");
		return;
	}

	hr = mouse->SetCooperativeLevel(WinApp::GetInstance()->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("SetCooperativeLevel", "Mouse");
		return;
	}

	initalizeSucceeded = true;
}

Mouse::~Mouse() {
	if (mouse) {
		mouse->Unacquire();
	}
}



void Mouse::Input() {
	if (!instance->initalizeSucceeded) {
		return;
	}

	instance->preMosueState = instance->mosueState;

	instance->mouse->Acquire();

	instance->mosueState = {};
	instance->mouse->GetDeviceState(sizeof(DIMOUSESTATE2), &instance->mosueState);

	instance->wheel += static_cast<size_t>(instance->mosueState.lZ);
}

bool Mouse::Pushed(Mouse::Button button) {
	if (!instance->initalizeSucceeded) {
		return false;
	}

	if ((instance->mosueState.rgbButtons[uint8_t(button)] & 0x80) &&
		!(instance->preMosueState.rgbButtons[uint8_t(button)] & 0x80))
	{
		return true;
	}

	return false;
}

bool Mouse::LongPush(Mouse::Button button) {
	if (!instance->initalizeSucceeded) {
		return false;
	}

	if ((instance->mosueState.rgbButtons[uint8_t(button)] & 0x80) &&
		(instance->preMosueState.rgbButtons[uint8_t(button)] & 0x80))
	{
		return true;
	}

	return false;
}

bool Mouse::Releaed(Mouse::Button button) {
	if (!instance->initalizeSucceeded) {
		return false;
	}

	if (!(instance->mosueState.rgbButtons[uint8_t(button)] & 0x80) &&
		(instance->preMosueState.rgbButtons[uint8_t(button)] & 0x80))
	{
		return true;
	}

	return false;
}

Vector2 Mouse::GetVelocity() {
	if (!instance->initalizeSucceeded) {
		return Vector2();
	}
	return { static_cast<float>(instance->mosueState.lX), -static_cast<float>(instance->mosueState.lY) };
}

float Mouse::GetWheel() {
	if (!instance->initalizeSucceeded) {
		return 0.0f;
	}
	return static_cast<float>(instance->wheel);
}

float Mouse::GetWheelVelocity() {
	if (!instance->initalizeSucceeded) {
		return 0.0f;
	}
	return static_cast<float>(instance->mosueState.lZ);
}

Vector2 Mouse::GetPos() {
	if (!instance->initalizeSucceeded) {
		return Vector2();
	}

	POINT p{};
	GetCursorPos(&p);
	ScreenToClient(FindWindowW(WinApp::GetInstance()->GetWindowClassName().c_str(), nullptr), &p);

	Vector2 pos{ static_cast<float>(p.x),static_cast<float>(p.y) };

	return pos;
}

void Mouse::Show(bool flg) {
	::ShowCursor(BOOL(flg));
}

void Mouse::Debug() {
	ImGui::Begin("Mouse Debug");
	ImGui::Text("Left          : %d", LongPush(Mouse::Button::Left));
	ImGui::Text("Midle         : %d", LongPush(Mouse::Button::Middle));
	ImGui::Text("Right         : %d", LongPush(Mouse::Button::Right));
	ImGui::Text("EX0           : %d", LongPush(Mouse::Button::EX0));
	ImGui::Text("EX1           : %d", LongPush(Mouse::Button::EX1));
	ImGui::Text("EX2           : %d", LongPush(Mouse::Button::EX2));
	ImGui::Text("EX3           : %d", LongPush(Mouse::Button::EX3));
	ImGui::Text("EX4           : %d", LongPush(Mouse::Button::EX4));
	ImGui::Text("pos           : %.2f, %.2f", GetPos().x, GetPos().y);
	ImGui::Text("velocity      : %.2f, %.2f", GetVelocity().x, GetVelocity().y);
	ImGui::Text("Wheel         : %.0f", GetWheel());
	ImGui::Text("WheelVelocity : %.0f", GetWheelVelocity());
	ImGui::End();
}