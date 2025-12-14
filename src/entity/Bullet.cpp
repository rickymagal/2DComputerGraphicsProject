#include "../../include/entity/Bullet.h"

Bullet::Bullet()
    : radius(0.1f), ownerId(0), alive(false) {}

void Bullet::spawn(const Vec2& p, const Vec2& v, float r, int owner) {
    pos = p;
    vel = v;
    radius = r;
    ownerId = owner;
    alive = true;
}

void Bullet::update(float dt) {
    pos += vel * dt;
}

bool Bullet::isOutsideArena(const Arena& arena) const {
    Vec2 d = pos - arena.center;
    return d.lengthSq() > arena.radius * arena.radius;
}

bool Bullet::hitsObstacle(const Obstacle& ob) const {
    float r = radius + ob.radius;
    return (pos - ob.pos).lengthSq() <= r * r;
}
