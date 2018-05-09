#ifndef TESTSCENE1_H
#define TESTSCENE1_H

#include "scene.h"

#include <QDebug>
#include <iostream>
using namespace std;

class TestScene1 : public Scene
{
public:
    TestScene1();

    void initializeScene(int brick_size);
    QList<QRectF> handleEvent(int x, int y, bool release);
};

#endif // TESTSCENE1_H
