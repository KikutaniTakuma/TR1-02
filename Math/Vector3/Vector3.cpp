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

Vector3 Vector3::identity = {1.0f,1.0f,1.0f};
Vector3 Vector3::xIdy = { 1.0f,0.0f,0.0f };
Vector3 Vector3::yIdy = { 0.0f,1.0f,0.0f };
Vector3 Vector3::zIdy = { 0.0f,0.0f,1.0f };

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
	Vector4 vec = { x,y,z,1.0f };
	Vector4 vec1 = { mat[0][0], mat[1][0], mat[2][0], mat[3][0] };
	Vector4 vec2 = { mat[0][1], mat[1][1], mat[2][1], mat[3][1] };
	Vector4 vec3 = { mat[0][2], mat[1][2], mat[2][2], mat[3][2] };
	Vector4 vec4 = { mat[0][3], mat[1][3], mat[2][3], mat[3][3] };

	result.x = vec.Dot(vec1);
	result.y = vec.Dot(vec2);
	result.z = vec.Dot(vec3);
	float&& w = vec.Dot(vec4);
	assert(w != 0.0f);
	w = 1.0f / w;
	result.x *= w;
	result.y *= w;
	result.z *= w;

	return result;
}

Vector3 operator*(const Mat4x4& left, const Vector3& right) {
	Vector3 result;
	Vector4 vec = { right.x,right.y,right.z,1.0f };

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
	return sqrtf(x*x + y*y + z*z);
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