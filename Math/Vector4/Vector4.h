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
	Vector4& operator=(const class Vector3& right) noexcept;
	Vector4& operator=(const class Vector2& right) noexcept;
	Vector4& operator=(Vector4&& right) noexcept;

	Vector4 operator+(const Vector4& right) const noexcept;
	Vector4& operator+=(const Vector4& right) noexcept;

	Vector4 operator-(const Vector4& right) const noexcept;
	Vector4& operator-=(const Vector4& right) noexcept;

	Vector4 operator*(float scalar) const noexcept;
	Vector4& operator*=(float scalar) noexcept;
	Vector4 operator/(float scalar) const noexcept;
	Vector4& operator/=(float scalar) noexcept;

	Vector4 operator*(const class Mat4x4& mat) const noexcept;
	Vector4& operator*=(const class Mat4x4& mat) noexcept;

	bool operator==(const Vector4& right) const noexcept;
	bool operator!=(const Vector4& right) const noexcept;

	float& operator[](size_t index) noexcept;
	const float& operator[](size_t index) const noexcept;

public:
	float Length() const noexcept;

	Vector4 Normalize() const noexcept;

	float Dot(const Vector4& right) const noexcept;

	class Vector3 getVector3D() const noexcept;
	class Vector2 getVector2D() const noexcept;

public:
	union {
		std::array<float, 4> m;
		Vector4::Vector vec;
		Vector4::Color color;
	};
};