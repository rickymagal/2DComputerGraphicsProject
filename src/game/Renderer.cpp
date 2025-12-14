#include "../../include/game/Renderer.h"
#include "../../include/math/Angle.h"
#include <GL/glut.h>
#include <cmath>
#include <cstdio>

static void drawText(float x, float y, const char* s, void* font) {
    glRasterPos2f(x, y);
    for (const char* c = s; *c; ++c) glutBitmapCharacter(font, *c);
}

static void drawCircleFilledYDown(const Vec2& c, float r, int segments = 96) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(c.x, c.y);
    for (int i = 0; i <= segments; ++i) {
        float a = 2.0f * 3.1415926535f * float(i) / float(segments);
        glVertex2f(c.x + std::cos(a) * r, c.y - std::sin(a) * r);
    }
    glEnd();
}

static void drawCircleOutlineYDown(const Vec2& c, float r, int segments = 96) {
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < segments; ++i) {
        float a = 2.0f * 3.1415926535f * float(i) / float(segments);
        glVertex2f(c.x + std::cos(a) * r, c.y - std::sin(a) * r);
    }
    glEnd();
}

static void drawEllipseFilledYDown(const Vec2& c, float rx, float ry, float rotRad, int segments = 96) {
    float cr = std::cos(rotRad);
    float sr = std::sin(rotRad);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(c.x, c.y);
    for (int i = 0; i <= segments; ++i) {
        float a = 2.0f * 3.1415926535f * float(i) / float(segments);
        float ex = std::cos(a) * rx;
        float ey = -std::sin(a) * ry;

        float x = ex * cr - ey * sr;
        float y = ex * sr + ey * cr;
        glVertex2f(c.x + x, c.y + y);
    }
    glEnd();
}

static void drawEllipseOutlineYDown(const Vec2& c, float rx, float ry, float rotRad, int segments = 96) {
    float cr = std::cos(rotRad);
    float sr = std::sin(rotRad);

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < segments; ++i) {
        float a = 2.0f * 3.1415926535f * float(i) / float(segments);
        float ex = std::cos(a) * rx;
        float ey = -std::sin(a) * ry;

        float x = ex * cr - ey * sr;
        float y = ex * sr + ey * cr;
        glVertex2f(c.x + x, c.y + y);
    }
    glEnd();
}

static void rectPoints(const Vec2& center, const Vec2& dirUnit, float halfLen, float halfW,
                       Vec2& p0, Vec2& p1, Vec2& p2, Vec2& p3) {
    Vec2 d = dirUnit;
    Vec2 n(-d.y, d.x);

    p0 = center - d * halfLen - n * halfW;
    p1 = center + d * halfLen - n * halfW;
    p2 = center + d * halfLen + n * halfW;
    p3 = center - d * halfLen + n * halfW;
}

static void drawRectFilled(const Vec2& center, const Vec2& dirUnit, float halfLen, float halfW) {
    Vec2 p0, p1, p2, p3;
    rectPoints(center, dirUnit, halfLen, halfW, p0, p1, p2, p3);

    glBegin(GL_QUADS);
    glVertex2f(p0.x, p0.y);
    glVertex2f(p1.x, p1.y);
    glVertex2f(p2.x, p2.y);
    glVertex2f(p3.x, p3.y);
    glEnd();
}

static void drawRectOutline(const Vec2& center, const Vec2& dirUnit, float halfLen, float halfW) {
    Vec2 p0, p1, p2, p3;
    rectPoints(center, dirUnit, halfLen, halfW, p0, p1, p2, p3);

    glBegin(GL_LINE_LOOP);
    glVertex2f(p0.x, p0.y);
    glVertex2f(p1.x, p1.y);
    glVertex2f(p2.x, p2.y);
    glVertex2f(p3.x, p3.y);
    glEnd();
}

static int footSwapNow(const Player& p) {
    if (!p.walking) return 0;
    int ms = glutGet(GLUT_ELAPSED_TIME);
    float t = float(ms) * 0.001f;
    float swapsPerSecond = 2.0f; // 2 swaps/s (tune if you want faster)
    int k = (int)std::floor(t * swapsPerSecond);
    return (k & 1);
}

void Renderer::drawArena(const Arena& arena) {
    glLineWidth(4.0f);
    glColor3f(0.1f, 0.35f, 1.0f);
    drawCircleOutlineYDown(arena.center, arena.radius);
    glLineWidth(1.0f);
}

