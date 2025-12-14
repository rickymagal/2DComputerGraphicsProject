#include "../../include/game/InputState.h"
#include <cstring>

InputState::InputState() {
    clear();
}

void InputState::clear() {
    std::memset(keys, 0, sizeof(keys));
    std::memset(specialKeys, 0, sizeof(specialKeys));
    mouseX = 0;
    mouseY = 0;
    mouseLeftPressed = false;
}
