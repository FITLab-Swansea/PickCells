#ifndef COLORSCENE_H
#define COLORSCENE_H

#include "scene.h"
#include "../widgets/swatch.h"
#include "../../controller/cellsstates.h"

#include <QDateTime>
#include <QClipboard>
#include <QApplication>

#include <QDebug>
#include <iostream>
using namespace std;

class ColorScene : public Scene
{
public:
    ColorScene();

    void setWidgets(QGraphicsScene *scene);
    void unsetWidgets(QGraphicsScene *scene);
    void setSwatches();
    void unsetSwatches();
    void handle_configuration();
    void handle_action(QString action);


    void initializeScene(int brick_size);
    QList<QRectF> handleEvent(int x, int y, bool release, bool ctrl_button);

private:
    QMap<QString, Swatch*> swatches;

    QGraphicsScene *_scene;
};

#endif // COLORSCENE_H
