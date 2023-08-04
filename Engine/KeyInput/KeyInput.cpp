#include "KeyInput.h"
#include "Engine/WinApp/WinApp.h"
#include "Engine/Engine.h"
#include <cassert>
#include "Engine/ErrorCheck/ErrorCheck.h"

void KeyInput::Input() {
	if (!instance->initalizeSucceeded) {
		return;
	}

	std::copy(instance->key.begin(), instance->key.end(), instance->preKey.begin());
	
	// キーボード情報取得開始
	instance->keyBoard->Acquire();

	// キー入力
	instance->key = { 0 };
	instance->keyBoard->GetDeviceState(DWORD(instance->key.size()), instance->key.data());
}

bool KeyInput::Pushed(uint8_t keyType) {
	if (!instance->initalizeSucceeded) {
		return false;
	}
	return (instance->key[keyType] & 0x80) && !(instance->preKey[keyType] & 0x80);
}
bool KeyInput::LongPush(uint8_t keyType) {
	if (!instance->initalizeSucceeded) {
		return false;
	}
	return (instance->key[keyType] & 0x80) && (instance->preKey[keyType] & 0x80);
}
bool KeyInput::Releaed(uint8_t keyType) {
	if (!instance->initalizeSucceeded) {
		return false;
	}
	return !(instance->key[keyType] & 0x80) && (instance->preKey[keyType] & 0x80);
}

KeyInput* KeyInput::instance = nullptr;

void KeyInput::Initialize() {
	instance = new KeyInput();
}
void KeyInput::Finalize() {
	delete instance;
	instance = nullptr;
}

KeyInput::KeyInput():
	keyBoard(),
	key{0},
	preKey{0},
	initalizeSucceeded(false)
{
	
	HRESULT hr = Engine::GetDirectInput()->CreateDevice(GUID_SysKeyboard, keyBoard.GetAddressOf(), NULL);
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("CreateDevice failed", "KeyInput");
		return;
	}

	hr = keyBoard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("SetDataFormat failed", "KeyInput");
		return;
	}

	hr = keyBoard->SetCooperativeLevel(WinApp::GetInstance()->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("SetCooperativeLevel failed", "KeyInput");
		return;
	}

	initalizeSucceeded = true;
}

KeyInput::~KeyInput() {
	keyBoard->Unacquire();
}