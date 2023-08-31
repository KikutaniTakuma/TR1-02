#include "Bullet.h"
#include "externals/imgui/imgui.h"

std::unique_ptr<Model> Bullet::model;

void Bullet::LoadModel() {
	if (!model) {
		model = std::make_unique<Model>(1000);
		model->LoadObj("./Resources/Ball.obj");
		model->LoadShader();
		model->CreateGraphicsPipeline();
	}
}
void Bullet::UnloadModel() {
	model.reset();
}

Bullet::Bullet():
	isCollision(false),
	spd(15.0f),
	moveVec(),
	radius(5.0f),
	attenuation(),
	attenuationSpd(radius * 0.5f),
	pos(),
	scale(Vector3::identity),
	rotate(),
	startTime(),
	deathTime(3),
	isDeath(false),
	attack(10.0f),
	attenuationAttack()
{
	
}

Bullet::Bullet(const Bullet& right) :
	Bullet()
{
	*this = right;
}
Bullet::Bullet(Bullet&& right) noexcept :
	Bullet()
{
	*this = std::move(right);
}

Bullet::~Bullet() {
	
}

Bullet& Bullet::operator=(const Bullet& right) {
	isCollision = right.isCollision;
	spd = right.spd;
	moveVec = right.moveVec;
	radius = right.radius;

	startTime = right.startTime;
	deathTime = right.deathTime;

	return *this;
}
Bullet& Bullet::operator=(Bullet&& right) noexcept {
	isCollision = std::move(right.isCollision);
	spd = std::move(right.spd);
	moveVec = std::move(right.moveVec);
	radius = std::move(right.radius);

	startTime = std::move(right.startTime);
	deathTime = std::move(right.deathTime);

	return *this;
}

void Bullet::Initialize(const Vector3& pos_, const Vector3& rotate_, const Vector3& scale_) {
	pos = pos_;
	rotate = rotate_;
	scale = scale_;
	moveVec = Vector3::zIdy * spd;
	moveVec *= HoriMakeMatrixRotateY(rotate.y);
	
	attenuation = -moveVec;
	attenuation = attenuation.Normalize();
	attenuation *= attenuationSpd;

	startTime = std::chrono::steady_clock::now();

	attenuationAttack = attack / 3.0f;
}
void Bullet::Update() {
	auto nowTime = std::chrono::steady_clock::now();
	if (std::chrono::duration_cast<std::chrono::seconds>(nowTime - startTime)
		> deathTime
		) {
		isDeath = true;
	}

	if (moveVec.Length() >= 0.01f && !isDeath) {
		pos += moveVec * ImGui::GetIO().DeltaTime;
		moveVec += attenuation * ImGui::GetIO().DeltaTime;
		attack -= attenuationAttack * ImGui::GetIO().DeltaTime;
	}

	model->Update();
}
void Bullet::Draw(const Mat4x4& viewProjection, const Vector3& cameraPos) {
	model->pos = pos;
	model->scale = scale;

	model->Draw(viewProjection, cameraPos);
}

bool Bullet::Collision(Bullet& bullet) {
	Vector3 length = pos - bullet.pos;
	if (radius + bullet.radius >= length.Length()) {
		isDeath = true;
		bullet.isDeath = true;
		return true;
	}
	return false;
}

bool Bullet::Collision(const Vector3& pos_, float radius_) {
	Vector3 length = pos - pos_;
	if (radius + radius_ >= length.Length()) {
		isDeath = true;
		return true;
	}
	return false;
}