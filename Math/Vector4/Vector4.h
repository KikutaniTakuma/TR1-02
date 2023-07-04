#pragma once
#include <array>

class Vector4 final {
private:
	struct Vector {
		float x;
		float y;
		float z;
		float w;
	};

	struct Color {
		float r;
		float g;
		float b;
		float a;
	};

public:
	~Vector4() = default;

public:
	Vector4() noexcept;
	Vector4(const Vector4& right) noexcept;
	Vector4(Vector4&& right) noexcept;
	Vector4(float x, float y, float z, float w) noexcept;

public:
	Vector4& operator=(const Vector4& right) noexcept;
	Vector4& operator=(const class Vector3D& right) noexcept;
	Vector4& operator=(const class Vector2D& right) noexcept;
	Vector4& operator=(Vector4&& right) noexcept;

	Vector4 operator+(const Vector4& right) const noexcept;
	Vector4& operator+=(const Vector4& right) noexcept;

	Vector4 operator-(const Vector4& right) const noexcept;
	Vector4& operator-=(const Vector4& right) noexcept;

	Vector4 operator*(float scalar) const noexcept;
	Vector4& operator*=(float scalar) noexcept;

	Vector4 operator*(const class Mat4x4& mat) const noexcept;
	Vector4& operator*=(const class Mat4x4& mat) noexcept;

public:
	float Length() const noexcept;

	Vector4 Normalize() const noexcept;

	class Vector3D getVector3D() const noexcept;
	class Vector2D getVector2D() const noexcept;

public:
	union {
		std::array<float, 4> m;
		Vector4::Vector vec;
		Vector4::Color color;
	};
};