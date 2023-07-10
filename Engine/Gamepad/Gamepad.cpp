#include "GamePad.h"
#include <iostream>
#include <limits.h>

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

bool Gamepad::LongPush(Button type) {
	return GetButton(type) && GetPreButton(type);
}

bool Gamepad::Released(Button type) {
	return !GetButton(type) && GetPreButton(type);
}

unsigned char Gamepad::GetTriger(Triger type) {
	switch (type)
	{
	case Gamepad::Triger::LEFT:
		return Gamepad::GetInstans()->state.Gamepad.bLeftTrigger;
		break;

	case Gamepad::Triger::RIGHT:
		return Gamepad::GetInstans()->state.Gamepad.bRightTrigger;
		break;

	default:
		return 0;
		break;
	}
}

short Gamepad::GetStick(Stick type) {
	switch (type)
	{
	case Gamepad::Stick::LEFT_X:
		return Gamepad::GetInstans()->state.Gamepad.sThumbLX;
		break;
	case Gamepad::Stick::LEFT_Y:
		return Gamepad::GetInstans()->state.Gamepad.sThumbLY;
		break;
	case Gamepad::Stick::RIGHT_X:
		return Gamepad::GetInstans()->state.Gamepad.sThumbRX;
		break;
	case Gamepad::Stick::RIGHT_Y:
		return Gamepad::GetInstans()->state.Gamepad.sThumbRY;
		break;
	default:
		return 0;
		break;
	}
}

void Gamepad::Vibration(uint16_t leftVibIntensity, uint16_t rightVibIntensity) {
	Gamepad::GetInstans()->vibration.wLeftMotorSpeed = WORD(leftVibIntensity);
	Gamepad::GetInstans()->vibration.wRightMotorSpeed = WORD(rightVibIntensity);
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