#include "GamePad.h"
#include <iostream>
#include <limits.h>
#include <algorithm>

Gamepad::Gamepad() :
	preButton(0),
	state({0}),
	vibration({0})
{}

Gamepad* Gamepad::GetInstans() {
	static Gamepad pInstans;
	return &pInstans;
}

void Gamepad::Input() {
	Gamepad::GetInstans()->preButton = Gamepad::GetInstans()->state.Gamepad.wButtons;
    XInputGetState(0, &Gamepad::GetInstans()->state);
}

bool Gamepad::GetButton(Button type) {
    return (Gamepad::GetInstans()->state.Gamepad.wButtons >> static_cast<short>(type)) % 2 == 1;
}

bool Gamepad::GetPreButton(Button type) {
	return (Gamepad::GetInstans()->preButton >> static_cast<short>(type)) % 2 == 1;
}

bool Gamepad::Pushed(Button type) {
	return GetButton(type) && !GetPreButton(type);
}

bool Gamepad::LongPushed(Button type) {
	return GetButton(type) && GetPreButton(type);
}

bool Gamepad::Released(Button type) {
	return !GetButton(type) && GetPreButton(type);
}

float Gamepad::GetTriger(Triger type) {
	static constexpr float kNormal = 1.0f / static_cast<float>(UCHAR_MAX);

	switch (type)
	{
	case Gamepad::Triger::LEFT:
		return static_cast<float>(Gamepad::GetInstans()->state.Gamepad.bLeftTrigger) * kNormal;
		break;

	case Gamepad::Triger::RIGHT:
		return static_cast<float>(Gamepad::GetInstans()->state.Gamepad.bRightTrigger) * kNormal;
		break;

	default:
		return 0;
		break;
	}
}

float Gamepad::GetStick(Stick type) {
	static constexpr float kNormal = 1.0f / static_cast<float>(SHRT_MAX);

	switch (type)
	{
	case Gamepad::Stick::LEFT_X:
		return static_cast<float>(Gamepad::GetInstans()->state.Gamepad.sThumbLX) * kNormal;
		break;
	case Gamepad::Stick::LEFT_Y:
		return static_cast<float>(Gamepad::GetInstans()->state.Gamepad.sThumbLY) * kNormal;
		break;
	case Gamepad::Stick::RIGHT_X:
		return static_cast<float>(Gamepad::GetInstans()->state.Gamepad.sThumbRX) * kNormal;
		break;
	case Gamepad::Stick::RIGHT_Y:
		return static_cast<float>(Gamepad::GetInstans()->state.Gamepad.sThumbRY) * kNormal;
		break;
	default:
		return 0;
		break;
	}
}

void Gamepad::Vibration(float leftVibIntensity, float rightVibIntensity) {
	leftVibIntensity = std::clamp(leftVibIntensity, 0.0f, 1.0f);
	rightVibIntensity = std::clamp(rightVibIntensity, 0.0f, 1.0f);

	Gamepad::GetInstans()->vibration.wLeftMotorSpeed = static_cast<WORD>(static_cast<float>(USHRT_MAX) * leftVibIntensity);
	Gamepad::GetInstans()->vibration.wRightMotorSpeed = static_cast<WORD>(static_cast<float>(USHRT_MAX) * rightVibIntensity);
	XInputSetState(0, &Gamepad::GetInstans()->vibration);
}

void Gamepad::Draw() {
    printf("LeftX = %.2f%%\n", static_cast<float>(GetStick(Stick::LEFT_X)) / SHRT_MAX * 100.0f);
	printf( "LeftY = %.2f%%\n", static_cast<float>(GetStick(Stick::LEFT_Y)) / SHRT_MAX * 100.0f);
	printf( "RightX = %.2f%%\n", static_cast<float>(GetStick(Stick::RIGHT_X)) / SHRT_MAX * 100.0f);
	printf( "RightY = %.2f%%\n", static_cast<float>(GetStick(Stick::RIGHT_Y)) / SHRT_MAX * 100.0f);
	printf( "UP = %d\n", GetButton(Button::UP));
	printf("DOWN = %d\n", GetButton(Button::DOWN));
	printf("LEFT = %d\n", GetButton(Button::LEFT));
	printf("RIGHT = %d\n", GetButton(Button::RIGHT));
	printf("START = %d\n", GetButton(Button::START));
	printf("BACK = %d\n", GetButton(Button::BACK));
	printf("LEFT_THUMB = %d\n", GetButton(Button::LEFT_THUMB));
	printf("RIGHT_THUMB = %d\n", GetButton(Button::RIGHT_THUMB));
	printf("LEFT_SHOULDER = %d\n", GetButton(Button::LEFT_SHOULDER));
	printf("RIGHT_SHOULDER = %d\n", GetButton(Button::RIGHT_SHOULDER));
	printf("A = %d\n", GetButton(Button::A));
	printf("B = %d\n", GetButton(Button::B));
	printf("X = %d\n", GetButton(Button::X));
	printf("Y = %d\n", GetButton(Button::Y));
	printf("LEFT_TRIGER = %d\n", GetTriger(Triger::LEFT));
	printf("RIGHT_TRIGER = %d\n", GetTriger(Triger::RIGHT));
}