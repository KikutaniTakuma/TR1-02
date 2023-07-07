#include "Vector2.h"
#include <assert.h>
#include <cmath>

Vector2::Vector2() {
	this->x = 0.0f;
	this->y = 0.0f;
}
Vector2::Vector2(const Vector2& num) {
	this->x = num.x;
	this->y = num.y;
}
Vector2::Vector2(const float& X, const float& Y) {
	this->x = X;
	this->y = Y;
}

Vector2 Vector2::identity = { 1.0f,1.0f };

Vector2 iHat = { 1.0f,0.0f };
Vector2 jHat = { 0.0f,1.0f };


Vector2 Vector2::operator+(const Vector2& num) const {
	Vector2 tmp;

	tmp.x = this->x + num.x;
	tmp.y = this->y + num.y;

	return tmp;
};
Vector2 Vector2::operator-(const Vector2& num) const {
	Vector2 tmp;

	tmp.x = this->x - num.x;
	tmp.y = this->y - num.y;

	return tmp;
}


Vector2 Vector2::operator*(float num) const {
	Vector2 tmp;

	tmp.x = this->x * num;
	tmp.y = this->y * num;

	return tmp;
}
Vector2 Vector2::operator/(float num) const {
	Vector2 tmp;

	tmp.x = this->x / num;
	tmp.y = this->y / num;

	return tmp;
}

Vector2 operator+(float num, const Vector2& vec) {
	Vector2 tmp;

	tmp.x = num + vec.x;
	tmp.y = num + vec.y;

	return tmp;
}
Vector2 operator-(float num, const Vector2& vec) {
	Vector2 tmp;

	tmp.x = num - vec.x;
	tmp.y = num - vec.y;

	return tmp;
}
Vector2 operator*(float num, const Vector2& vec) {
	Vector2 tmp;

	tmp.x = num * vec.x;
	tmp.y = num * vec.y;

	return tmp;
}
Vector2 operator/(float num, const Vector2& vec) {
	Vector2 tmp;

	tmp.x = num / vec.x;
	tmp.y = num / vec.y;

	return tmp;
}

const Vector2& Vector2::operator=(const Vector2& num) {
	this->x = num.x;
	this->y = num.y;

	return *this;
}


const Vector2& Vector2::operator+=(const Vector2& num) {
	this->x += num.x;
	this->y += num.y;

	return *this;
}
const Vector2& Vector2::operator-=(const Vector2& num) {
	this->x -= num.x;
	this->y -= num.y;

	return *this;
}

const Vector2& Vector2::operator*=(float num) {
	this->x *= num;
	this->y *= num;

	return *this;
}
const Vector2& Vector2::operator/=(float num) {
	this->x /= num;
	this->y /= num;

	return *this;
}

bool Vector2::operator==(const Vector2& num) const {
	return this->x == num.x && this->y == num.y;
}
bool Vector2::operator!=(const Vector2& num) const {
	return this->x != num.x && this->y != num.y;
}

void Vector2::Rotate(float rad) {
	x *= cosf(rad);
	y *= sinf(rad);
}

float Vector2::Cross(const Vector2& vec) const {
	return x * vec.y - y * vec.x;
}
float Vector2::Dot(const Vector2& vec) const {
	return x * vec.x + y * vec.y;
}

float Vector2::Length() const {
	return std::hypot(x, y);
}

Vector2 Vector2::Normalize() const {
	return *this / Length();
}