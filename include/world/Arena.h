#ifndef WORLD_ARENA_H
#define WORLD_ARENA_H

#include "../math/Vec2.h"

struct Arena {
    Vec2 center;
    float radius;

    Arena();

    bool containsCircle(const Vec2& p, float r) const;
};

#endif

