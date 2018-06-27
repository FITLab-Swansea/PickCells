#ifndef TESTSCENE2_H
#define TESTSCENE2_H

#include "scene.h"

#include <QDebug>
#include <iostream>
using namespace std;

class ColorScene : public Scene
{
public:
    ColorScene();

    void initializeScene(int brick_size);
    QList<QRectF> handleEvent(int x, int y, bool release);
};

#endif // TESTSCENE2_H
