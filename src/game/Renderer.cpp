#include "../../include/game/Renderer.h"
#include <GL/glut.h>
#include <cmath>
#include <cstdio>

static void drawCircle(const Vec2& c, float r, int segments = 64) {
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < segments; ++i) {
        float a = 2.0f * 3.1415926535f * float(i) / float(segments);
        glVertex2f(c.x + std::cos(a) * r, c.y + std::sin(a) * r);
    }
    glEnd();
}

static void drawText(float x, float y, const char* s, void* font) {
    glRasterPos2f(x, y);
    for (const char* c = s; *c; ++c) {
        glutBitmapCharacter(font, *c);
    }
}

void Renderer::drawArena(const Arena& arena) {
    glColor3f(0.0f, 0.0f, 1.0f);
    drawCircle(arena.center, arena.radius);
}

void Renderer::drawObstacle(const Obstacle& obstacle) {
    glColor3f(0.0f, 0.0f, 0.0f);
    drawCircle(obstacle.pos, obstacle.radius);
}

void Renderer::drawPlayer(const Player& player) {
    glColor3f(0.0f, 1.0f, 0.0f);

    drawCircle(player.pos, player.headRadius);

    Vec2 f = player.forward();
    Vec2 armDir = player.armWorldDir();

    glBegin(GL_LINES);
    glVertex2f(player.pos.x, player.pos.y);
    glVertex2f(player.pos.x + f.x * player.headRadius * 1.5f,
               player.pos.y + f.y * player.headRadius * 1.5f);
    glEnd();

    glBegin(GL_LINES);
    glVertex2f(player.pos.x, player.pos.y);
    glVertex2f(player.pos.x + armDir.x * player.headRadius * 1.8f,
               player.pos.y + armDir.y * player.headRadius * 1.8f);
    glEnd();
}

void Renderer::drawBullet(const Bullet& bullet) {
    glColor3f(0.8f, 0.0f, 0.0f);
    drawCircle(bullet.pos, bullet.radius, 16);
}

void Renderer::drawHud(const Arena& arena, int livesP1, int livesP2) {
    float left = arena.center.x - arena.radius;
    float right = arena.center.x + arena.radius;
    float top = arena.center.y - arena.radius;

    float margin = arena.radius * 0.06f;
    float y = top + margin;

    char buf1[32];
    char buf2[32];

    std::snprintf(buf1, sizeof(buf1), "P1: %d", livesP1);
    std::snprintf(buf2, sizeof(buf2), "P2: %d", livesP2);

    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(left + margin, y, buf1, GLUT_BITMAP_8_BY_13);
    drawText(right - margin - arena.radius * 0.25f, y, buf2, GLUT_BITMAP_8_BY_13);
}

void Renderer::drawGameOver(const Arena& arena, int winnerId) {
    float cx = arena.center.x;
    float cy = arena.center.y;

    const char* msg = (winnerId == 1) ? "PLAYER 1 WINS" : "PLAYER 2 WINS";

    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(cx - arena.radius * 0.25f, cy, msg, GLUT_BITMAP_HELVETICA_18);
}
