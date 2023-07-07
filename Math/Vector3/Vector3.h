#pragma once
#include <string>

class Mat4x4;

class Vector3 {
public:
	Vector3();
	Vector3(float x, float y, float z);
	Vector3(const Vector3& vec) = default;

public:
	static Vector3 identity;
	static Vector3 xIdy;
	static Vector3 yIdy;
	static Vector3 zIdy;

public:
	float x;
	float y;
	float z;

public:
	Vector3& operator=(const Vector3& vec) = default;
	Vector3 operator+(const Vector3& vec) const;
	Vector3 operator-(const Vector3& vec) const;
	Vector3& operator+=(const Vector3& vec);
	Vector3& operator-=(const Vector3& vec);
	Vector3 operator*(float scalar) const;
	Vector3& operator*=(float scalar);
	Vector3 operator*(const Mat4x4& mat) const;
	Vector3& operator*=(const Mat4x4& mat);

	bool operator==(const Vector3& vec) const;
	bool operator!=(const Vector3& vec) const;

	float Dot(const Vector3& vec) const;
	Vector3 Cross(const Vector3& vec) const;
	float Length() const;
	Vector3 Normalize() const;
};