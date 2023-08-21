#pragma once
#include "Model/Model.h"

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
	void Initialize(const Vector3& pos_, const Vector3& rotate_);
	void Update();
	void Draw(const Mat4x4& viewProjection, const Vector3& cameraPos);

	bool Collision(const Bullet& bullet);

	static void LoadModel();
	static void UnloadModel();

private:
	static std::unique_ptr<Model> model;
	bool isCollision;
	float spd;
	Vector3 moveVec;
	float radius;

	Vector3 attenuation;
	float attenuationSpd;

	Vector3 pos;
	Vector3 size;
	Vector3 rotate;
};