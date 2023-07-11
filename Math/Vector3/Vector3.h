#pragma once
#include <string>

class Vector3 final {
public:
	Vector3() noexcept;
	Vector3(float x, float y, float z) noexcept;
	Vector3(const Vector3& right) noexcept;
	Vector3(Vector3&& right) noexcept;

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
	Vector3& operator=(const Vector3& right) noexcept;
	Vector3& operator=(Vector3&& right) noexcept;
	Vector3 operator+(const Vector3& right) const noexcept;
	Vector3 operator-(const Vector3& right) const noexcept;
	Vector3& operator+=(const Vector3& right) noexcept;
	Vector3& operator-=(const Vector3& right) noexcept;
	Vector3 operator*(float scalar) const noexcept;
	Vector3& operator*=(float scalar) noexcept;

	Vector3 operator*(const class Mat4x4& mat) const noexcept;
	Vector3& operator*=(const class Mat4x4& mat) noexcept;
	friend Vector3 operator*(const class Mat4x4& left, const Vector3& right);

	Vector3& operator=(const class Vector2& right) noexcept;


	bool operator==(const Vector3& right) const noexcept;
	bool operator!=(const Vector3& right) const noexcept;

	float Dot(const Vector3& right) const noexcept;
	Vector3 Cross(const Vector3& right) const noexcept;
	float Length() const noexcept;
	Vector3 Normalize() const noexcept;
};