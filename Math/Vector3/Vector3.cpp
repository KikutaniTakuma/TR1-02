#include "Vector3.h"
#include <cmath>
#include "Math/Mat4x4/Mat4x4.h"
#include "Math/Vector2/Vector2.h"
#include <cassert>

Vector3::Vector3() noexcept :
	x(0.0f),
	y(0.0f),
	z(0.0f)
{}

Vector3::Vector3(float x, float y, float z) noexcept :
	x(x),
	y(y),
	z(z)
{}

Vector3::Vector3(const Vector3& right) noexcept
{
	*this = right;
}

Vector3::Vector3(Vector3&& right) noexcept
{
	*this = std::move(right);
}

Vector3 const Vector3::identity = {1.0f,1.0f,1.0f};
Vector3 const Vector3::xIdy = { 1.0f,0.0f,0.0f };
Vector3 const Vector3::yIdy = { 0.0f,1.0f,0.0f };
Vector3 const Vector3::zIdy = { 0.0f,0.0f,1.0f };

Vector3& Vector3::operator=(const Vector3& right) noexcept {
	x = right.x;
	y = right.y;
	z = right.z;

	return *this;
}

Vector3& Vector3::operator=(Vector3&& right) noexcept {
	x = std::move(right.x);
	y = std::move(right.y);
	z = std::move(right.z);

	return *this;
}

Vector3 Vector3::operator+(const Vector3& right) const noexcept {
	Vector3 tmp(x + right.x, y + right.y, z + right.z);

	return tmp;
}
Vector3 Vector3::operator-(const Vector3& right) const noexcept {
	Vector3 tmp(x - right.x, y - right.y, z - right.z);

	return tmp;
}
Vector3& Vector3::operator+=(const Vector3& right) noexcept {
	*this = *this + right;

	return *this;
}
Vector3& Vector3::operator-=(const Vector3& right) noexcept {
	*this = *this - right;

	return *this;
}

Vector3 Vector3::operator*(float scalar) const noexcept {
	Vector3 tmp(x * scalar, y * scalar, z * scalar);

	return tmp;
}
Vector3& Vector3::operator*=(float scalar) noexcept {
	*this = *this * scalar;

	return *this;
}

Vector3 Vector3::operator*(const Mat4x4& mat) const noexcept {
	Vector3 result;
	Vector4 vec = { *this,1.0f };
	auto tmp = mat;
	tmp.Transepose();

	result.x = vec.Dot(tmp[0]);
	result.y = vec.Dot(tmp[1]);
	result.z = vec.Dot(tmp[2]);
	float&& w = vec.Dot(tmp[3]);
	assert(w != 0.0f);
	w = 1.0f / w;
	result.x *= w;
	result.y *= w;
	result.z *= w;

	return result;
}

Vector3 operator*(const Mat4x4& left, const Vector3& right) {
	Vector3 result;
	Vector4 vec = { right,1.0f };

	result.x = left[0].Dot(vec);
	result.y = left[1].Dot(vec);
	result.z = left[2].Dot(vec);
	float&& w = left[3].Dot(vec);
	assert(w != 0.0f);
	w = 1.0f / w;
	result.x *= w;
	result.y *= w;
	result.z *= w;

	return result;
}

Vector3& Vector3::operator*=(const Mat4x4& mat) noexcept {
	*this = *this * mat;

	return *this;
}

Vector3& Vector3::operator=(const Vector2& right) noexcept {
	x = right.x;
	y = right.y;
	z = 0.0f;

	return *this;
}

bool Vector3::operator==(const Vector3& right) const noexcept {
	return x == right.x && y == right.y;
}
bool Vector3::operator!=(const Vector3& right) const noexcept {
	return x != right.x || y != right.y;
}

float Vector3::Length() const noexcept {
	return std::sqrt(x*x + y*y + z*z);
}

float Vector3::Dot(const Vector3& right) const noexcept {
	return x * right.x + y * right.y + z * right.z;
}

Vector3 Vector3::Cross(const Vector3& right) const noexcept {
	Vector3 result;

	result.x = y * right.z - z * right.y;
	result.y = z * right.x - x * right.z;
	result.z = x * right.y - y * right.x;

	return result;
}

Vector3 Vector3::Normalize() const noexcept {
	if (*this == Vector3()) {
		return *this;
	}

	float nor = 1.0f / this->Length();

	return *this * nor;
}