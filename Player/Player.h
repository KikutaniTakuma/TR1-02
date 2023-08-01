#pragma once
#include "Model/Model.h"
#include <memory>
#include "Camera/Camera.h"

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
		return model->pos;
	}

	inline void SetCamera(Camera* camera_) {
		camera = camera_;
	}

private:
	std::unique_ptr<Model> model;

	float spd;

	Vector3 moveVec;

	Camera* camera;
};