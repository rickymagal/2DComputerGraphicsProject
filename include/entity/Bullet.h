#ifndef ENTITY_BULLET_H
#define ENTITY_BULLET_H

#include "../math/Vec2.h"
#include "../world/Arena.h"
#include "../world/Obstacle.h"

struct Bullet {
    Vec2 pos;
    Vec2 vel;
    float radius;

    int ownerId;
    bool alive;

    Bullet();

    void spawn(const Vec2& p, const Vec2& v, float r, int owner);

    void update(float dt);

    bool isOutsideArena(const Arena& arena) const;

    bool hitsObstacle(const Obstacle& ob) const;
};

#endif

