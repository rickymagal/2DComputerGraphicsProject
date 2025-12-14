#include <GL/glut.h>
#include <chrono>
#include <cstdio>

#include "../include/game/Game.h"

static Game game;

static int windowWidth = 500;
static int windowHeight = 500;

static std::chrono::steady_clock::time_point lastTime;

static void applyCamera() {
    const Arena& a = game.getArena();

    float left = a.center.x - a.radius;
    float right = a.center.x + a.radius;
    float top = a.center.y - a.radius;
    float bottom = a.center.y + a.radius;

    glViewport(0, 0, windowWidth, windowHeight);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluOrtho2D(left, right, bottom, top);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

static void displayCallback() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    game.render();

    glutSwapBuffers();
}

static void idleCallback() {
    auto now = std::chrono::steady_clock::now();
    float dt = std::chrono::duration<float>(now - lastTime).count();
    lastTime = now;

    game.update(dt);

    glutPostRedisplay();
}

static void reshapeCallback(int w, int h) {
    windowWidth = w;
    windowHeight = h;
    applyCamera();
}

static void keyDownCallback(unsigned char key, int, int) {
    game.onKeyDown(key);
}

static void keyUpCallback(unsigned char key, int, int) {
    game.onKeyUp(key);
}

static void specialKeyDownCallback(int key, int, int) {
    game.onSpecialKeyDown(key);
}

static void specialKeyUpCallback(int key, int, int) {
    game.onSpecialKeyUp(key);
}

static void mouseButtonCallback(int button, int state, int /*x*/, int /*y*/) {
    if (button == GLUT_LEFT_BUTTON) {
        game.onMouseClick(state == GLUT_DOWN);
    }
}

static void mouseMoveCallback(int x, int y) {
    game.onMouseMove(x, y);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::fprintf(stderr, "Usage: %s <path-to-svg>\n", argv[0]);
        return 1;
    }

    if (!game.loadFromSvg(argv[1])) {
        std::fprintf(stderr, "Error: failed to load SVG scene from '%s'\n", argv[1]);
        return 1;
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Trabalho CG 2D");

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    glutDisplayFunc(displayCallback);
    glutIdleFunc(idleCallback);
    glutReshapeFunc(reshapeCallback);
    glutKeyboardFunc(keyDownCallback);
    glutKeyboardUpFunc(keyUpCallback);
    glutSpecialFunc(specialKeyDownCallback);
    glutSpecialUpFunc(specialKeyUpCallback);
    glutMouseFunc(mouseButtonCallback);
    glutPassiveMotionFunc(mouseMoveCallback);

    lastTime = std::chrono::steady_clock::now();
    applyCamera();

    glutMainLoop();
    return 0;
}
