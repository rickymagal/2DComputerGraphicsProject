#ifndef MATH_COLLISION_H
#define MATH_COLLISION_H

#include "../math/Vec2.h"
#include "../world/Arena.h"
#include "../world/Obstacle.h"

namespace Collision {

float distance(const Vec2& a, const Vec2& b);
float distanceSq(const Vec2& a, const Vec2& b);

bool circleCircle(const Vec2& aPos, float aR, const Vec2& bPos, float bR);

bool circleInsideArena(const Vec2& p, float r, const Arena& arena);

bool circleHitsObstacle(const Vec2& p, float r, const Obstacle& ob);

} // namespace Collision

#endif

