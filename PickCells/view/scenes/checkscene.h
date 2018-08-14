#ifndef CHECKSCENE_H
#define CHECKSCENE_H

#include "scene.h"

#include <QDebug>
#include <iostream>
using namespace std;

class CheckScene : public Scene
{
public:
    CheckScene();

    void setWidgets(QGraphicsScene *scene);
    void unsetWidgets(QGraphicsScene *scene);
    void handle_configuration();
    void handle_action(QString action);

    void initializeScene(int brick_size);
    QList<QRectF> handleEvent(int x, int y, bool release, bool ctrl_button);
};

#endif // CHECKSCENE_H
