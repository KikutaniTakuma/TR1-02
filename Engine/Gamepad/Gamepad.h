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
	/// <summary>
	/// 入力処理
	/// </summary>
	static void Input();

private:
	/// <summary>
	/// ボタンの状態取得
	/// </summary>
	/// <param name="type">ボタンタイプ</param>
	/// <returns>1:押された 0:押されてない</returns>
	static bool GetButton(Button type);

	/// <summary>
	/// 前フレームのボタンの状態取得
	/// </summary>
	/// <param name="type">ボタンタイプ</param>
	/// <returns>1:押された 0:押されてない</returns>
	static bool GetPreButton(Button type);

public:
	/// <summary>
	/// 押した瞬間かを取得
	/// </summary>
	/// <param name="type">ボタンタイプ</param>
	/// <returns>1:押された 0:押されてない</returns>
	static bool Pushed(Button type);

	/// <summary>
	/// 押し続けているかを取得
	/// </summary>
	/// <param name="type">ボタンタイプ</param>
	/// <returns>1:押された 0:押されてない</returns>
	static bool LongPushed(Button type);

	/// <summary>
	/// ボタンを離した瞬間かを取得
	/// </summary>
	/// <param name="type">ボタンタイプ</param>
	/// <returns>1:押された 0:押されてない</returns>
	static bool Released(Button type);

	/// <summary>
	/// トリガーを取得
	/// </summary>
	/// <param name="type">トリガーのタイプ</param>
	/// <returns>0.0f ～ 1.0f の値</returns>
	static float GetTriger(Triger type);

	/// <summary>
	/// Stick
	/// </summary>
	/// <param name="type">スティックのタイプ</param>
	/// <returns>-1.0f ～ 1.0f の値</returns>
	static float GetStick(Stick type);

	/// <summary>
	/// バイブレーション
	/// </summary>
	/// <param name="leftVibIntensity">左側のバイブレーション 0.0f ～ 1.0f で強さを指定</param>
	/// <param name="rightVibIntensity">右側のバイブレーション 0.0f ～ 1.0f で強さを指定</param>
	static void Vibration(float leftVibIntensity, float rightVibIntensity);

	// デバッグ用
	static void Debug();
};