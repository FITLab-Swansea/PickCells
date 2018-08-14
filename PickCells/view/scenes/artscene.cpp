#include "artscene.h"

ArtScene::ArtScene() : Scene() {
    background_color = QColor("#FFFFFF");
}

void ArtScene::setWidgets(QGraphicsScene *scene) {
    setWidgets_scene(scene);
}

void ArtScene::unsetWidgets(QGraphicsScene *scene) {
    unsetWidgets_scene(scene);
}

void ArtScene::handle_configuration() {
    handle_configuration_scene();
}

void ArtScene::handle_action(QString action) {
    handle_action_scene(action);
}

void ArtScene::initializeScene(int brick_size) {
    deleteWidgets();

    Button *but;

    but = new Button(NULL, brick_size, brick_size);
    but->setPos(2*brick_size, 1*brick_size);
    but->setZValue(5);
    if ((CellsStates::getInstance()->getKeyWord() == QString("conf6")) ||
        (CellsStates::getInstance()->getKeyWord() == QString("conf7")) ||
        (CellsStates::getInstance()->getKeyWord() == QString("conf8"))) {
        but->setIcon(QPixmap(":/imgs/art3.png"));
    } else {
        but->setIcon(QPixmap(":/imgs/art0.png"));
    }
    but->setBackground(false);
    but->setAction("");
    button_list.append(but);

    but = new Button(NULL, brick_size, brick_size);
    but->setPos(3*brick_size, 1*brick_size);
    but->setZValue(5);
    if ((CellsStates::getInstance()->getKeyWord() == QString("conf7")) ||
        (CellsStates::getInstance()->getKeyWord() == QString("conf8"))) {
        but->setIcon(QPixmap(":/imgs/art4.png"));
    } else {
        but->setIcon(QPixmap(":/imgs/art0.png"));
    }
    but->setBackground(false);
    but->setAction("");
    button_list.append(but);

    but = new Button(NULL, brick_size, brick_size);
    but->setPos(1*brick_size, 2*brick_size);
    but->setZValue(5);
    if ((CellsStates::getInstance()->getKeyWord() == QString("conf7")) ||
        (CellsStates::getInstance()->getKeyWord() == QString("conf8"))) {
        but->setIcon(QPixmap(":/imgs/art2.png"));
    } else {
        but->setIcon(QPixmap(":/imgs/art0.png"));
    }
    but->setBackground(false);
    but->setAction("");
    button_list.append(but);

    but = new Button(NULL, brick_size, brick_size);
    but->setPos(2*brick_size, 2*brick_size);
    but->setZValue(5);
    but->setIcon(QPixmap(":/imgs/art1.png"));
    but->setBackground(false);
    but->setAction("");
    button_list.append(but);

    but = new Button(NULL, brick_size, brick_size);
    but->setPos(2*brick_size, 3*brick_size);
    but->setZValue(5);
    if ((CellsStates::getInstance()->getKeyWord() == QString("conf7")) ||
        (CellsStates::getInstance()->getKeyWord() == QString("conf8"))) {
        but->setIcon(QPixmap(":/imgs/art5.png"));
    } else {
        but->setIcon(QPixmap(":/imgs/art0.png"));
    }
    but->setBackground(false);
    but->setAction("");
    button_list.append(but);

    but = new Button(NULL, brick_size, brick_size);
    but->setPos(3*brick_size, 3*brick_size);
    but->setZValue(5);
    if (CellsStates::getInstance()->getKeyWord() == QString("conf8")) {
        but->setIcon(QPixmap(":/imgs/art6.png"));
    } else {
        but->setIcon(QPixmap(":/imgs/art0.png"));
    }
    but->setBackground(false);
    but->setAction("");
    button_list.append(but);
}

QList<QRectF> ArtScene::handleEvent(int x, int y, bool release, bool ctrl_button) {
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
