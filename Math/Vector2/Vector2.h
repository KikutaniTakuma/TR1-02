#pragma once

class Vector2 final {
public:
	Vector2() noexcept;
	Vector2(float x, float y) noexcept;

	Vector2(const Vector2& right) noexcept;
	Vector2(Vector2&& right) noexcept;

	~Vector2() = default;

public:
	static Vector2 identity;
	static Vector2 xIdy;
	static Vector2 yIdy;

public:
	float x;
	float y;

public:

	Vector2 operator+(const Vector2& right) const noexcept;
	Vector2 operator-(const Vector2& right) const noexcept;
	Vector2 operator*(float scalar) const noexcept;
	Vector2 operator/(float scalar) const noexcept;
	Vector2& operator=(const Vector2& right) noexcept;
	Vector2& operator=(Vector2&& right) noexcept;
	Vector2& operator+=(const Vector2& right) noexcept;
	Vector2& operator-=(const Vector2& right) noexcept;
	Vector2& operator*=(float scalar) noexcept;
	Vector2& operator/=(float scalar) noexcept;
	bool operator==(const Vector2& right) const noexcept;
	bool operator!=(const Vector2& right) const noexcept;

	// =================================================================================

	void Rotate(float rad) noexcept;

	float Cross(const Vector2& right) const noexcept;
	float Dot(const Vector2& right) const noexcept;
	float Length() const noexcept;

	Vector2 Normalize() const noexcept;
};