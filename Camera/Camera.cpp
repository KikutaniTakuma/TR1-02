#include "Camera.h"
#include "Engine/Engine.h"

Camera::Camera() noexcept :
	pos(),
	scale(Vector3::identity),
	rotate(),
	kNearClip(0.01f),
	farClip(1000.0f),
	fov(0.45f),
	view(),
	projection(),
	othograohics()
{}

Camera::Camera(const Camera& right) noexcept :
	kNearClip(0.01f)
{
	*this = right;
}

Camera::Camera(Camera&& right) noexcept :
	kNearClip(0.01f)
{
	*this = std::move(right);
}

Camera& Camera::operator=(const Camera& right) noexcept {
	pos = right.pos;
	scale = right.scale;
	rotate = right.rotate;

	farClip = right.farClip;
	fov = right.fov;

	view = right.view;
	projection = right.projection;
	othograohics = right.othograohics;

	return *this;
}
Camera& Camera::operator=(Camera&& right) noexcept {
	pos = std::move(right.pos);
	scale = std::move(right.scale);
	rotate = std::move(right.rotate);

	farClip = std::move(right.farClip);
	fov = std::move(right.fov);

	view = std::move(right.view);
	projection = std::move(right.projection);
	othograohics = std::move(right.othograohics);

	return *this;
}

void Camera::Update() {
	view.Affin(scale, rotate, pos);
	view.Inverse();

	auto engine = Engine::GetInstance();

	float aspect = static_cast<float>(engine->clientWidth) / static_cast<float>(engine->clientHeight);
	fov = std::clamp(fov, 0.0f, 1.0f);
	projection.PerspectiveFov(fov, aspect, kNearClip, farClip);

	othograohics.Orthographic(
		-static_cast<float>(engine->clientWidth) * 0.5f,
		static_cast<float>(engine->clientHeight) * 0.5f,
		static_cast<float>(engine->clientWidth) * 0.5f,
		-static_cast<float>(engine->clientHeight) * 0.5f,
		kNearClip, farClip);

	viewProjecction = view * projection;
	viewOthograohics = view * othograohics;
}