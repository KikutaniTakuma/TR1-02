#pragma once
#include "Model/Model.h"
#include <chrono>

class Bullet {
public:
	Bullet();
	Bullet(const Bullet& right);
	Bullet(Bullet&& right) noexcept;
	~Bullet();

public:
	Bullet& operator=(const Bullet& right);
	Bullet& operator=(Bullet&& right) noexcept;

public:
	void Initialize(const Vector3& pos_, const Vector3& rotate_, const Vector3& scale_ = Vector3::identity);
	void Update();
	void Draw(const Mat4x4& viewProjection, const Vector3& cameraPos);

	bool Collision(const Bullet& bullet);

	inline bool GetIsDeath() const {
		return isDeath;
	}

	inline float GetAttack() const {
		return attack;
	}

public:
	static void LoadModel();
	static void UnloadModel();

public:
	Vector3 pos;
	Vector3 scale;
	Vector3 rotate;
	float spd;
	float radius;

	// state
	float attack;

private:
	static std::unique_ptr<Model> model;
	bool isCollision;
	
	Vector3 moveVec;
	

	Vector3 attenuation;
	float attenuationSpd;

	float attenuationAttack;

	std::chrono::steady_clock::time_point startTime;
	std::chrono::seconds deathTime;

	bool isDeath;
};