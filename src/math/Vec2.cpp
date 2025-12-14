#include "../../include/math/Vec2.h"

Vec2::Vec2() : x(0.0f), y(0.0f) {}
Vec2::Vec2(float px, float py) : x(px), y(py) {}

Vec2 Vec2::operator+(const Vec2& o) const { return Vec2(x + o.x, y + o.y); }
Vec2 Vec2::operator-(const Vec2& o) const { return Vec2(x - o.x, y - o.y); }
Vec2 Vec2::operator*(float s) const { return Vec2(x * s, y * s); }
Vec2 Vec2::operator/(float s) const { return Vec2(x / s, y / s); }

Vec2& Vec2::operator+=(const Vec2& o) { x += o.x; y += o.y; return *this; }
Vec2& Vec2::operator-=(const Vec2& o) { x -= o.x; y -= o.y; return *this; }
Vec2& Vec2::operator*=(float s) { x *= s; y *= s; return *this; }
Vec2& Vec2::operator/=(float s) { x /= s; y /= s; return *this; }

float Vec2::length() const { return std::sqrt(x * x + y * y); }
float Vec2::lengthSq() const { return x * x + y * y; }

Vec2 Vec2::normalized() const {
    float len = length();
    if (len == 0.0f) return Vec2(0.0f, 0.0f);
    return Vec2(x / len, y / len);
}

float Vec2::dot(const Vec2& a, const Vec2& b) {
    return a.x * b.x + a.y * b.y;
}

float Vec2::cross(const Vec2& a, const Vec2& b) {
    return a.x * b.y - a.y * b.x;
}

Vec2 operator*(float s, const Vec2& v) {
    return Vec2(v.x * s, v.y * s);
}
