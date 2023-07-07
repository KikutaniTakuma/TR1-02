#include "Mouse.h"
#include "Engine/Engine.h"
#include "Engine/WinApp/WinApp.h"
#include <cassert>


Mouse* Mouse::instance = nullptr;

void Mouse::Initialize() {
	instance = new Mouse();
	assert(instance);
}


void Mouse::Finalize() {
	delete instance;
	instance = nullptr;
}

Mouse::Mouse() :
	mouse(),
	mosueState(),
	preMosueState()
{
	HRESULT hr = Engine::GetDirectInput()->CreateDevice(GUID_SysMouse, mouse.GetAddressOf(), NULL);
	assert(SUCCEEDED(hr));

	hr = mouse->SetDataFormat(&c_dfDIMouse2);
	assert(SUCCEEDED(hr));

	hr = mouse->SetCooperativeLevel(WinApp::GetInstance()->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr));
}

Mouse::~Mouse() {
	mouse->Unacquire();
}



void Mouse::Input() {
	instance->preMosueState = instance->mosueState;

	instance->mouse->Acquire();

	instance->mosueState = {};
	instance->mouse->GetDeviceState(sizeof(DIMOUSESTATE2), &instance->mosueState);
}

bool Mouse::Pushed(Mouse::Button button) {
	if (!(instance->mosueState.rgbButtons[uint8_t(button)] & 0x80) &&
		(instance->preMosueState.rgbButtons[uint8_t(button)] & 0x80))
	{
		return true;
	}

	return false;
}

bool Mouse::LongPush(Mouse::Button button) {
	if ((instance->mosueState.rgbButtons[uint8_t(button)] & 0x80) &&
		(instance->preMosueState.rgbButtons[uint8_t(button)] & 0x80))
	{
		return true;
	}

	return false;
}

bool Mouse::Releaed(Mouse::Button button) {
	if ((instance->mosueState.rgbButtons[uint8_t(button)] & 0x80) &&
		!(instance->preMosueState.rgbButtons[uint8_t(button)] & 0x80))
	{
		return true;
	}

	return false;
}

Vector2D Mouse::GetVelocity() {
	return { static_cast<float>(instance->mosueState.lX), -static_cast<float>(instance->mosueState.lY) };
}

Vector2D Mouse::GetPos() {
	POINT p{};
	GetCursorPos(&p);
	ScreenToClient(FindWindowW(WinApp::GetInstance()->GetWindowClassName().c_str(), nullptr), &p);

	Vector2D pos{ static_cast<float>(p.x),static_cast<float>(p.y) };

	return pos;
}

void Mouse::Show(bool flg) {
	::ShowCursor(BOOL(flg));
}