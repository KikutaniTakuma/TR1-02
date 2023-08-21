#include "Bullet.h"
#include "externals/imgui/imgui.h"

std::unique_ptr<Model> Bullet::model;

void Bullet::LoadModel() {
	if (!model) {
		model = std::make_unique<Model>();
		model->LoadObj("./Resources/Cube.obj");
		model->LoadShader();
		model->CreateGraphicsPipeline();
	}
}
void Bullet::UnloadModel() {
	model.reset();
}

Bullet::Bullet():
	isCollision(false),
	spd(5.0f),
	moveVec(),
	radius(5.0f),
	attenuation(),
	attenuationSpd(radius * 0.5f),
	pos(),
	size(Vector3::identity),
	rotate()
{
	if (!model) {
		model->LoadObj("./Resources/Ball.obj");
		model->LoadShader();
		model->CreateGraphicsPipeline();
	}
}

Bullet::Bullet(const Bullet& right) :
	isCollision(false),
	spd(5.0f),
	moveVec(Vector3::zero),
	radius(5.0f),
	attenuation(),
	attenuationSpd(radius * 0.5f),
	pos(),
	size(Vector3::identity),
	rotate()
{
	if (!model) {
		model->LoadObj("./Resources/Ball.obj");
		model->LoadShader();
		model->CreateGraphicsPipeline();
	}

	*this = right;
}
Bullet::Bullet(Bullet&& right) noexcept :
	isCollision(false),
	spd(5.0f),
	moveVec(Vector3::zero),
	radius(5.0f),
	attenuation(),
	attenuationSpd(radius * 0.5f),
	pos(),
	size(Vector3::identity),
	rotate()
{
	if (!model) {
		model->LoadObj("./Resources/Ball.obj");
		model->LoadShader();
		model->CreateGraphicsPipeline();
	}

	*this = std::move(right);
}

Bullet::~Bullet() {
	
}

Bullet& Bullet::operator=(const Bullet& right) {
	isCollision = right.isCollision;
	spd = right.spd;
	moveVec = right.moveVec;
	radius = right.radius;

	return *this;
}
Bullet& Bullet::operator=(Bullet&& right) noexcept {
	isCollision = std::move(right.isCollision);
	spd = std::move(right.spd);
	moveVec = std::move(right.moveVec);
	radius = std::move(right.radius);

	return *this;
}

void Bullet::Initialize(const Vector3& pos_, const Vector3& rotate_) {
	pos = pos_;
	rotate = rotate_;
	moveVec = Vector3::xIdy * spd;
	moveVec *= HoriMakeMatrixAffin(Vector3::identity, rotate, Vector3::zero);
	
	attenuation = -moveVec;
	attenuation = attenuation.Normalize();
	attenuation *= attenuationSpd;
}
void Bullet::Update() {
	if (moveVec.Length() >= 0.01f) {
		pos += moveVec * ImGui::GetIO().DeltaTime;
		moveVec += attenuation * ImGui::GetIO().DeltaTime;
	}

	model->Update();
}
void Bullet::Draw(const Mat4x4& viewProjection, const Vector3& cameraPos) {
	model->pos = pos;
	model->scale = size;

	model->Draw(viewProjection, cameraPos);
}

bool Bullet::Collision(const Bullet& bullet) {
	Vector3 length = pos - bullet.pos;
	if (radius + bullet.radius <= length.Length()) {
		return true;
	}
	return false;
}