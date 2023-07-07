#pragma once

#include <array>
#include <string>

static const int kRowHeight = 20;
static const int kColumnWidth = 60;

class Vector3;

class Mat4x4 final {
public:
	Mat4x4();

	Mat4x4(const Mat4x4& mat);
	Mat4x4(Mat4x4&& mat);

	Mat4x4(const std::array<std::array<float, 4>, 4>& num);

	~Mat4x4() = default;

private:
	static const int HEIGHT = 4;
	static const int WIDTH = 4;

private:
	std::array<std::array<float, 4>, 4> m;

public:
	Mat4x4& operator=(const Mat4x4& mat);
	Mat4x4& operator=(Mat4x4&& mat);
	Mat4x4 operator*(const Mat4x4& mat) const;
	Mat4x4& operator*=(const Mat4x4& mat);

	Mat4x4 operator+(const Mat4x4& mat) const;
	Mat4x4& operator+=(const Mat4x4& mat);
	Mat4x4 operator-(const Mat4x4& mat) const;
	Mat4x4& operator-=(const Mat4x4& mat);

	std::array<float, 4>& operator[](size_t index);

	bool operator==(const Mat4x4& mat) const;
	bool operator!=(const Mat4x4& mat) const;

	inline std::array<std::array<float, 4>, 4> get() const {
		return m;
	}

public:
	void Indentity();

	void Translate(const Vector3& vec);

	void Scalar(const Vector3& vec);

	void RotateX(float rad);

	void RotateY(float rad);

	void RotateZ(float rad);

	void Affin(const Vector3& scale, const Vector3& rad, const Vector3& translate);

	void Inverse();

	void Transepose();

	void PerspectiveFov(float fovY, float aspectRatio, float nearClip, float farClip);

	void Orthographic(float left, float right, float top, float bottom, float nearClip, float farClip);

	void ViewPort(float left, float top, float width, float height, float minDepth, float maxDepth);
};


Mat4x4 MakeMatrixIndentity();

Mat4x4 MakeMatrixTranslate(Vector3 vec);

Mat4x4 MakeMatrixScalar(Vector3 vec);

Mat4x4 MakeMatrixRotateX(float rad);

Mat4x4 MakeMatrixRotateY(float rad);

Mat4x4 MakeMatrixRotateZ(float rad);

Mat4x4 MakeMatrixAffin(const Vector3& scale, const Vector3& rad, const Vector3& translate);

Mat4x4 MakeMatrixInverse(Mat4x4 mat);

Mat4x4 MakeMatrixTransepose(Mat4x4 mat);

Mat4x4 MakeMatrixPerspectiveFov(float fovY, float aspectRatio, float nearClip, float farClip);

Mat4x4 MakeMatrixOrthographic(float left, float top, float right, float bottom, float nearClip, float farClip);

Mat4x4 MakeMatrixViewPort(float left, float top, float width, float height, float minDepth, float maxDepth);