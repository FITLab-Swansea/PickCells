#ifndef COLORSCENE_H
#define COLORSCENE_H

#include "scene.h"
#include "../widgets/swatch.h"
#include "../../controller/cellsstates.h"

#include <QDateTime>

#include <QDebug>
#include <iostream>
using namespace std;

enum SwatchState {
    Auto       = 0,
    Master     = 1
};

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
    QList<QRectF> handleEvent(int x, int y, bool release);

private:
    QMap<QString, int> swatch_states;
    QMap<QString, bool> swatches_visible;
    QMap<QString, Swatch*> swatches;

    QGraphicsScene *_scene;
};

#endif // COLORSCENE_H
