#include "Camera.h"
#include "Engine/Engine.h"
#include "Engine/KeyInput/KeyInput.h"
#include "Engine/Mouse/Mouse.h"
#include "Engine/Gamepad/Gamepad.h"
#include "externals/imgui/imgui.h"
#include <numbers>

Camera::Camera() noexcept :
	mode(Mode::Projecction),
	isDebug(false),
	pos(),
	scale(Vector3::identity),
	rotate(),
	drawScale(1.0f),
	moveVec(),
	moveSpd(0.02f),
	moveRotate(),
	moveRotateSpd(std::numbers::pi_v<float> / 720.0f),
	gazePointRotate(),
	gazePointRotateSpd(std::numbers::pi_v<float> / 90.0f),
	farClip(1000.0f),
	fov(0.45f),
	view(),
	projection(),
	othograohics()
{}

Camera::Camera(Camera::Mode mode) noexcept :
	mode(mode),
	isDebug(false),
	pos(),
	scale(Vector3::identity),
	rotate(),
	drawScale(1.0f),
	moveVec(),
	moveSpd(0.02f),
	moveRotate(),
	moveRotateSpd(std::numbers::pi_v<float> / 720.0f),
	gazePointRotate(),
	gazePointRotateSpd(std::numbers::pi_v<float> / 9.0f),
	farClip(1000.0f),
	fov(0.45f),
	view(),
	projection(),
	othograohics()
{}

Camera::Camera(const Camera& right) noexcept
{
	*this = right;
}

Camera::Camera(Camera&& right) noexcept
{
	*this = std::move(right);
}

Camera& Camera::operator=(const Camera& right) noexcept {
	pos = right.pos;
	scale = right.scale;
	rotate = right.rotate;

	drawScale = right.drawScale;

	moveVec = right.moveVec;
	moveSpd = right.moveSpd;
	moveRotate = right.moveRotate;
	moveRotateSpd = right.moveRotateSpd;
	gazePointRotate = right.gazePointRotate;
	gazePointRotateSpd = right.gazePointRotateSpd;

	farClip = right.farClip;
	fov = right.fov;

	view = right.view;
	projection = right.projection;
	othograohics = right.othograohics;

	viewProjecction = right.viewProjecction;
	viewOthograohics = right.viewOthograohics;

	mode = right.mode;
	isDebug = right.isDebug;

	return *this;
}
Camera& Camera::operator=(Camera&& right) noexcept {
	pos = std::move(right.pos);
	scale = std::move(right.scale);
	rotate = std::move(right.rotate);

	drawScale = std::move(right.drawScale);

	moveVec = std::move(right.moveVec);
	moveSpd = std::move(right.moveSpd);
	moveRotate = std::move(right.moveRotate);
	moveRotateSpd = std::move(right.moveRotateSpd);
	gazePointRotate = std::move(right.gazePointRotate);
	gazePointRotateSpd = std::move(right.gazePointRotateSpd);

	farClip = std::move(right.farClip);
	fov = std::move(right.fov);

	view = std::move(right.view);
	projection = std::move(right.projection);
	othograohics = std::move(right.othograohics);

	viewProjecction = std::move(right.viewProjecction);
	viewOthograohics = std::move(right.viewOthograohics);

	mode = std::move(right.mode);
	isDebug = std::move(right.isDebug);

	return *this;
}

void Camera::Update(const Vector3& gazePoint) {
	if (isDebug) {
		moveVec = Vector3();

		switch (mode)
		{
		case Camera::Mode::Projecction:
		default:
			moveSpd = 0.02f;
			if (Mouse::LongPush(Mouse::Button::Right) && (KeyInput::LongPush(DIK_LSHIFT) || KeyInput::LongPush(DIK_RSHIFT))) {
				auto moveRotateBuf = Mouse::GetVelocity().Normalize() * moveRotateSpd;
				moveRotateBuf.x *= -1.0f;
				moveRotate += moveRotateBuf;
			}
			else if (Mouse::LongPush(Mouse::Button::Right)) {
				auto moveRotateBuf = Mouse::GetVelocity().Normalize() * gazePointRotateSpd;
				moveRotateBuf.x *= -1.0f;
				gazePointRotate -= moveRotateBuf;
			}
			if (Mouse::LongPush(Mouse::Button::Middle)) {
				moveVec = Mouse::GetVelocity().Normalize() * moveSpd;
				moveVec *= HoriMakeMatrixRotateX(rotate.x) * HoriMakeMatrixRotateY(rotate.y) * HoriMakeMatrixRotateZ(rotate.z);
				pos -= moveVec;
			}
			if (Mouse::GetWheelVelocity() != 0.0f) {
				moveVec.z = Mouse::GetWheelVelocity();
				moveVec = moveVec.Normalize() * moveSpd;
				moveVec *= HoriMakeMatrixRotateX(rotate.x) * HoriMakeMatrixRotateY(rotate.y) * HoriMakeMatrixRotateZ(rotate.z);
				pos += moveVec;
			}
			break;
		case Camera::Mode::Othographic:
			moveSpd = 15.0f;

			if (Mouse::LongPush(Mouse::Button::Middle)) {
				moveVec = Mouse::GetVelocity().Normalize() * moveSpd;
				moveVec *= HoriMakeMatrixRotateX(rotate.x) * HoriMakeMatrixRotateY(rotate.y) * HoriMakeMatrixRotateZ(rotate.z);
				pos -= moveVec * drawScale;
			}
			if (Mouse::GetWheelVelocity() != 0.0f) {
				moveVec.z = Mouse::GetWheelVelocity();
				if (drawScale <= 1.0f) {
					moveVec = moveVec.Normalize() * (moveSpd * 0.00005f);
				}
				else {
					moveVec = moveVec.Normalize() * (moveSpd * 0.001f);
				}
				drawScale -= moveVec.z;
				drawScale = std::clamp(drawScale, 0.1f, 10.0f);
			}
			break;
		}

		view = VertMakeMatrixAffin(scale, rotate, pos) * HoriMakeMatrixRotateY(moveRotate.x) * HoriMakeMatrixRotateX(moveRotate.y);
		view = VertMakeMatrixAffin(Vector3::identity, Vector3(gazePointRotate.y, gazePointRotate.x, 0.0f), Vector3()) * VertMakeMatrixTranslate(gazePoint) * view;
		view.Inverse();
	}
	else {
		view.VertAffin(scale, rotate, pos);
		view = VertMakeMatrixAffin(Vector3::identity, Vector3(gazePointRotate.y, gazePointRotate.x, 0.0f), Vector3()) * VertMakeMatrixTranslate(gazePoint) * view;
		view.Inverse();
	}
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
			-static_cast<float>(engine->clientWidth) * 0.5f * drawScale,
			static_cast<float>(engine->clientHeight) * 0.5f * drawScale,
			static_cast<float>(engine->clientWidth) * 0.5f * drawScale,
			-static_cast<float>(engine->clientHeight) * 0.5f * drawScale,
			kNearClip, farClip);
		viewOthograohics = othograohics * view;
		break;
	}
}