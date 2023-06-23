#pragma once
#include <array>
#include <Windows.h>

class KeyInput {
private:
	KeyInput() = default;
	~KeyInput() = default;

public:
	static void Input();

	static bool Pushed(int32_t keyType);
	static bool LongPush(int32_t keyType);
	static bool Releaed(int32_t keyType);


private:
	// キー入力バッファー
	static std::array<BYTE, 0x100> key;
	static std::array<BYTE, 0x100> preKey;
};