#ifndef ENTITY_PLAYER_H
#define ENTITY_PLAYER_H

#include <cstdint>

#include "../math/Vec2.h"
#include "../world/Arena.h"

enum class PlayerId : int {
    P1 = 1,
    P2 = 2
};

struct Player {
    PlayerId id;

    Vec2 pos;
    float headingRad;

    float headRadius;

    float armRelRad;
    float armMinRelRad;
    float armMaxRelRad;

    float moveSpeed;
    float turnSpeedRad;

    int lives;

    float walkPhase;
    bool walking;

    Player();

    float bodyRadius() const;

    Vec2 forward() const;

    void setDefaults(PlayerId pid);

    void resetAt(const Vec2& p, float headR);

    void applyMovement(float dt, bool moveForward, bool moveBackward, bool turnLeft, bool turnRight);

    void setArmRelative(float relRad);
    void addArmRelative(float deltaRelRad);

    Vec2 armWorldDir() const;

    void clampArm();
};

#endif

