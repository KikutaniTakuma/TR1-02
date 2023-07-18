#pragma once
#include <array>
#include <immintrin.h>

class Vector4 final {
	/// <summary>
	/// サブクラス
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
	/// コンストラクタ
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
	/// 演算子のオーバーロード
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
	/// メンバ関数
	/// </summary>
public:
	float Length() const noexcept;

	Vector4 Normalize() const noexcept;

	float Dot(const Vector4& right) const noexcept;

	class Vector3 GetVector3() const noexcept;
	class Vector2 GetVector2() const noexcept;

	/// <summary>
	/// メンバ変数
	/// </summary>
public:
	union {
		std::array<float, 4> m;
		Vector4::Vector vec;
		Vector4::Color color;
		__m128 m128;
	};

};

/// <summary>
/// uint32_tからVector4への変換
/// </summary>
Vector4 UintToVector4(uint32_t color);