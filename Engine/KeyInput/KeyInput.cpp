#include "KeyInput.h"
#include "Engine/WinApp/WinApp.h"
#include "Engine/Engine.h"
#include <cassert>

void KeyInput::Input() {
	std::copy(instance->key.begin(), instance->key.end(), instance->preKey.begin());
	
	// キーボード情報取得開始
	instance->keyBoard->Acquire();

	// キー入力
	instance->key = { 0 };
	instance->keyBoard->GetDeviceState(DWORD(instance->key.size()), instance->key.data());
}

bool KeyInput::Pushed(uint8_t keyType) {
	return (instance->key[keyType] & 0x80) && !(instance->preKey[keyType] & 0x80);
}
bool KeyInput::LongPush(uint8_t keyType) {
	return (instance->key[keyType] & 0x80) && (instance->preKey[keyType] & 0x80);
}
bool KeyInput::Releaed(uint8_t keyType) {
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
	preKey{0}
{
	
	HRESULT hr = Engine::GetDirectInput()->CreateDevice(GUID_SysKeyboard, keyBoard.GetAddressOf(), NULL);
	assert(SUCCEEDED(hr));

	hr = keyBoard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(hr));

	hr = keyBoard->SetCooperativeLevel(WinApp::GetInstance()->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr));
}

KeyInput::~KeyInput() {
	keyBoard->Unacquire();
}