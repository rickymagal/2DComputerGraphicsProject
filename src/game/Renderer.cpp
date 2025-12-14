#include "../../include/game/Renderer.h"
#include "../../include/math/Angle.h"
#include <GL/glut.h>
#include <cmath>
#include <cstdio>

static void drawCircleOutline(const Vec2& c, float r, int segments = 96) {
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < segments; ++i) {
        float a = 2.0f * 3.1415926535f * float(i) / float(segments);
        glVertex2f(c.x + std::cos(a) * r, c.y - std::sin(a) * r);
    }
    glEnd();
}

static void drawCircleFilled(const Vec2& c, float r, int segments = 96) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(c.x, c.y);
    for (int i = 0; i <= segments; ++i) {
        float a = 2.0f * 3.1415926535f * float(i) / float(segments);
        glVertex2f(c.x + std::cos(a) * r, c.y - std::sin(a) * r);
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
    glLineWidth(4.0f);
    glColor3f(0.1f, 0.35f, 1.0f);
    drawCircleOutline(arena.center, arena.radius);
    glLineWidth(1.0f);
}

void Renderer::drawObstacle(const Obstacle& obstacle) {
    glColor3f(0.02f, 0.02f, 0.02f);
    drawCircleFilled(obstacle.pos, obstacle.radius);

    glLineWidth(3.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawCircleOutline(obstacle.pos, obstacle.radius);
    glLineWidth(1.0f);
}

static int stepParity(const Player& p) {
    if (!p.walking) return 0;
    float phase = p.walkPhase;
    int k = int(std::floor(phase / Angle::pi()));
    return (k & 1);
}

void Renderer::drawPlayer(const Player& player) {
    if ((int)player.id == 1) glColor3f(0.1f, 0.9f, 0.2f);
    else glColor3f(0.9f, 0.2f, 0.2f);

    // Head
    drawCircleFilled(player.pos, player.headRadius);

    Vec2 f = player.forward();
    Vec2 left(-f.y, f.x);

    // Torso reference points
    Vec2 neck = player.pos + f * (player.headRadius * 0.95f);
    Vec2 hip  = player.pos - f * (player.headRadius * 1.25f);

    // Torso
    glLineWidth(4.0f);
    glBegin(GL_LINES);
    glVertex2f(neck.x, neck.y);
    glVertex2f(hip.x, hip.y);
    glEnd();

    // Arm (one arm), from shoulder on the side
    Vec2 shoulder = player.pos + left * (player.headRadius * 0.55f);
    Vec2 armDir = player.armWorldDir();
    Vec2 hand = shoulder + armDir * (player.headRadius * 1.75f);

    glBegin(GL_LINES);
    glVertex2f(shoulder.x, shoulder.y);
    glVertex2f(hand.x, hand.y);
    glEnd();

    // Legs: two discrete poses (spec figure)
    // Pose A: left foot forward, right foot back
    // Pose B: right foot forward, left foot back
    int parity = stepParity(player);

    float stride = player.headRadius * 1.05f;
    float spread = player.headRadius * 0.35f;

    // 45-degree-ish leg direction components
    Vec2 forwardStep = f * (stride * 0.9f);
    Vec2 backStep    = f * (-stride * 0.9f);

    Vec2 hipL = hip + left * spread;
    Vec2 hipR = hip - left * spread;

    Vec2 footL;
    Vec2 footR;

    if (parity == 0) {
        // left forward, right back
        footL = hipL + forwardStep - left * (spread * 0.2f);
        footR = hipR + backStep + left * (spread * 0.2f);
    } else {
        // right forward, left back
        footL = hipL + backStep - left * (spread * 0.2f);
        footR = hipR + forwardStep + left * (spread * 0.2f);
    }

    glBegin(GL_LINES);
    glVertex2f(hipL.x, hipL.y);
    glVertex2f(footL.x, footL.y);
    glVertex2f(hipR.x, hipR.y);
    glVertex2f(footR.x, footR.y);
    glEnd();

    glLineWidth(1.0f);

    // Facing marker (white)
    glColor3f(1.0f, 1.0f, 1.0f);
    Vec2 tip = player.pos + f * (player.headRadius * 1.7f);
    glBegin(GL_LINES);
    glVertex2f(player.pos.x, player.pos.y);
    glVertex2f(tip.x, tip.y);
    glEnd();
}

void Renderer::drawBullet(const Bullet& bullet) {
    glColor3f(1.0f, 0.9f, 0.2f);
    drawCircleFilled(bullet.pos, bullet.radius, 24);
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
    drawText(right - margin - arena.radius * 0.28f, y, buf2, GLUT_BITMAP_8_BY_13);
}

void Renderer::drawGameOver(const Arena& arena, int winnerId) {
    float cx = arena.center.x;
    float cy = arena.center.y;

    const char* msg = (winnerId == 1) ? "PLAYER 1 WINS" : "PLAYER 2 WINS";

    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(cx - arena.radius * 0.25f, cy, msg, GLUT_BITMAP_HELVETICA_18);
}
