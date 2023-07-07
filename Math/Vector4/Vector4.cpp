#include "Vector4.h"
#include "Math/Mat4x4/Mat4x4.h"
#include "Math/Vector3/Vector3.h"
#include "Math/Vector2/Vector2.h"
#include <cmath>

Vector4::Vector4() noexcept :
	m{0.0f}
{}

Vector4::Vector4(const Vector4& right) noexcept :
	m(right.m)
{}
Vector4::Vector4(Vector4&& right)  noexcept :
	m(std::move(right.m))
{}

Vector4::Vector4(float x, float y, float z, float w) noexcept :
	m{x,y,z,w}
{}

Vector4& Vector4::operator=(const Vector4& right) noexcept {
	std::copy(right.m.begin(), right.m.end(), m.begin());

	return *this;
}

Vector4& Vector4::operator=(const Vector3& right) noexcept {
	vec.x = right.x;
	vec.y = right.y;
	vec.z = right.z;
	vec.w = 0.0f;

	return *this;
}

Vector4& Vector4::operator=(const Vector2& right) noexcept {
	vec.x = right.x;
	vec.y = right.y;
	vec.z = 0.0f;
	vec.w = 0.0f;

	return *this;
}

Vector4& Vector4::operator=(Vector4&& right) noexcept {
	m = std::move(right.m);

	return *this;
}

Vector4 Vector4::operator+(const Vector4& right) const noexcept {
	Vector4 result;

	for (size_t i = 0; i < result.m.size(); i++) {
		result.m[i] = m[i] + right.m[i];
	}

	return result;
}

Vector4& Vector4::operator+=(const Vector4& right) noexcept {
	for (size_t i = 0; i < m.size(); i++) {
		m[i] += right.m[i];
	}

	return *this;
}

Vector4 Vector4::operator-(const Vector4& right) const noexcept {
	Vector4 result;

	for (size_t i = 0; i < result.m.size(); i++) {
		result.m[i] = m[i] - right.m[i];
	}

	return result;
}

Vector4& Vector4::operator-=(const Vector4& right) noexcept {
	for (size_t i = 0; i < m.size(); i++) {
		m[i] -= right.m[i];
	}

	return *this;
}

Vector4 Vector4::operator*(float scalar) const noexcept {
	Vector4 result;

	for (size_t i = 0; i < result.m.size(); i++) {
		result.m[i] = m[i] * scalar;
	}

	return result;
}
Vector4& Vector4::operator*=(float scalar) noexcept {
	for (auto& i : m) {
		i *= scalar;
	}

	return *this;
}

Vector4 Vector4::operator/(float scalar) const noexcept {
	Vector4 result;

	for (size_t i = 0; i < result.m.size(); i++) {
		result.m[i] = m[i] / scalar;
	}

	return result;
}
Vector4& Vector4::operator/=(float scalar) noexcept {
	for (auto& i : m) {
		i /= scalar;
	}

	return *this;
}

Vector4 Vector4::operator*(const class Mat4x4& mat) const noexcept {
	Vector4 result;

	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {
			result.m[y] += this->m[x] * mat.get()[y][x];
		}
	}

	return result;
}

Vector4& Vector4::operator*=(const class Mat4x4& mat) noexcept {
	Vector4 result;

	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {
			result.m[y] += this->m[x] * mat.get()[y][x];
		}
	}

	*this = result;

	return *this;
}

bool Vector4::operator==(const Vector4& right) const noexcept {
	return m == right.m;
}
bool Vector4::operator!=(const Vector4& right) const noexcept {
	return m != right.m;
}

float Vector4::Length() const noexcept {
	return std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w);
}

Vector4 Vector4::Normalize() const noexcept {
	if (*this == Vector4()) {
		return *this;
	}

	float nor = 1.0f / this->Length();

	return Vector4(*this) * nor;
}

Vector3 Vector4::getVector3D() const noexcept {
	return Vector3(vec.x, vec.y, vec.z);
}
Vector2 Vector4::getVector2D() const noexcept {
	return Vector2(vec.x, vec.y);
}