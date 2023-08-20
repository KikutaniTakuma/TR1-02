#include "Bullet.h"

std::unique_ptr<Model> Bullet::model;

Bullet::Bullet():
	isCollision(false),
	spd(50.0f),
	moveVec(Vector3::zero),
	radius(5.0f)
{
	if (!model) {
		model->LoadObj("./Resources/Ball.obj");
		model->LoadShader();
		model->CreateGraphicsPipeline();
	}
}