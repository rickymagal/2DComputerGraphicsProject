#include "../../include/math/Collision.h"
#include <cmath>

namespace Collision {

float distance(const Vec2& a, const Vec2& b) {
    return (a - b).length();
}

float distanceSq(const Vec2& a, const Vec2& b) {
    return (a - b).lengthSq();
}

bool circleCircle(const Vec2& aPos, float aR, const Vec2& bPos, float bR) {
    float r = aR + bR;
    return distanceSq(aPos, bPos) <= r * r;
}

bool circleInsideArena(const Vec2& p, float r, const Arena& arena) {
    Vec2 d = p - arena.center;
    float maxDist = arena.radius - r;
    return d.lengthSq() <= maxDist * maxDist;
}

bool circleHitsObstacle(const Vec2& p, float r, const Obstacle& ob) {
    return circleCircle(p, r, ob.pos, ob.radius);
}

}
