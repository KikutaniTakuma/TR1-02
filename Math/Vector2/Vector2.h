#pragma once

class Vector2 {
public:
	Vector2();
	Vector2(const float& X, const float& Y);

	Vector2(const Vector2& num);

	inline ~Vector2(){}

public:
	static Vector2 identity;
	static Vector2 xIdy;
	static Vector2 yIdy;

public:
	float x;
	float y;

public:

	Vector2 operator+(const Vector2& num) const;
	Vector2 operator-(const Vector2& num) const;
	Vector2 operator*(float num) const;
	Vector2 operator/(float num) const;
	const Vector2& operator=(const Vector2& num);
	const Vector2& operator+=(const Vector2& num);
	const Vector2& operator-=(const Vector2& num);
	const Vector2& operator*=(float num);
	const Vector2& operator/=(float num);
	bool operator==(const Vector2& num) const;
	bool operator!=(const Vector2& num) const;

	// =================================================================================

	void Rotate(float rad);

	float Cross(const Vector2& vec) const;
	float Dot(const Vector2& vec) const;
	float Length() const;

	Vector2 Normalize() const;
};