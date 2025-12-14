#include "../../include/world/Obstacle.h"

Obstacle::Obstacle()
    : pos(0.0f, 0.0f), radius(1.0f) {}

Obstacle::Obstacle(const Vec2& p, float r)
    : pos(p), radius(r) {}