void Renderer::drawObstacle(const Obstacle& obstacle) {
    glColor3f(0.02f, 0.02f, 0.02f);
    drawCircleFilledYDown(obstacle.pos, obstacle.radius);

    glLineWidth(3.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawCircleOutlineYDown(obstacle.pos, obstacle.radius);
    glLineWidth(1.0f);
}

void Renderer::drawPlayer(const Player& player) {
    float R = player.headRadius;

    Vec2 f = player.forward();      // heading=0 -> north (0,-1)
    Vec2 left(f.y, -f.x);
    Vec2 right(-f.y, f.x);

    float rFill, gFill, bFill;
    if ((int)player.id == 1) { rFill = 0.0f; gFill = 0.75f; bFill = 0.25f; }
    else { rFill = 0.85f; gFill = 0.20f; bFill = 0.20f; }

    // Arms: fixed ellipses left/right, colored like body
    float armRx = R * 0.95f;
    float armRy = R * 0.35f;
    Vec2 armL = player.pos + left  * (R * 1.05f);
    Vec2 armR = player.pos + right * (R * 1.05f);
    float armsRot = std::atan2(left.y, left.x);

    glColor3f(rFill, gFill, bFill);
    drawEllipseFilledYDown(armL, armRx, armRy, armsRot);
    drawEllipseFilledYDown(armR, armRx, armRy, armsRot);

    glLineWidth(3.0f);
    glColor3f(0.0f, 0.0f, 0.0f);
    drawEllipseOutlineYDown(armL, armRx, armRy, armsRot);
    drawEllipseOutlineYDown(armR, armRx, armRy, armsRot);
    glLineWidth(1.0f);

    // Feet: short black rectangles at extreme front/back, tangent to circle, and SWAP
    float footHalfLen = R * 0.55f;
    float footHalfW   = R * 0.18f;

    Vec2 baseFront = player.pos + f * (R + footHalfLen); // inner end touches circle at +f*R
    Vec2 baseBack  = player.pos - f * (R + footHalfLen); // inner end touches circle at -f*R

    int sw = footSwapNow(player);
    Vec2 footFront = (sw == 0) ? baseFront : baseBack;
    Vec2 footBack  = (sw == 0) ? baseBack  : baseFront;

    // Draw back foot first, then body, then front foot last (so the "front" foot is on top)
    glLineWidth(3.0f);
    glColor3f(0.0f, 0.0f, 0.0f);
    drawRectFilled(footBack, f, footHalfLen, footHalfW);

    // Body
    glColor3f(rFill, gFill, bFill);
    drawCircleFilledYDown(player.pos, R);

    glLineWidth(3.0f);
    glColor3f(0.0f, 0.0f, 0.0f);
    drawCircleOutlineYDown(player.pos, R);
    glLineWidth(1.0f);

    glLineWidth(3.0f);
    glColor3f(0.0f, 0.0f, 0.0f);
    drawRectFilled(footFront, f, footHalfLen, footHalfW);
    glLineWidth(1.0f);

    // Weapon: anchored at center of right arm ellipse; color matches body (P2 weapon red)
    Vec2 weaponDir = player.armWorldDir();
    Vec2 weaponBase = armR;

    float weaponLen = R * 1.45f;
    float weaponHalfLen = weaponLen * 0.5f;
    float weaponHalfW = R * 0.14f;
    Vec2 weaponCenter = weaponBase + weaponDir * weaponHalfLen;

    glColor3f(rFill, gFill, bFill);
    drawRectFilled(weaponCenter, weaponDir, weaponHalfLen, weaponHalfW);

    glLineWidth(3.0f);
    glColor3f(0.0f, 0.0f, 0.0f);
    drawRectOutline(weaponCenter, weaponDir, weaponHalfLen, weaponHalfW);
    glLineWidth(1.0f);
}

void Renderer::drawBullet(const Bullet& bullet) {
    glColor3f(1.0f, 0.9f, 0.2f);
    drawCircleFilledYDown(bullet.pos, bullet.radius, 24);
    glColor3f(0.0f, 0.0f, 0.0f);
    drawCircleOutlineYDown(bullet.pos, bullet.radius, 24);
}

void Renderer::drawHud(const Arena& arena, int livesP1, int livesP2) {
    float leftX = arena.center.x - arena.radius;
    float rightX = arena.center.x + arena.radius;
    float topY = arena.center.y - arena.radius;

    float margin = arena.radius * 0.06f;
    float y = topY + margin;

    char buf1[32];
    char buf2[32];

    std::snprintf(buf1, sizeof(buf1), "P1: %d", livesP1);
    std::snprintf(buf2, sizeof(buf2), "P2: %d", livesP2);

    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(leftX + margin, y, buf1, GLUT_BITMAP_8_BY_13);
    drawText(rightX - margin - arena.radius * 0.28f, y, buf2, GLUT_BITMAP_8_BY_13);
}

void Renderer::drawGameOver(const Arena& arena, int winnerId) {
    float cx = arena.center.x;
    float cy = arena.center.y;

    const char* msg = (winnerId == 1) ? "PLAYER 1 WINS" : "PLAYER 2 WINS";

    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(cx - arena.radius * 0.25f, cy, msg, GLUT_BITMAP_HELVETICA_18);
}
