#include "Vector2.h"
#include <assert.h>
#include <cmath>
#include <numbers>

Vector2::Vector2() noexcept :
	x(0.0f),
	y(0.0f)
{}

Vector2::Vector2(const Vector2& right) noexcept {
	*this = right;
}
Vector2::Vector2(float x, float y) noexcept :
	x(x),
	y(y)
{}

Vector2::Vector2(Vector2&& right) noexcept {
	*this = std::move(right);
}

const Vector2 Vector2::identity = { 1.0f,1.0f };
const Vector2 Vector2::zero = {0.0f, 0.0f};
const Vector2 Vector2::xIdy = { 1.0f,0.0f };
const Vector2 Vector2::yIdy = { 0.0f,1.0f };


Vector2 Vector2::operator+(const Vector2& right) const noexcept {
	Vector2 result;

	result.x = this->x + right.x;
	result.y = this->y + right.y;

	return result;
};
Vector2 Vector2::operator-(const Vector2& right) const noexcept {
	Vector2 result;

	result.x = this->x - right.x;
	result.y = this->y - right.y;

	return result;
}


Vector2 Vector2::operator*(float scalar) const noexcept {
	Vector2 result;

	result.x = this->x * scalar;
	result.y = this->y * scalar;

	return result;
}
Vector2 Vector2::operator/(float scalar) const noexcept {
	Vector2 result;

	result.x = this->x / scalar;
	result.y = this->y / scalar;

	return result;
}

Vector2& Vector2::operator=(const Vector2& right) noexcept {
	this->x = right.x;
	this->y = right.y;

	return *this;
}

Vector2& Vector2::operator=(Vector2&& right) noexcept {
	this->x = std::move(right.x);
	this->y = std::move(right.y);

	return *this;
}


Vector2& Vector2::operator+=(const Vector2& right) noexcept {
	this->x += right.x;
	this->y += right.y;

	return *this;
}
Vector2& Vector2::operator-=(const Vector2& right) noexcept {
	this->x -= right.x;
	this->y -= right.y;

	return *this;
}

Vector2& Vector2::operator*=(float scalar) noexcept {
	this->x *= scalar;
	this->y *= scalar;

	return *this;
}
Vector2& Vector2::operator/=(float scalar) noexcept {
	this->x /= scalar;
	this->y /= scalar;

	return *this;
}

bool Vector2::operator==(const Vector2& right) const noexcept {
	return this->x == right.x && this->y == right.y;
}
bool Vector2::operator!=(const Vector2& right) const noexcept {
	return this->x != right.x && this->y != right.y;
}

void Vector2::Rotate(float rad) noexcept {
	x *= cosf(rad);
	y *= sinf(rad);
}

float Vector2::Cross(const Vector2& right) const noexcept {
	return x * right.y - y * right.x;
}
float Vector2::Dot(const Vector2& right) const noexcept {
	return x * right.x + y * right.y;
}

float Vector2::Length() const noexcept {
	return std::hypot(x, y);
}

Vector2 Vector2::Normalize() const noexcept {
	if (*this == Vector2::zero) {
		return *this;
	}
	return *this / Length();
}

float Vector2::GetRad() const noexcept {
	float result = 0.0f;
	if (*this == Vector2::zero) {
		return 0.0f;
	}

	Vector2 normalize = this->Normalize();

	result = std::atan2f(normalize.y, normalize.x);

	return result;
}