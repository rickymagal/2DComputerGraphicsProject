#include "../../include/entity/Player.h"
#include "../../include/math/Angle.h"
#include <cmath>

Player::Player() {
    setDefaults(PlayerId::P1);
}

void Player::setDefaults(PlayerId pid) {
    id = pid;
    headingRad = 0.0f;
    armRelRad = 0.0f;
    armMinRelRad = Angle::degToRad(-60.0f);
    armMaxRelRad = Angle::degToRad(60.0f);
    moveSpeed = 2.5f;
    turnSpeedRad = Angle::degToRad(180.0f);
    lives = 3;
    walkPhase = 0.0f;
    walking = false;
}

void Player::resetAt(const Vec2& p, float headR) {
    pos = p;
    headRadius = headR;
}

float Player::bodyRadius() const {
    return headRadius;
}

Vec2 Player::forward() const {
    return Vec2(std::cos(headingRad), std::sin(headingRad));
}

void Player::applyMovement(float dt, bool moveForward, bool moveBackward, bool turnLeft, bool turnRight) {
    float turn = 0.0f;
    if (turnLeft)  turn += 1.0f;
    if (turnRight) turn -= 1.0f;

    if (moveForward || moveBackward) {
        headingRad += turn * turnSpeedRad * dt;
        headingRad = Angle::wrap2Pi(headingRad);
    }

    float move = 0.0f;
    if (moveForward) move += 1.0f;
    if (moveBackward) move -= 1.0f;

    if (move != 0.0f) {
        Vec2 dir = forward();
        pos += dir * (move * moveSpeed * dt);
        walkPhase += dt * 6.0f;
        walking = true;
    } else {
        walking = false;
    }
}

void Player::setArmRelative(float relRad) {
    armRelRad = relRad;
    clampArm();
}

void Player::addArmRelative(float deltaRelRad) {
    armRelRad += deltaRelRad;
    clampArm();
}

Vec2 Player::armWorldDir() const {
    float a = headingRad + armRelRad;
    return Vec2(std::cos(a), std::sin(a));
}

void Player::clampArm() {
    armRelRad = Angle::clamp(armRelRad, armMinRelRad, armMaxRelRad);
}
