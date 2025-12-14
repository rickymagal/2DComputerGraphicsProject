#ifndef GAME_GAME_H
#define GAME_GAME_H

#include <vector>
#include <string>

#include "../entity/Player.h"
#include "../entity/Bullet.h"
#include "../world/Arena.h"
#include "../world/Obstacle.h"
#include "InputState.h"

enum class GameState {
    RUNNING,
    GAME_OVER
};

class Game {
public:
    Game();

    bool loadFromSvg(const std::string& path);

    void update(float deltaTime);
    void render() const;

    void onKeyDown(unsigned char key);
    void onKeyUp(unsigned char key);
    void onSpecialKeyDown(int key);
    void onSpecialKeyUp(int key);
    void onMouseMove(int x, int y);
    void onMouseClick(bool pressed);

    void reset();

    bool isRunning() const;

    const Arena& getArena() const;

private:
    void updatePlayers(float dt);
    void updateBullets(float dt);
    void handleCollisions();
    void checkGameOver();

    void spawnBulletFromPlayer(const Player& p);

private:
    GameState state;

    Arena arena;
    std::vector<Obstacle> obstacles;

    Player player1;
    Player player2;

    std::vector<Bullet> bullets;

    InputState input;

    int winnerId;

    bool prevMouseLeft;
    bool prevKey5;

    float shootCooldownP1;
    float shootCooldownP2;
};

#endif

