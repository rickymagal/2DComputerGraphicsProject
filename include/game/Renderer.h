#ifndef GAME_RENDERER_H
#define GAME_RENDERER_H

#include "../world/Arena.h"
#include "../world/Obstacle.h"
#include "../entity/Player.h"
#include "../entity/Bullet.h"

class Renderer {
public:
    static void drawArena(const Arena& arena);
    static void drawObstacle(const Obstacle& obstacle);
    static void drawPlayer(const Player& player);
    static void drawBullet(const Bullet& bullet);

    static void drawHud(const Arena& arena, int livesP1, int livesP2);
    static void drawGameOver(const Arena& arena, int winnerId);
};

#endif

