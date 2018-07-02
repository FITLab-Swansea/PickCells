#ifndef SCENE_H
#define SCENE_H

#include <QObject>
#include <QList>
#include <QGraphicsScene>
#include "../widgets/button.h"

#include <QDebug>
#include <iostream>
using namespace std;

class Scene : public QObject
{
    Q_OBJECT
public:
    Scene() { is_set = false; }
    virtual void initializeScene(int brick_size) = 0;
    virtual QList<QRectF> handleEvent(int x, int y, bool release, bool ctrl_button) = 0;

    void setWidgets_scene(QGraphicsScene *scene) {
        if (!is_set) {
            for (int k = 0; k < button_list.size(); k++) {
                scene->addItem(button_list[k]);
            }
            is_set = true;
        }
    }

    void unsetWidgets_scene(QGraphicsScene *scene) {
        if (is_set) {
            for (int k = 0; k < button_list.size(); k++) {
                scene->removeItem(button_list[k]);
            }
            is_set = false;
        }
    }

    void deleteWidgets() {
        for (int k = 0; k < button_list.size(); k++) {
            delete button_list[k];
        }
        button_list.clear();
    }

    void handle_configuration_scene() {}
    void handle_action_scene(QString action) { /*qDebug() << "Error action scene!" << action << "does not exist";*/ }

    virtual void setWidgets(QGraphicsScene *scene) = 0;
    virtual void unsetWidgets(QGraphicsScene *scene) = 0;
    virtual void handle_configuration() = 0;
    virtual void handle_action(QString action) = 0;

    bool isSet() { return is_set; }

signals:
    void action(QString);
    void need_general_update();
    void display_info(QString);
    void put_into_clip_board(QString);

protected:
    bool is_set;
    QList<Button*> button_list;

    int _brick_size;
};

#endif // SCENE_H
