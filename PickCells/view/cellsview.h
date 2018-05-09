#ifndef CELLSVIEW_H
#define CELLSVIEW_H

#include <QObject>
#include <QGraphicsView>
#include <QResizeEvent>
#include <QGraphicsScene>

#include "../controller/cellsstates.h"
#include "brick.h"

#include <QDebug>
#include <iostream>
using namespace std;

class CellsView: public QGraphicsView
{
    Q_OBJECT
public:
    explicit CellsView(QWidget *parent = 0);
    ~CellsView();

    void resizeEvent(QResizeEvent* e);
    void updateStates();

    void setImage(QString file_path);

    void sceneUpdate() { _scene->update(); }

public slots:
    void handle_new_configuration(QString configuration);

private:
    QGraphicsScene *_scene;
};

#endif // CELLSVIEW_H
