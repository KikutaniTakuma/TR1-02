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
	void Initialize(const Vector3 pos);
	void Update();
	void Draw(const Mat4x4& viewProjection);
	void Finalize();

	void Collision(const Bullet& bullet);

private:
	static std::unique_ptr<Model> model;
	bool isCollision;
	float spd;
	Vector3 moveVec;
	float radius;
};