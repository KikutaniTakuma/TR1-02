#include "KeyInput.h"
#include "Engine/WinApp/WinApp.h"
#include <cassert>

void KeyInput::Input() {
	std::copy(instance->key.begin(), instance->key.end(), instance->preKey.begin());
	
	// �L�[�{�[�h���擾�J�n
	instance->keyBoard->Acquire();

	// �L�[����
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
	directInput(),
	keyBoard(),
	key{0},
	preKey{0}
{
	HRESULT hr = DirectInput8Create(WinApp::GetInstance()->getHinstance(), DIRECTINPUT_VERSION, IID_IDirectInput8,
		reinterpret_cast<void**>(directInput.GetAddressOf()), nullptr);
	assert(SUCCEEDED(hr));

	hr = directInput->CreateDevice(GUID_SysKeyboard, keyBoard.GetAddressOf(), NULL);
	assert(SUCCEEDED(hr));

	hr = keyBoard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(hr));

	hr = keyBoard->SetCooperativeLevel(WinApp::GetInstance()->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr));
}