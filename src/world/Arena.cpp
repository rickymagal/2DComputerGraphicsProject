#include "../../include/world/Arena.h"
#include <cmath>

Arena::Arena()
    : center(0.0f, 0.0f), radius(1.0f) {}

bool Arena::containsCircle(const Vec2& p, float r) const {
    Vec2 d = p - center;
    float distSq = d.lengthSq();
    float maxDist = radius - r;
    return distSq <= maxDist * maxDist;
}
