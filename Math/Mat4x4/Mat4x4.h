#pragma once

#include <array>
#include <string>
#include "Math/Vector4/Vector4.h"

static const int kRowHeight = 20;
static const int kColumnWidth = 60;

class Vector3;

class Mat4x4 final {
/// <summary>
/// コンストラクタ
/// </summary>
public:
	Mat4x4();
	Mat4x4(const Mat4x4& mat);
	Mat4x4(Mat4x4&& mat) noexcept;
	Mat4x4(const std::array<Vector4, 4>& num);
public:
	~Mat4x4() = default;

/// <summary>
/// 演算子のオーバーロード
/// </summary>
public:
	Mat4x4& operator=(const Mat4x4& mat);
	Mat4x4& operator=(Mat4x4&& mat) noexcept;
	Mat4x4 operator*(const Mat4x4& mat) const;
	Mat4x4& operator*=(const Mat4x4& mat);

	Mat4x4 operator+(const Mat4x4& mat) const;
	Mat4x4& operator+=(const Mat4x4& mat);
	Mat4x4 operator-(const Mat4x4& mat) const;
	Mat4x4& operator-=(const Mat4x4& mat);

	Vector4& operator[](size_t index);
	const Vector4& operator[](size_t index) const;

	bool operator==(const Mat4x4& mat) const;
	bool operator!=(const Mat4x4& mat) const;

/// <summary>
/// メンバ関数
/// </summary>
public:
	void Indentity();

	void HoriTranslate(const Vector3& vec);
	void VertTranslate(const Vector3& vec);

	void Scalar(const Vector3& vec);

	void HoriRotateX(float rad);
	void VertRotateX(float rad);

	void HoriRotateY(float rad);
	void VertRotateY(float rad);

	void HoriRotateZ(float rad);
	void VertRotateZ(float rad);

	void HoriAffin(const Vector3& scale, const Vector3& rad, const Vector3& translate);
	void VertAffin(const Vector3& scale, const Vector3& rad, const Vector3& translate);

	void Inverse();

	void Transepose();

	void HoriPerspectiveFov(float fovY, float aspectRatio, float nearClip, float farClip);
	void VertPerspectiveFov(float fovY, float aspectRatio, float nearClip, float farClip);

	void HoriOrthographic(float left, float right, float top, float bottom, float nearClip, float farClip);
	void VertOrthographic(float left, float right, float top, float bottom, float nearClip, float farClip);

	void HoriViewPort(float left, float top, float width, float height, float minDepth, float maxDepth);
	void VertViewPort(float left, float top, float width, float height, float minDepth, float maxDepth);

/// <summary>
/// 静的定数
/// </summary>
private:
	static const int HEIGHT = 4;
	static const int WIDTH = 4;

/// <summary>
/// メンバ変数
/// </summary>
private:
	std::array<Vector4, 4> m;
};


Mat4x4 MakeMatrixIndentity();

Mat4x4 HoriMakeMatrixTranslate(Vector3 vec);
Mat4x4 VertMakeMatrixTranslate(Vector3 vec);

Mat4x4 MakeMatrixScalar(Vector3 vec);

Mat4x4 HoriMakeMatrixRotateX(float rad);
Mat4x4 VertMakeMatrixRotateX(float rad);

Mat4x4 HoriMakeMatrixRotateY(float rad);
Mat4x4 VertMakeMatrixRotateY(float rad);

Mat4x4 HoriMakeMatrixRotateZ(float rad);
Mat4x4 VertMakeMatrixRotateZ(float rad);

Mat4x4 HoriMakeMatrixAffin(const Vector3& scale, const Vector3& rad, const Vector3& translate);
Mat4x4 VertMakeMatrixAffin(const Vector3& scale, const Vector3& rad, const Vector3& translate);

Mat4x4 MakeMatrixInverse(Mat4x4 mat);

Mat4x4 MakeMatrixTransepose(Mat4x4 mat);

Mat4x4 HoriMakeMatrixPerspectiveFov(float fovY, float aspectRatio, float nearClip, float farClip);
Mat4x4 VertMakeMatrixPerspectiveFov(float fovY, float aspectRatio, float nearClip, float farClip);

Mat4x4 HoriMakeMatrixOrthographic(float left, float top, float right, float bottom, float nearClip, float farClip);
Mat4x4 VertMakeMatrixOrthographic(float left, float top, float right, float bottom, float nearClip, float farClip);

Mat4x4 HoriMakeMatrixViewPort(float left, float top, float width, float height, float minDepth, float maxDepth);
Mat4x4 VertMakeMatrixViewPort(float left, float top, float width, float height, float minDepth, float maxDepth);