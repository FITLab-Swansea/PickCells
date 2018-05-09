#ifndef APPSVIEW_H
#define APPSVIEW_H

#include <QObject>
#include <QGraphicsView>
#include <QResizeEvent>
#include <QGraphicsScene>
#include <QTimer>
#include <QMap>
#include <QPair>

#include <QGraphicsEllipseItem>
#include "widgets/button.h"

#include "../controller/cellsstates.h"
#include "../controller/appscreen.h"
#include "scenes/scene.h"
#include "scenes/testscene1.h"
#include "scenes/testscene2.h"

#include <QDebug>
#include <iostream>
using namespace std;

class AppsView: public QGraphicsView
{
    Q_OBJECT
public:
    explicit AppsView(QWidget *parent = 0);
    ~AppsView();

    void resizeEvent(QResizeEvent* e);
    void updateStates();

    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

    void sceneUpdate() { _scene->update(); }

public slots:
    void handle_general_update();
    void handle_actions(QString action);
    void handle_new_configuration(QString configuration);

private:
    void updateMouseEvent(QMouseEvent *event, bool release);
    void updateCells(QRectF visual_change, bool persistent);
    void resetCells();

    QList<Cell*> _to_restore;
    QGraphicsScene *_scene;
    QGraphicsEllipseItem *_mouse_ptr;

    QTimer *_general_update_timer;
    int _brick_size;

    QMap<QPair<QString, QString>, QString> _configuration_mapping;

    QMap<QString, Scene*> _app_mapping;
    QString _cur_scene_name;
    Scene* _cur_scene;
    TestScene1 _test_scene1;
    TestScene2 _test_scene2;
};

#endif // APPSVIEW_H
