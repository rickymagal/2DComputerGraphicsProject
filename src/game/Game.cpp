#include "../../include/game/Game.h"
#include "../../include/game/Renderer.h"
#include "../../include/math/Collision.h"
#include "../../include/io/SvgLoader.h"

#include <GL/glut.h>

Game::Game()
    : state(GameState::RUNNING), winnerId(0) {}

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
}

bool Game::isRunning() const {
    return state == GameState::RUNNING;
}

const Arena& Game::getArena() const {
    return arena;
}

void Game::update(float dt) {
    if (state != GameState::RUNNING) return;

    updatePlayers(dt);
    updateBullets(dt);
    handleCollisions();
    checkGameOver();
}

void Game::updatePlayers(float dt) {
    bool p1Forward  = input.keys['w'] || input.specialKeys[GLUT_KEY_UP];
    bool p1Backward = input.keys['s'] || input.specialKeys[GLUT_KEY_DOWN];
    bool p1TurnLeft = input.keys['a'] || input.specialKeys[GLUT_KEY_LEFT];
    bool p1TurnRight = input.keys['d'] || input.specialKeys[GLUT_KEY_RIGHT];

    player1.applyMovement(dt, p1Forward, p1Backward, p1TurnLeft, p1TurnRight);

    player2.applyMovement(
        dt,
        input.keys['o'],
        input.keys['l'],
        input.keys['k'],
        input.keys[';'] || input.keys[231]
    );
}

void Game::updateBullets(float dt) {
    for (auto& b : bullets) {
        if (!b.alive) continue;
        b.update(dt);
        if (b.isOutsideArena(arena)) {
            b.alive = false;
        }
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
