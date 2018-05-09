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
    virtual QList<QRectF> handleEvent(int x, int y, bool release) = 0;

    void setWidgets(QGraphicsScene *scene) {
        if (!is_set) {
            for (int k = 0; k < button_list.size(); k++) {
                scene->addItem(button_list[k]);
            }
            is_set = true;
        }
    }

    void unsetWidgets(QGraphicsScene *scene) {
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

    bool isSet() { return is_set; }

signals:
    void action(QString);

protected:
    bool is_set;
    QList<Button*> button_list;
};

#endif // SCENE_H
