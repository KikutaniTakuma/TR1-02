#pragma once
#include "Model/Model.h"
#include <memory>
#include "Camera/Camera.h"
#include <vector>

class Player {
public:
	Player();
	Player(const Player&) = default;
	~Player() = default;
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

private:
	void Animation();

private:
	std::vector<std::unique_ptr<Model>> model;

	float spd;

	Vector3 moveVec;

	Camera* camera;

	float freqSpd;
	float freq;

	float armFreqSpd;
	float armFreq;
};