#pragma once
#include <array>
#include <wrl.h>
#include <dinput.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

class KeyInput {
private:
	KeyInput();
	~KeyInput();
	KeyInput& operator=(const KeyInput&) = delete;
	

public:
	static void Input();

	static bool Pushed(uint8_t keyType);
	static bool LongPush(uint8_t keyType);
	static bool Releaed(uint8_t keyType);

public:
	static void Initialize();
	static void Finalize();

private:
	static KeyInput* instance;


private:
	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyBoard;

	// キー入力バッファー
	std::array<BYTE, 0x100> key;
	std::array<BYTE, 0x100> preKey;

	bool initalizeSucceeded;
};