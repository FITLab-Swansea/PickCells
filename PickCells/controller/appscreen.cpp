#include "appscreen.h"

AppScreen::AppScreen(QObject *parent) : QObject(parent) {

}

void AppScreen::initialize() {
    _x = 0;
    _y = 0;
    _width = 0;
    _height = 0;
    _has_content = false;

    _device = 0;
    _layer = 0;
    _row = 0;
    _col = 0;

    avatar = NULL;
}
