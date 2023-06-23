#include "GamePad.h"
#include <iostream>
#include <limits.h>

Gamepad* Gamepad::pInstans = nullptr;

Gamepad::Gamepad() :
	preButton(0),
	state({0}),
	vibration({0})
{}

void Gamepad::Initilize() {
	pInstans = new Gamepad();
}

void Gamepad::Finalize() {
	delete pInstans;
	pInstans = nullptr;
}

Gamepad* Gamepad::GetInstans() {
	return pInstans;
}

void Gamepad::Input() {
	preButton = state.Gamepad.wButtons;
    XInputGetState(0, &state);
}

bool Gamepad::getButton(Button type) {
    return (state.Gamepad.wButtons >> static_cast<short>(type)) % 2 == 1;
}

bool Gamepad::getPreButton(Button type) {
	return (preButton >> static_cast<short>(type)) % 2 == 1;
}

bool Gamepad::Pushed(Button type) {
	return getButton(type) && !getPreButton(type);
}

bool Gamepad::LongPush(Button type) {
	return getButton(type) && getPreButton(type);
}

bool Gamepad::Released(Button type) {
	return !getButton(type) && getPreButton(type);
}

unsigned char Gamepad::getTriger(Triger type) {
	switch (type)
	{
	case Gamepad::Triger::LEFT:
		return state.Gamepad.bLeftTrigger;
		break;

	case Gamepad::Triger::RIGHT:
		return state.Gamepad.bRightTrigger;
		break;

	default:
		return 0;
		break;
	}
}

short Gamepad::getStick(Stick type) {
	switch (type)
	{
	case Gamepad::Stick::LEFT_X:
		return state.Gamepad.sThumbLX;
		break;
	case Gamepad::Stick::LEFT_Y:
		return state.Gamepad.sThumbLY;
		break;
	case Gamepad::Stick::RIGHT_X:
		return state.Gamepad.sThumbRX;
		break;
	case Gamepad::Stick::RIGHT_Y:
		return state.Gamepad.sThumbRY;
		break;
	default:
		return 0;
		break;
	}
}

void Gamepad::isVibration(const int& flag) {
	if(flag >= 1){
		vibration.wLeftMotorSpeed = 65535;
		vibration.wRightMotorSpeed = 65535;
		XInputSetState(0, &vibration);
	}
	else {
		vibration.wLeftMotorSpeed = 0;
		vibration.wRightMotorSpeed = 0;
		XInputSetState(0, &vibration);
	}
}

void Gamepad::Draw() {
    printf("LeftX = %.2f%%\n", static_cast<float>(getStick(Stick::LEFT_X)) / SHRT_MAX * 100.0f);
	printf( "LeftY = %.2f%%\n", static_cast<float>(getStick(Stick::LEFT_Y)) / SHRT_MAX * 100.0f);
	printf( "RightX = %.2f%%\n", static_cast<float>(getStick(Stick::RIGHT_X)) / SHRT_MAX * 100.0f);
	printf( "RightY = %.2f%%\n", static_cast<float>(getStick(Stick::RIGHT_Y)) / SHRT_MAX * 100.0f);
	printf( "UP = %d\n", getButton(Button::UP));
	printf("DOWN = %d\n", getButton(Button::DOWN));
	printf("LEFT = %d\n", getButton(Button::LEFT));
	printf("RIGHT = %d\n", getButton(Button::RIGHT));
	printf("START = %d\n", getButton(Button::START));
	printf("BACK = %d\n", getButton(Button::BACK));
	printf("LEFT_THUMB = %d\n", getButton(Button::LEFT_THUMB));
	printf("RIGHT_THUMB = %d\n", getButton(Button::RIGHT_THUMB));
	printf("LEFT_SHOULDER = %d\n", getButton(Button::LEFT_SHOULDER));
	printf("RIGHT_SHOULDER = %d\n", getButton(Button::RIGHT_SHOULDER));
	printf("A = %d\n", getButton(Button::A));
	printf("B = %d\n", getButton(Button::B));
	printf("X = %d\n", getButton(Button::X));
	printf("Y = %d\n", getButton(Button::Y));
	printf("LEFT_TRIGER = %d\n", getTriger(Triger::LEFT));
	printf("RIGHT_TRIGER = %d\n", getTriger(Triger::RIGHT));
}