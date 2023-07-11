#pragma once

class Vector2 final {
/// <summary>
/// �R���X�g���N�^
/// </summary>
public:
	Vector2() noexcept;
	Vector2(float x, float y) noexcept;

	Vector2(const Vector2& right) noexcept;
	Vector2(Vector2&& right) noexcept;
public:
	~Vector2() = default;

/// <summary>
/// ���Z�q�̃I�[�o�[���[�h
/// </summary>
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

/// <summary>
/// �����o�֐�
/// </summary>
public:
	void Rotate(float rad) noexcept;

	float Cross(const Vector2& right) const noexcept;
	float Dot(const Vector2& right) const noexcept;
	float Length() const noexcept;

	Vector2 Normalize() const noexcept;

/// <summary>
/// �ÓI�萔
/// </summary>
public:
	static const Vector2 identity;
	static const Vector2 xIdy;
	static const Vector2 yIdy;

/// <summary>
/// �����o�ϐ�
/// </summary>
public:
	float x;
	float y;
};