#include "lockscene.h"

LockScene::LockScene() : Scene() {
    background_color = QColor("#FFFFFF");
}

void LockScene::setWidgets(QGraphicsScene *scene) {
    setWidgets_scene(scene);
}

void LockScene::unsetWidgets(QGraphicsScene *scene) {
    unsetWidgets_scene(scene);
}

void LockScene::handle_configuration() {
    handle_configuration_scene();
}

void LockScene::handle_action(QString action) {
    handle_action_scene(action);
}

void LockScene::initializeScene(int brick_size) {
    deleteWidgets();

    // button on 1,1 and 6,1
    int offset = 5;

    Button *but;

    but = new Button(NULL, brick_size-2*offset, brick_size-2*offset);
    but->setPos(1*brick_size + offset, 1*brick_size + offset);
    but->setZValue(5);
    if (CellsStates::getInstance()->getKeyWord() == QString("conf3")) {
        but->setIcon(QPixmap(":/imgs/facebook.png"));
    } else {
        but->setIcon(QPixmap(":/imgs/facebook-lock.png"));
    }
    but->setBackground(false);
    but->setAction("");
    button_list.append(but);

    if (CellsStates::getInstance()->getKeyWord() != QString("conf1")) {
        but = new Button(NULL, brick_size-2*offset, brick_size-2*offset);
        but->setPos(1*brick_size + offset, 2*brick_size + offset);
        but->setZValue(5);
        if (CellsStates::getInstance()->getKeyWord() == QString("conf3")) {
            but->setIcon(QPixmap(":/imgs/one-ok.png"));
        } else {
            but->setIcon(QPixmap(":/imgs/one-ko.png"));
        }
        but->setBackground(false);
        but->setAction("");
        button_list.append(but);

        but = new Button(NULL, brick_size-2*offset, brick_size-2*offset);
        but->setPos(2*brick_size + offset, 2*brick_size + offset);
        but->setZValue(5);
        if (CellsStates::getInstance()->getKeyWord() == QString("conf3")) {
            but->setIcon(QPixmap(":/imgs/two-ok.png"));
        } else {
            but->setIcon(QPixmap(":/imgs/two-ko.png"));
        }
        but->setBackground(false);
        but->setAction("");
        button_list.append(but);

        but = new Button(NULL, brick_size-2*offset, brick_size-2*offset);
        but->setPos(1*brick_size + offset, 3*brick_size + offset);
        but->setZValue(5);
        if (CellsStates::getInstance()->getKeyWord() == QString("conf3")) {
            but->setIcon(QPixmap(":/imgs/three-ok.png"));
        } else {
            but->setIcon(QPixmap(":/imgs/three-ko.png"));
        }
        but->setBackground(false);
        but->setAction("");
        button_list.append(but);

        but = new Button(NULL, brick_size-2*offset, brick_size-2*offset);
        but->setPos(2*brick_size + offset, 3*brick_size + offset);
        but->setZValue(5);
        if (CellsStates::getInstance()->getKeyWord() == QString("conf3")) {
            but->setIcon(QPixmap(":/imgs/four-ok.png"));
        } else {
            but->setIcon(QPixmap(":/imgs/four-ko.png"));
        }
        but->setBackground(false);
        but->setAction("");
        button_list.append(but);

    }

    but = new Button(NULL, brick_size-2*offset, brick_size-2*offset);
    but->setPos(2*brick_size + offset, 1*brick_size + offset);
    but->setZValue(5);
    if (CellsStates::getInstance()->getKeyWord() == QString("conf3")) {
        but->setIcon(QPixmap(":/imgs/unlocked.png"));
    } else {
        but->setIcon(QPixmap(":/imgs/locked.png"));
    }
    but->setBackground(false);
    but->setAction("");
    button_list.append(but);
}

QList<QRectF> LockScene::handleEvent(int x, int y, bool release, bool ctrl_button) {
    QList<QRectF> to_update;
    bool but_active = false;
    for (int k = 0; k < button_list.size(); k++) {
        if (button_list[k]->isActive()) {
            if (release) {
                button_list[k]->setActive(false);
                QRectF bbox(button_list[k]->x(),
                            button_list[k]->y(),
                            button_list[k]->boundingRect().width(),
                            button_list[k]->boundingRect().height());
                if (bbox.contains(x,y)) { emit action(button_list[k]->getAction()); }
                to_update.append(bbox);
                break;
            }
            but_active = true;
        }
    }

    if (!but_active && !release) {
        for (int k = 0; k < button_list.size(); k++) {
            QRectF bbox(button_list[k]->x(),
                        button_list[k]->y(),
                        button_list[k]->boundingRect().width(),
                        button_list[k]->boundingRect().height());
            if (bbox.contains(x,y)) {
                button_list[k]->setActive(true);
                to_update.append(bbox);
                break;
            }
        }
    }
    return to_update;
}
