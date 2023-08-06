#pragma once
#include "Model/Model.h"
#include <memory>
#include "Camera/Camera.h"
#include <vector>

class Enemy {
public:
	Enemy();
	Enemy(const Enemy&) = default;
	~Enemy() = default;
	Enemy& operator=(const Enemy&) = default;

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
	std::vector<std::unique_ptr<Model>> model;

	float spd;

	Vector3 moveVec;

	Camera* camera;

	float freqSpd;
	float freq;
	float radius;
};