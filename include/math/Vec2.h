#ifndef MATH_VEC2_H
#define MATH_VEC2_H

#include <cmath>

struct Vec2 {
    float x;
    float y;

    Vec2();
    Vec2(float px, float py);

    Vec2 operator+(const Vec2& o) const;
    Vec2 operator-(const Vec2& o) const;
    Vec2 operator*(float s) const;
    Vec2 operator/(float s) const;

    Vec2& operator+=(const Vec2& o);
    Vec2& operator-=(const Vec2& o);
    Vec2& operator*=(float s);
    Vec2& operator/=(float s);

    float length() const;
    float lengthSq() const;

    Vec2 normalized() const;

    static float dot(const Vec2& a, const Vec2& b);
    static float cross(const Vec2& a, const Vec2& b);
};

Vec2 operator*(float s, const Vec2& v);

#endif

