#ifndef LOCKSCENE_H
#define LOCKSCENE_H

#include "scene.h"

#include <QDebug>
#include <iostream>
using namespace std;

class LockScene : public Scene
{
public:
    LockScene();

    void setWidgets(QGraphicsScene *scene);
    void unsetWidgets(QGraphicsScene *scene);
    void handle_configuration();
    void handle_action(QString action);

    void initializeScene(int brick_size);
    QList<QRectF> handleEvent(int x, int y, bool release, bool ctrl_button);
};

#endif // LOCKSCENE_H
