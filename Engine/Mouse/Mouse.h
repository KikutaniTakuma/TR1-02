#pragma once
#include <array>
#define DIRECTINPUT_VERSION 0x0800 // DirectInputのバージョン指定
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

class Mouse {
private:
	Mouse() = default;
	~Mouse() = default;

public:
	static void Input();

	static bool Pushed(int32_t keyType);
	static bool LongPush(int32_t keyType);
	static bool Releaed(int32_t keyType);
};