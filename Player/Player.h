#pragma once
#include "Model/Model.h"
#include <memory>
#include "Camera/Camera.h"
#include <list>
#include <vector>
#include "Bullet/Bullet.h"
#include "StringOut/StringOut.h"

class Player {
public:
	enum class Behavior {
		Normal,
		Attack,
	};

	struct State {
		float hp;
		float recovery;
		float attack;
	};

public:
	Player();
	Player(const Player&) = default;
	~Player();
	Player& operator=(const Player&) = default;

public:
	void Update();

	void Draw();

	inline Vector3 GetPos() const {
		return model.pos;
	}

	inline void SetCamera(Camera* camera_) {
		camera = camera_;
	}

	void Debug();

	inline std::list<Bullet>& GetBulletList() {
		return bullets;
	}

	inline float GetHp() const {
		return state.hp;
	}

	inline void DamageHp(float attack) {
		state.hp -= attack;
		damageTime = std::chrono::steady_clock::now();
	}

	inline bool IsDamge() const {
		auto nowTime = std::chrono::steady_clock::now();
		auto timeFromDamge = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - damageTime);

		return timeFromDamge < std::chrono::milliseconds(1000);
	}

	inline float GetAttack() const {
		return state.attack;
	}

	inline float GetRadius() const {
		return radius;
	}

private:
	void Animation();

private:
	Model model;

	float spd;

	Vector3 moveVec;

	Vector3 pos;

	Camera* camera;

	std::list<Bullet> bullets;

	State state;

	float preHp;

	std::chrono::steady_clock::time_point damageTime;
	bool isRed;
	float redFreq;
	float redFreqSpd;

	float radius;

	StringOut hphud;
};