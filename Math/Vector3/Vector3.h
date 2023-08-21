#pragma once
#include <string>

class Vector3 final {
/// <summary>
/// コンストラクタ
/// </summary>
public:
	Vector3() noexcept;
	Vector3(float x, float y, float z) noexcept;
	Vector3(const Vector3& right) noexcept;
	Vector3(const class Vector2& right) noexcept;
	Vector3(Vector3&& right) noexcept;
public:
	~Vector3() = default;

/// <summary>
/// 演算子のオーバーロード
/// </summary>
public:
	// 単項演算子
	Vector3 operator+() const noexcept;
	Vector3 operator-() const noexcept;


	// 二項演算子

	Vector3& operator=(const Vector3& right) noexcept;
	Vector3& operator=(Vector3&& right) noexcept;
	Vector3 operator+(const Vector3& right) const noexcept;
	Vector3 operator-(const Vector3& right) const noexcept;
	Vector3& operator+=(const Vector3& right) noexcept;
	Vector3& operator-=(const Vector3& right) noexcept;
	Vector3 operator*(float scalar) const noexcept;
	Vector3& operator*=(float scalar) noexcept;
	Vector3 operator/(float scalar) const noexcept;
	Vector3& operator/=(float scalar) noexcept;

	Vector3 operator*(const class Mat4x4& mat) const noexcept;
	Vector3& operator*=(const class Mat4x4& mat) noexcept;
	friend Vector3 operator*(const class Mat4x4& left, const Vector3& right);

	Vector3& operator=(const class Vector2& right) noexcept;


	// 等比演算子

	bool operator==(const Vector3& right) const noexcept;
	bool operator!=(const Vector3& right) const noexcept;

/// <summary>
/// メンバ関数
/// </summary>
public:
	float Dot(const Vector3& right) const noexcept;
	Vector3 Cross(const Vector3& right) const noexcept;
	float Length() const noexcept;
	Vector3 Normalize() const noexcept;
	Vector3 GetRad()const noexcept;

/// <summary>
/// 静的定数
/// </summary>
public:
	/// <summary>
	/// x = 1.0f, y = 1.0f, z = 1.0f
	/// </summary>
	static const Vector3 identity;
	/// <summary>
	/// x = 0.0f, y = 0.0f, z = 0.0f
	/// </summary>
	static const Vector3 zero;
	/// <summary>
	/// x = 1.0f, y = 0.0f, z = 0.0f
	/// </summary>
	static const Vector3 xIdy;
	/// <summary>
	/// x = 0.0f, y = 1.0f, z = 0.0f
	/// </summary>
	static const Vector3 yIdy;
	/// <summary>
	/// x = 0.0f, y = 0.0f, z = 1.0f
	/// </summary>
	static const Vector3 zIdy;

/// <summary>
/// メンバ関数
/// </summary>
public:
	float x;
	float y;
	float z;
};