#pragma once
#include "Model/Model.h"
#include "Action/Frame/Frame.h"
#include "Camera/Camera.h"
#include <list>
#include "Player/Bullet/Bullet.h"
#include "Enemy/Enemy.h"

class Boss {
public:
	struct State {
		float hp;
		float attack;
	};

public:
	Boss();
	Boss(const Boss&) = default;
	Boss(Boss&&) = default;

	Boss& operator=(const Boss&) = default;
	Boss& operator=(Boss&&) = default;

public:
	void Initialize();

	void SetPLayer(class Player* player_);

	void Start();

	void Update();

	void Draw();

	inline Vector3 GetPos() const {
		return model.pos;
	}

	inline void SetCamera(Camera* camera_) {
		camera = camera_;
	}

	inline std::list<Bullet>& GetBulletList() {
		return bullets;
	}

	inline std::list<Enemy>& GetEnemyList() {
		return enemys;
	}

	inline float GetHp() const {
		return state.hp;
	}

	inline float GetRadius() const {
		return radius;
	}

	inline void Damage(float damage) {
		state.hp -= damage;
	}

	inline float GetAttack() const {
		return state.attack;
	}

private:
	Model model;

	float spd;

	Vector3 moveVec;

	Camera* camera;

	float radius;

	Vector3 pos;
	Vector3 scale;
	Vector3 rotate;

	Frame barrage;
	Frame barrage2;
	Frame enemyGenerate;
	Frame standBy;
	Frame roar;

	std::list<Bullet> bullets;

	// state
	std::list<Enemy> enemys;

	State state;

	class Player* player;
};