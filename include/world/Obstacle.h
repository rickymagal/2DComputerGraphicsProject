#ifndef WORLD_OBSTACLE_H
#define WORLD_OBSTACLE_H

#include "../math/Vec2.h"

struct Obstacle {
    Vec2 pos;
    float radius;

    Obstacle();
    Obstacle(const Vec2& p, float r);
};

#endif

