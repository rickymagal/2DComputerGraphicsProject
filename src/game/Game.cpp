#include "../../include/game/Game.h"
#include "../../include/game/Renderer.h"
#include "../../include/math/Collision.h"
#include "../../include/math/Angle.h"
#include "../../include/io/SvgLoader.h"

#include <GL/glut.h>
#include <algorithm>
#include <cmath>

Game::Game()
    : state(GameState::RUNNING),
      winnerId(0),
      prevMouseLeft(false),
      prevKey5(false),
      shootCooldownP1(0.0f),
      shootCooldownP2(0.0f) {}

bool Game::loadFromSvg(const std::string& path) {
    SvgSceneData data;
    if (!SvgLoader::load(path, data)) return false;

    arena = data.arena;
    obstacles = data.obstacles;

    player1.setDefaults(PlayerId::P1);
    player2.setDefaults(PlayerId::P2);

    player1.resetAt(data.player1Pos, data.player1HeadRadius);
    player2.resetAt(data.player2Pos, data.player2HeadRadius);

    reset();
    return true;
}

void Game::reset() {
    state = GameState::RUNNING;
    winnerId = 0;

    bullets.clear();

    player1.lives = 3;
    player2.lives = 3;

    prevMouseLeft = false;
    prevKey5 = false;

    shootCooldownP1 = 0.0f;
    shootCooldownP2 = 0.0f;

    input.clear();
}

bool Game::isRunning() const {
    return state == GameState::RUNNING;
}

const Arena& Game::getArena() const {
    return arena;
}

void Game::update(float dt) {
    if (state != GameState::RUNNING) return;

    shootCooldownP1 = std::max(0.0f, shootCooldownP1 - dt);
    shootCooldownP2 = std::max(0.0f, shootCooldownP2 - dt);

    updatePlayers(dt);
    updateBullets(dt);
    handleCollisions();
    checkGameOver();
}

static float mapMouseXToArmRel(int mouseX, int winW, float minRel, float maxRel) {
    if (winW <= 1) return 0.0f;
    float t = float(mouseX) / float(winW - 1);
    return minRel + (maxRel - minRel) * t;
}

static void keepInsideArena(Player& p, const Arena& a) {
    float pr = p.headRadius;
    Vec2 d = p.pos - a.center;
    float dist = std::sqrt(d.lengthSq());
    float maxDist = a.radius - pr;

    if (dist > maxDist) {
        Vec2 n = (dist > 1e-6f) ? (d / dist) : Vec2(1.0f, 0.0f);
        p.pos = a.center + n * maxDist;
    }
}

static void pushOutOfObstacle(Player& p, const Obstacle& ob) {
    float pr = p.headRadius;
    Vec2 d = p.pos - ob.pos;
    float dist = std::sqrt(d.lengthSq());
    float minDist = pr + ob.radius;

    if (dist < minDist) {
        Vec2 n = (dist > 1e-6f) ? (d / dist) : Vec2(1.0f, 0.0f);
        p.pos += n * (minDist - dist);
    }
}

static void separatePlayers(Player& a, Player& b) {
    if (a.lives <= 0 || b.lives <= 0) return;

    float ra = a.headRadius;
    float rb = b.headRadius;

    Vec2 d = a.pos - b.pos;
    float dist = std::sqrt(d.lengthSq());
    float minDist = ra + rb;

    if (dist < minDist) {
        Vec2 n = (dist > 1e-6f) ? (d / dist) : Vec2(1.0f, 0.0f);
        float push = (minDist - dist) * 0.5f;
        a.pos += n * push;
        b.pos -= n * push;
    }
}

void Game::spawnBulletFromPlayer(const Player& p) {
    if (p.lives <= 0) return;

    Vec2 f = p.forward();
    Vec2 left(-f.y, f.x);

    Vec2 shoulder = p.pos + left * (p.headRadius * 0.55f);
    Vec2 armDir = p.armWorldDir();

    Vec2 spawnPos = shoulder + armDir * (p.headRadius * 1.75f);

    float bulletSpeed = 2.0f * p.moveSpeed;
    Vec2 vel = armDir * bulletSpeed;

    Bullet b;
    b.spawn(spawnPos, vel, p.headRadius * 0.15f, (int)p.id);
    bullets.push_back(b);
}

static void resolveWorldForPlayer(Player& p, const Arena& a, const std::vector<Obstacle>& obs) {
    keepInsideArena(p, a);

    for (int it = 0; it < 3; ++it) {
        for (const auto& ob : obs)
            pushOutOfObstacle(p, ob);

        keepInsideArena(p, a);
    }
}

