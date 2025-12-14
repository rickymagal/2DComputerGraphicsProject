#ifndef MATH_ANGLE_H
#define MATH_ANGLE_H

#include <cmath>

namespace Angle {

static inline float pi() { return 3.14159265358979323846f; }

static inline float degToRad(float deg) { return deg * (pi() / 180.0f); }
static inline float radToDeg(float rad) { return rad * (180.0f / pi()); }

static inline float clamp(float v, float lo, float hi) {
    return (v < lo) ? lo : ((v > hi) ? hi : v);
}

static inline float wrapPi(float a) {
    while (a <= -pi()) a += 2.0f * pi();
    while (a >  pi()) a -= 2.0f * pi();
    return a;
}

static inline float wrap2Pi(float a) {
    while (a < 0.0f) a += 2.0f * pi();
    while (a >= 2.0f * pi()) a -= 2.0f * pi();
    return a;
}

} // namespace Angle

#endif

