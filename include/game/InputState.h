#ifndef GAME_INPUT_STATE_H
#define GAME_INPUT_STATE_H

#include <cstdint>

struct InputState {
    bool keys[256];
    bool specialKeys[256];

    int mouseX;
    int mouseY;
    bool mouseLeftPressed;

    InputState();

    void clear();
};

#endif

