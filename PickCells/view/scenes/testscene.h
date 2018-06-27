#ifndef TESTSCENE_H
#define TESTSCENE_H

#include "scene.h"

#include <QDebug>
#include <iostream>
using namespace std;

class TestScene : public Scene
{
public:
    TestScene();

    void initializeScene(int brick_size);
    QList<QRectF> handleEvent(int x, int y, bool release);
};

#endif // TESTSCENE_H
