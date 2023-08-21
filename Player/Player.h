#pragma once
#include "Model/Model.h"
#include <memory>
#include "Camera/Camera.h"
#include <list>
#include <vector>
#include "Bullet/Bullet.h"

class Player {
public:
	enum class Behavior {
		Normal,
		Attack,
	};

public:
	Player() = delete;
	Player(class GlobalVariables* data_);
	Player(const Player&) = default;
	~Player();
	Player& operator=(const Player&) = default;

public:
	void Update();

	void Draw();

	inline Vector3 GetPos() const {
		return (*model.begin())->pos;
	}

	inline void SetCamera(Camera* camera_) {
		camera = camera_;
	}

	void Debug();

private:
	void Animation();

	void ApplyGlobalVariables();

private:
	std::vector<std::unique_ptr<Model>> model;
	std::unique_ptr<Model> weapon;

	float spd;

	Vector3 moveVec;

	Vector3 pos;

	Camera* camera;

	float freqSpd;
	float freq;

	float armFreqSpd;
	float armFreq;

	Behavior behavior;

	float attack;
	float attackSpd;

	class GlobalVariables* data;

	std::list<Bullet> bullets;
};