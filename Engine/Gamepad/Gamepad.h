#pragma once

#include <Windows.h>
#include <Xinput.h>
#pragma comment (lib, "xinput.lib")
#include <stdint.h>

// X�{�b�N�X�R���g���[���[�ł�����̂ŃR�����g�A�E�g�̐����͂���
// �ق��̃R���g���[���[�͕�����Ȃ��̂Ō��ؕK�{
// steam�Ȃǂ�pad�̐ݒ���s���Ă���ꍇ���̐ݒ�̂ق����D�悳��Ă��܂��̂ŃA�v���𗎂Ƃ��Ďg�p���Ă�������

class Gamepad {
public:
	// �������݂̃{�^��
	enum class Button {
		UP = 0, // �\���L�[��
		DOWN, // �\���L�[��
		LEFT, // �\���L�[��
		RIGHT, // �\���L�[�E
		START, // �����̎O�݂����Ȃ��
		BACK, // �l�p���d�Ȃ����悤�Ȃ��
		LEFT_THUMB, // ���X�e�B�b�N��������
		RIGHT_THUMB, // �E�X�e�B�b�N�������� 
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

	// �X�e�B�b�N
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
	/// ���͏���
	/// </summary>
	static void Input();

private:
	/// <summary>
	/// �{�^���̏�Ԏ擾
	/// </summary>
	/// <param name="type">�{�^���^�C�v</param>
	/// <returns>1:�����ꂽ 0:������ĂȂ�</returns>
	static bool GetButton(Button type);

	/// <summary>
	/// �O�t���[���̃{�^���̏�Ԏ擾
	/// </summary>
	/// <param name="type">�{�^���^�C�v</param>
	/// <returns>1:�����ꂽ 0:������ĂȂ�</returns>
	static bool GetPreButton(Button type);

public:
	/// <summary>
	/// �������u�Ԃ����擾
	/// </summary>
	/// <param name="type">�{�^���^�C�v</param>
	/// <returns>1:�����ꂽ 0:������ĂȂ�</returns>
	static bool Pushed(Button type);

	/// <summary>
	/// ���������Ă��邩���擾
	/// </summary>
	/// <param name="type">�{�^���^�C�v</param>
	/// <returns>1:�����ꂽ 0:������ĂȂ�</returns>
	static bool LongPushed(Button type);

	/// <summary>
	/// �{�^���𗣂����u�Ԃ����擾
	/// </summary>
	/// <param name="type">�{�^���^�C�v</param>
	/// <returns>1:�����ꂽ 0:������ĂȂ�</returns>
	static bool Released(Button type);

	/// <summary>
	/// �g���K�[���擾
	/// </summary>
	/// <param name="type">�g���K�[�̃^�C�v</param>
	/// <returns>0.0f �` 1.0f �̒l</returns>
	static float GetTriger(Triger type);

	/// <summary>
	/// Stick
	/// </summary>
	/// <param name="type">�X�e�B�b�N�̃^�C�v</param>
	/// <returns>-1.0f �` 1.0f �̒l</returns>
	static float GetStick(Stick type);

	/// <summary>
	/// �o�C�u���[�V����
	/// </summary>
	/// <param name="leftVibIntensity">�����̃o�C�u���[�V���� 0.0f �` 1.0f �ŋ������w��</param>
	/// <param name="rightVibIntensity">�E���̃o�C�u���[�V���� 0.0f �` 1.0f �ŋ������w��</param>
	static void Vibration(float leftVibIntensity, float rightVibIntensity);

	// �f�o�b�O�p
	static void Debug();
};