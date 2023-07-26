#include "Camera.h"
#include "Engine/Engine.h"

Camera::Camera() noexcept :
	mode(Mode::Projecction),
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

Camera::Camera(Camera::Mode mode) noexcept :
	mode(mode),
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
	kNearClip(right.kNearClip)
{
	*this = right;
}

Camera::Camera(Camera&& right) noexcept :
	kNearClip(right.kNearClip)
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
	view.VertAffin(scale, rotate, pos);
	view.Inverse();

	static auto engine = Engine::GetInstance();
	static const float aspect = static_cast<float>(engine->clientWidth) / static_cast<float>(engine->clientHeight);

	switch (mode)
	{
	case Camera::Mode::Projecction:
	default:
		fov = std::clamp(fov, 0.0f, 1.0f);
		projection.VertPerspectiveFov(fov, aspect, kNearClip, farClip);
		viewProjecction = projection * view;
		break;

	case Camera::Mode::Othographic:
		othograohics.VertOrthographic(
			-static_cast<float>(engine->clientWidth) * 0.5f,
			static_cast<float>(engine->clientHeight) * 0.5f,
			static_cast<float>(engine->clientWidth) * 0.5f,
			-static_cast<float>(engine->clientHeight) * 0.5f,
			kNearClip, farClip);
		viewOthograohics = othograohics * view;
		break;
	}
}

void Camera::Update(const Vector3& gazePoint, const Vector3& rotation) {
	view.VertAffin(scale, rotate, pos);
	view = VertMakeMatrixAffin(Vector3::identity, rotation, Vector3()) * VertMakeMatrixTranslate(gazePoint) * view;
	view.Inverse();

	static auto engine = Engine::GetInstance();
	static const float aspect = static_cast<float>(engine->clientWidth) / static_cast<float>(engine->clientHeight);

	switch (mode)
	{
	case Camera::Mode::Projecction:
	default:
		fov = std::clamp(fov, 0.0f, 1.0f);
		projection.VertPerspectiveFov(fov, aspect, kNearClip, farClip);
		viewProjecction = projection * view;
		break;

	case Camera::Mode::Othographic:
		othograohics.VertOrthographic(
			-static_cast<float>(engine->clientWidth) * 0.5f,
			static_cast<float>(engine->clientHeight) * 0.5f,
			static_cast<float>(engine->clientWidth) * 0.5f,
			-static_cast<float>(engine->clientHeight) * 0.5f,
			kNearClip, farClip);
		viewOthograohics = othograohics * view;
		break;
	}
}