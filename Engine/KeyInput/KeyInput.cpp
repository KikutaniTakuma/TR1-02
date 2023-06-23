

#include "KeyInput.h"
#include <cassert>

std::array<BYTE, 0x100> KeyInput::key = { 0 };
std::array<BYTE, 0x100> KeyInput::preKey = { 0 };

void KeyInput::Input() {
	std::copy(key.begin(), key.end(), preKey.begin());
	// ƒL[“ü—Í
	if (!GetKeyboardState(key.data())) {
		assert(!"Input Failed");
	}
}

bool KeyInput::Pushed(int32_t keyType) {
	return (key[keyType] & 0x80) && !(preKey[keyType] & 0x80);
}
bool KeyInput::LongPush(int32_t keyType) {
	return (key[keyType] & 0x80) && (preKey[keyType] & 0x80);
}
bool KeyInput::Releaed(int32_t keyType) {
	return !(key[keyType] & 0x80) && (preKey[keyType] & 0x80);
}