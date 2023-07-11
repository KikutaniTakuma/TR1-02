#pragma once
#include <array>

class Vector4 final {
/// <summary>
/// �T�u�N���X
/// </summary>
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

/// <summary>
/// �R���X�g���N�^
/// </summary>
public:
	Vector4() noexcept;
	Vector4(const Vector4& right) noexcept;
	Vector4(Vector4&& right) noexcept;
	Vector4(float x, float y, float z, float w) noexcept;
	Vector4(const class Vector3& vec3, float w) noexcept;
	Vector4(const class Vector2& vec2, float z, float w) noexcept;
public:
	~Vector4() = default;

/// <summary>
/// ���Z�q�̃I�[�o�[���[�h
/// </summary>
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
	friend Vector4 operator*(const class Mat4x4& left, const Vector4& right) noexcept;

	bool operator==(const Vector4& right) const noexcept;
	bool operator!=(const Vector4& right) const noexcept;

	float& operator[](size_t index) noexcept;
	const float& operator[](size_t index) const noexcept;

/// <summary>
/// �����o�֐�
/// </summary>
public:
	float Length() const noexcept;

	Vector4 Normalize() const noexcept;

	float Dot(const Vector4& right) const noexcept;

	class Vector3 getVector3D() const noexcept;
	class Vector2 getVector2D() const noexcept;

/// <summary>
/// �����o�ϐ�
/// </summary>
public:
	union {
		std::array<float, 4> m;
		Vector4::Vector vec;
		Vector4::Color color;
	};
};