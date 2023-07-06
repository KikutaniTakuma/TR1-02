#pragma once
#include <array>
#include <wrl.h>
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include "Math/Vector2D/Vector2D.h"

class Mouse {
public:
	enum class Button : uint8_t {
		Left = 0,
		Right,
		Middle,
		EX0,
		EX1,
		EX2,
		EX3,
		EX4
	};


private:
	Mouse();
	~Mouse();

public:
	static void Input();

	static bool Pushed(Mouse::Button button);
	static bool LongPush(Mouse::Button button);
	static bool Releaed(Mouse::Button button);

	static Vector2D GetVelocity();

	static Vector2D GetPos();

public:
	static void Initialize();
	static void Finalize();

private:
	static Mouse* instance;

private:
	Microsoft::WRL::ComPtr<IDirectInputDevice8> mouse;

	DIMOUSESTATE2 mosueState;
	DIMOUSESTATE2 preMosueState;
};