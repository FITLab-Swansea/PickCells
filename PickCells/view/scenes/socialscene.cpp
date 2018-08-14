#include "socialscene.h"

SocialScene::SocialScene() : Scene() {
    background_color = QColor("#FFFFFF");
}

void SocialScene::setWidgets(QGraphicsScene *scene) {
    setWidgets_scene(scene);
}

void SocialScene::unsetWidgets(QGraphicsScene *scene) {
    unsetWidgets_scene(scene);
}

void SocialScene::handle_configuration() {
    handle_configuration_scene();
}

void SocialScene::handle_action(QString action) {
    handle_action_scene(action);
}

void SocialScene::initializeScene(int brick_size) {
    deleteWidgets();

    // button on 1,1 and 6,1
    int offset = 5;

    Button *but;
    if (CellsStates::getInstance()->getKeyWord() == QString("conf3")) {
        but = new Button(NULL, 2*brick_size-2*offset, 2*brick_size-2*offset);
    } else {
        but = new Button(NULL, brick_size-2*offset, brick_size-2*offset);
    }
    but->setPos(1*brick_size + offset, 1*brick_size + offset);
    but->setZValue(5);
    but->setIcon(QPixmap(":/imgs/open-book.png"));
    but->setBackground(false);
    but->setAction("");
    button_list.append(but);

    if (CellsStates::getInstance()->getKeyWord() != QString("conf3")) {
        but = new Button(NULL, brick_size-2*offset, brick_size-2*offset);
        but->setPos(1*brick_size + offset, 2*brick_size + offset);
        but->setZValue(5);
        if (CellsStates::getInstance()->getKeyWord() == QString("conf2")) {
            but->setIcon(QPixmap(":/imgs/facebook-lock.png"));
        } else {
            but->setIcon(QPixmap(":/imgs/facebook.png"));
        }
        but->setBackground(false);
        but->setAction("");
        button_list.append(but);

        but = new Button(NULL, brick_size-2*offset, brick_size-2*offset);
        but->setPos(2*brick_size + offset, 2*brick_size + offset);
        but->setZValue(5);
        if (CellsStates::getInstance()->getKeyWord() == QString("conf2")) {
            but->setIcon(QPixmap(":/imgs/gmail-lock.png"));
        } else {
            but->setIcon(QPixmap(":/imgs/gmail.png"));
        }
        but->setBackground(false);
        but->setAction("");
        button_list.append(but);
    }

    but = new Button(NULL, brick_size-2*offset, brick_size-2*offset);
    but->setPos(1*brick_size + offset, 3*brick_size + offset);
    but->setZValue(5);
    but->setIcon(QPixmap(":/imgs/network.png"));
    but->setBackground(false);
    but->setAction("");
    button_list.append(but);
}

QList<QRectF> SocialScene::handleEvent(int x, int y, bool release, bool ctrl_button) {
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
