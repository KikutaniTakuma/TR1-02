#pragma once
#include "Math/Vector2/Vector2.h"
#include "Math/Vector3/Vector3.h"
#include "Math/Vector4/Vector4.h"
#include "Math/Mat4x4/Mat4x4.h"

class Camera {
public:
	enum class Mode : uint8_t {
		Projecction,
		Othographic
	};

public:
	Camera() noexcept;
	Camera(Camera::Mode mode) noexcept;
	Camera(const Camera& right) noexcept;
	Camera(Camera&& right) noexcept;
	~Camera() noexcept = default;

public:
	Camera& operator=(const Camera& right) noexcept;
	Camera& operator=(Camera&& right) noexcept;

public:
	void Update();
	void Update(const Vector3& gazePoint, const Vector3& rotation);

	// 透視投影
	inline const Mat4x4& GetViewProjection() noexcept {
		return viewProjecction;
	}
	// 平衡投影
	inline const Mat4x4& GetViewOthographics() noexcept {
		return viewOthograohics;
	}

	//void SetViewPort();

public:
	Mode mode;

public:
	Vector3 pos;
	Vector3 scale;
	Vector3 rotate;

private:
	const float kNearClip;

public:
	float farClip;
	float fov;

private:
	Mat4x4 view;
	Mat4x4 projection;
	Mat4x4 othograohics;

	Mat4x4 viewProjecction;
	Mat4x4 viewOthograohics;
};