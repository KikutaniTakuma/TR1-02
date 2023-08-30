#pragma once
#include "Model/Model.h"
#include "Action/Frame/Frame.h"
#include "Camera/Camera.h"
#include <list>
#include "Player/Bullet/Bullet.h"
#include "Enemy/Enemy.h"

class Boss {
public:
	Boss();
	Boss(const Boss&) = default;
	Boss(Boss&&) = default;

	Boss& operator=(const Boss&) = default;
	Boss& operator=(Boss&&) = default;

public:
	void Initialize();

	void Update();

	void Draw();

	inline Vector3 GetPos() const {
		return model.pos;
	}

	inline void SetCamera(Camera* camera_) {
		camera = camera_;
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
	//std::list<Enemy> enemys;
};