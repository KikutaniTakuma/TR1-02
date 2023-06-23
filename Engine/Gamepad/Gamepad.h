#pragma once

#include <Windows.h>
#include <Xinput.h>
#pragma comment (lib, "xinput.lib")

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
	inline ~Gamepad() {}

private:
	short preButton;
	XINPUT_STATE state;
	XINPUT_VIBRATION vibration;
	
	static Gamepad* pInstans;

public:
	static void Initilize();

	static void Finalize();

	static Gamepad* GetInstans();


public:
	// ���͊֐�
	void Input();

	// �������݂̃{�^���̏�Ԃ��擾
	bool getButton(Button type);

	// �������݂̃{�^���̑O�t���[���̏�Ԃ��擾
	bool getPreButton(Button type);

	// �������݂̃{�^�����������u��
	bool Pushed(Button type);

	// �������݂̃{�^���𒷉���
	bool LongPush(Button type);

	// �������݂̃{�^���𗣂����u��
	bool Released(Button type);

	// �g���K�[�̃{�^���̏�Ԃ��擾
	unsigned char getTriger(Triger type);

	// �X�e�B�b�N�̃{�^��
	short getStick(Stick type);

	// �o�C�u���[�V����
	void isVibration(const int& flag);

	// �f�o�b�O�p
	void Draw();
};