void Game::updatePlayers(float dt) {
    bool p1Forward   = input.keys['w'] || input.specialKeys[GLUT_KEY_UP];
    bool p1Backward  = input.keys['s'] || input.specialKeys[GLUT_KEY_DOWN];
    bool p1TurnLeft  = input.keys['a'] || input.specialKeys[GLUT_KEY_LEFT];
    bool p1TurnRight = input.keys['d'] || input.specialKeys[GLUT_KEY_RIGHT];

    player1.applyMovement(dt, p1Forward, p1Backward, p1TurnLeft, p1TurnRight);

    bool p2Forward   = input.keys['o'];
    bool p2Backward  = input.keys['l'];
    bool p2TurnLeft  = input.keys['k'];
    bool p2TurnRight = input.keys[';'] || input.keys['p'] || input.keys[231];

    player2.applyMovement(dt, p2Forward, p2Backward, p2TurnLeft, p2TurnRight);

    int winW = glutGet(GLUT_WINDOW_WIDTH);
    player1.setArmRelative(
        mapMouseXToArmRel(input.mouseX, winW,
                          player1.armMinRelRad,
                          player1.armMaxRelRad)
    );

    float armSpeed = Angle::degToRad(120.0f);
    if (input.keys['4']) player2.addArmRelative(+armSpeed * dt);
    if (input.keys['6']) player2.addArmRelative(-armSpeed * dt);

    resolveWorldForPlayer(player1, arena, obstacles);
    resolveWorldForPlayer(player2, arena, obstacles);

    for (int i = 0; i < 3; ++i) {
        separatePlayers(player1, player2);
        resolveWorldForPlayer(player1, arena, obstacles);
        resolveWorldForPlayer(player2, arena, obstacles);
    }

    bool mouseLeft = input.mouseLeftPressed;
    bool key5 = input.keys['5'];

    if (mouseLeft && !prevMouseLeft && shootCooldownP1 <= 0.0f) {
        spawnBulletFromPlayer(player1);
        shootCooldownP1 = 0.15f;
    }

    if (key5 && !prevKey5 && shootCooldownP2 <= 0.0f) {
        spawnBulletFromPlayer(player2);
        shootCooldownP2 = 0.15f;
    }

    prevMouseLeft = mouseLeft;
    prevKey5 = key5;
}

void Game::updateBullets(float dt) {
    for (auto& b : bullets) {
        if (!b.alive) continue;
        b.update(dt);
        if (b.isOutsideArena(arena))
            b.alive = false;
    }
}

void Game::handleCollisions() {
    for (auto& b : bullets) {
        if (!b.alive) continue;

        for (const auto& ob : obstacles) {
            if (b.hitsObstacle(ob)) {
                b.alive = false;
                break;
            }
        }

        if (!b.alive) continue;

        Player* targets[2] = { &player1, &player2 };
        for (Player* p : targets) {
            if (p->lives <= 0) continue;
            if ((int)p->id == b.ownerId) continue;

            if (Collision::circleCircle(b.pos, b.radius, p->pos, p->headRadius)) {
                p->lives--;
                b.alive = false;
            }
        }
    }
}

void Game::checkGameOver() {
    if (player1.lives <= 0) {
        state = GameState::GAME_OVER;
        winnerId = 2;
    }
    if (player2.lives <= 0) {
        state = GameState::GAME_OVER;
        winnerId = 1;
    }
}

void Game::render() const {
    Renderer::drawArena(arena);

    for (const auto& ob : obstacles)
        Renderer::drawObstacle(ob);

    if (player1.lives > 0) Renderer::drawPlayer(player1);
    if (player2.lives > 0) Renderer::drawPlayer(player2);

    for (const auto& b : bullets)
        if (b.alive) Renderer::drawBullet(b);

    Renderer::drawHud(arena, player1.lives, player2.lives);

    if (state == GameState::GAME_OVER)
        Renderer::drawGameOver(arena, winnerId);
}

void Game::onKeyDown(unsigned char key) {
    input.keys[key] = true;
    if (key == 'r') reset();
}

void Game::onKeyUp(unsigned char key) {
    input.keys[key] = false;
}

void Game::onSpecialKeyDown(int key) {
    if (key >= 0 && key < 256) input.specialKeys[key] = true;
}

void Game::onSpecialKeyUp(int key) {
    if (key >= 0 && key < 256) input.specialKeys[key] = false;
}

void Game::onMouseMove(int x, int y) {
    input.mouseX = x;
    input.mouseY = y;
}

void Game::onMouseClick(bool pressed) {
    input.mouseLeftPressed = pressed;
}
