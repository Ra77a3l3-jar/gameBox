#include <raylib.h>
#include "utils.h"

const char* GetKeyName(int key) {
    switch (key) {
        case KEY_W: return "W";
        case KEY_S: return "S";
        case KEY_UP: return "UP";
        case KEY_DOWN: return "DOWN";
        case KEY_A: return "A";
        case KEY_D: return "D";
        case KEY_LEFT: return "LEFT";
        case KEY_RIGHT: return "RIGHT";
        case KEY_SPACE: return "SPACE";
        case KEY_ENTER: return "ENTER";
        default: return TextFormat("%d", key);
    }
}
