#include "Camera.h"
#include "Engine/Engine.h"
#include "Engine/KeyInput/KeyInput.h"
#include "Engine/Mouse/Mouse.h"
#include "Engine/Gamepad/Gamepad.h"
#include "externals/imgui/imgui.h"
#include "Engine/WinApp/WinApp.h"
#include <numbers>
#include <cmath>

Camera::Camera() noexcept :
	mode(Mode::Projecction),
	isDebug(false),
	pos(),
	scale(Vector3::identity),
	rotate(),
	drawScale(1.0f),
	moveVec(),
	moveSpd(1.65f),
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

void Camera::Update(const Vector3& gazePoint) {
	if (isDebug) {
		moveVec = Vector3();

		switch (mode)
		{
		case Camera::Mode::Projecction:
		default:
			moveSpd = 1.65f;
			if (Mouse::LongPush(Mouse::Button::Right) && (KeyInput::LongPush(DIK_LSHIFT) || KeyInput::LongPush(DIK_RSHIFT))) {
				auto moveRotate = Mouse::GetVelocity().Normalize() * moveRotateSpd;
				moveRotate.x *= -1.0f;

				rotate.x -= std::atan(moveRotate.y);
				rotate.y -= std::atan(moveRotate.x);
			}
			else if (Mouse::LongPush(Mouse::Button::Right)) {
				auto moveRotateBuf = Mouse::GetVelocity().Normalize() * gazePointRotateSpd;
				moveRotateBuf.x *= -1.0f;
				gazePointRotate -= moveRotateBuf;
			}
			if (Mouse::LongPush(Mouse::Button::Middle)) {
				moveVec = Mouse::GetVelocity().Normalize() * moveSpd * ImGui::GetIO().DeltaTime;
				moveVec *= HoriMakeMatrixRotateX(rotate.x) * HoriMakeMatrixRotateY(rotate.y) * HoriMakeMatrixRotateZ(rotate.z);
				pos -= moveVec;
			}
			if (Mouse::GetWheelVelocity() != 0.0f) {
				moveVec.z = Mouse::GetWheelVelocity();
				moveSpd = 6.6f;
				moveVec = moveVec.Normalize() * moveSpd * ImGui::GetIO().DeltaTime;
				moveVec *= HoriMakeMatrixRotateX(rotate.x) * HoriMakeMatrixRotateY(rotate.y) * HoriMakeMatrixRotateZ(rotate.z);
				pos += moveVec;
			}
			break;
		case Camera::Mode::Othographic:
			moveSpd = 15.0f;

			if (Mouse::LongPush(Mouse::Button::Middle)) {
				moveVec = Mouse::GetVelocity().Normalize() * moveSpd * ImGui::GetIO().DeltaTime;
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
				drawScale = std::clamp(drawScale, 0.01f, 10.0f);
			}
			break;
		}

		/*auto posTmp = pos + gazePoint;
		posTmp *= */
		view = VertMakeMatrixAffin(scale, rotate, pos);
		view = VertMakeMatrixAffin(Vector3::identity, Vector3(gazePointRotate.y, gazePointRotate.x, 0.0f), gazePoint) * view;
		worldPos = { view[0][3],view[1][3], view[2][3] };
		view.Inverse();
	}
	else {
		view.VertAffin(scale, rotate, pos + gazePoint);
		view = VertMakeMatrixAffin(Vector3::identity, Vector3(gazePointRotate.y, gazePointRotate.x, 0.0f), pos + gazePoint) * view;
		worldPos = { view[0][3],view[1][3], view[2][3] };
		view.Inverse();
	}
	static auto engine = Engine::GetInstance();
	static const float aspect = static_cast<float>(engine->clientWidth) / static_cast<float>(engine->clientHeight);

	const auto&& windowSize = WinApp::GetInstance()->GetWindowSize();

	switch (mode)
	{
	case Camera::Mode::Projecction:
	default:
		fov = std::clamp(fov, 0.0f, 1.0f);
		projection.VertPerspectiveFov(fov, aspect, kNearClip, farClip);
		viewProjecction = projection * view;

		viewProjecctionVp = VertMakeMatrixViewPort(0.0f, 0.0f, windowSize.x, windowSize.y, 0.0f, 1.0f) * viewProjecction;
		break;

	case Camera::Mode::Othographic:
		othograohics.VertOrthographic(
			-static_cast<float>(engine->clientWidth) * 0.5f * drawScale,
			static_cast<float>(engine->clientHeight) * 0.5f * drawScale,
			static_cast<float>(engine->clientWidth) * 0.5f * drawScale,
			-static_cast<float>(engine->clientHeight) * 0.5f * drawScale,
			kNearClip, farClip);
		viewOthograohics = othograohics * view;

		
		viewOthograohicsVp = VertMakeMatrixViewPort(0.0f, 0.0f, windowSize.x, windowSize.y, 0.0f, 1.0f) * viewOthograohics;
		break;
	}
}

void Camera::Update(const Mat4x4& worldMat) {
	view.VertAffin(scale, rotate, pos);
	view = worldMat * view;
	worldPos = { view[0][3],view[1][3], view[2][3] };
	view.Inverse();

	static auto engine = Engine::GetInstance();
	static const float aspect = static_cast<float>(engine->clientWidth) / static_cast<float>(engine->clientHeight);

	const auto&& windowSize = WinApp::GetInstance()->GetWindowSize();

	switch (mode)
	{
	case Camera::Mode::Projecction:
	default:
		fov = std::clamp(fov, 0.0f, 1.0f);
		projection.VertPerspectiveFov(fov, aspect, kNearClip, farClip);
		viewProjecction = projection * view;

		viewProjecctionVp = VertMakeMatrixViewPort(0.0f, 0.0f, windowSize.x, windowSize.y, 0.0f, 1.0f) * viewProjecction;
		break;

	case Camera::Mode::Othographic:
		othograohics.VertOrthographic(
			-static_cast<float>(engine->clientWidth) * 0.5f * drawScale,
			static_cast<float>(engine->clientHeight) * 0.5f * drawScale,
			static_cast<float>(engine->clientWidth) * 0.5f * drawScale,
			-static_cast<float>(engine->clientHeight) * 0.5f * drawScale,
			kNearClip, farClip);
		viewOthograohics = othograohics * view;


		viewOthograohicsVp = VertMakeMatrixViewPort(0.0f, 0.0f, windowSize.x, windowSize.y, 0.0f, 1.0f) * viewOthograohics;
		break;
	}
}