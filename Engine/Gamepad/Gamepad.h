#pragma once

#include <Windows.h>
#include <Xinput.h>
#pragma comment (lib, "xinput.lib")
#include <stdint.h>

// Xボックスコントローラーでやったのでコメントアウトの説明はそれ
// ほかのコントローラーは分からないので検証必須
// steamなどでpadの設定を行っている場合その設定のほうが優先されてしまうのでアプリを落として使用してください

class Gamepad {
public:
	// 押し込みのボタン
	enum class Button {
		UP = 0, // 十字キー上
		DOWN, // 十字キー下
		LEFT, // 十字キー左
		RIGHT, // 十字キー右
		START, // 漢字の三みたいなやつ
		BACK, // 四角が重なったようなやつ
		LEFT_THUMB, // 左スティック押し込み
		RIGHT_THUMB, // 右スティック押し込み 
		LEFT_SHOULDER, // LB
		RIGHT_SHOULDER, // RB
		A = 12, // A
		B, // B
		X, // X
		Y, // Y
	};

	// RT,LT
	enum class Triger {
		LEFT,
		RIGHT,
	};

	// スティック
	enum class Stick {
		LEFT_X,
		LEFT_Y,
		RIGHT_X,
		RIGHT_Y
	};

private:
	Gamepad();
	Gamepad(const Gamepad&) = delete;
	Gamepad(Gamepad&&) = delete;
	~Gamepad() = default;
	Gamepad& operator=(const Gamepad&) = delete;
	Gamepad& operator=(Gamepad&&) = delete;

private:
	short preButton;
	XINPUT_STATE state;
	XINPUT_VIBRATION vibration;

private:
	static Gamepad* GetInstans();


public:
	// 入力関数
	static void Input();

	// 押し込みのボタンの状態を取得
	static bool GetButton(Button type);

	// 押し込みのボタンの前フレームの状態を取得
	static bool GetPreButton(Button type);

	// 押し込みのボタンを押した瞬間
	static bool Pushed(Button type);

	// 押し込みのボタンを長押し
	static bool LongPush(Button type);

	// 押し込みのボタンを離した瞬間
	static bool Released(Button type);

	// トリガーのボタンの状態を取得
	static unsigned char GetTriger(Triger type);

	// スティックのボタン
	static short GetStick(Stick type);

	// バイブレーション
	static void Vibration(uint16_t leftVibIntensity, uint16_t rightVibIntensity);

	// デバッグ用
	static void Draw();
};