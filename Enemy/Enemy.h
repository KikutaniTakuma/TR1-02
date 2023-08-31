#pragma once
#include "Model/Model.h"
#include <memory>
#include "Camera/Camera.h"
#include <vector>
#include "Action/Frame/Frame.h"

class Enemy {
public:
	enum class Type {
		Bomb,
		Recovery
	};
	struct State {
		float hp;
		float attack;
		float recovery;
	};

public:
	Enemy();
	Enemy(const Enemy&) = default;
	~Enemy() = default;
	Enemy& operator=(const Enemy&) = default;

public:
	void Initialize(Enemy::Type type_, class Boss* boss_, float stateScale = 1.0f);

	void Update();

	void Draw();

	inline void SetCamera(Camera* camera_) {
		camera = camera_;
	}

	void SetPlayer(class Player* player_);

	bool Collision(const Vector3& pos_, float radius_) const;

	bool GetIsDeath() const {
		return isDeath;
	}

	static void LoadModel();
	static void UnloadModel();

public:
	Vector3 pos;
	Vector3 scale;
	Vector3 rotate;

private:
	Type type;

	static std::unique_ptr<Model> model;

	float spd;

	Vector3 moveVec;

	Camera* camera;

	float freqSpd;
	float freq;
	float radius;
	
	Frame attack;
	Frame standBy;

	float blastRange;

	bool isDeath;
	bool isRed;
	long long redFreq;
	long long redFreqMax;
	long long redFreqMin;
	float redFreqT;
	float redFreqSpd;

	class Player* player;
	class Boss* boss;

	float range;

	// state
	State state;
};