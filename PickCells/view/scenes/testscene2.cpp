#include "testscene2.h"

TestScene2::TestScene2() : Scene() {
}

void TestScene2::initializeScene(int brick_size) {
    deleteWidgets();

    // button on 1,3 and 6,2
    int offset = 5;

    Button *tl_but = new Button(NULL, brick_size-2*offset, brick_size-2*offset);
    tl_but->setPos(1*brick_size + offset, 3*brick_size + offset);
    tl_but->setZValue(5);
    tl_but->setIcon(QPixmap(":/imgs/picture.png"));
    tl_but->setBackground(false);
    tl_but->setAction("");
    button_list.append(tl_but);

    Button *t_but = new Button(NULL, brick_size-2*offset, brick_size-2*offset);
    t_but->setPos(6*brick_size + offset, 2*brick_size + offset);
    t_but->setZValue(5);
    t_but->setIcon(QPixmap(":/imgs/close.png"));
    t_but->setAction("test_scene1");
    button_list.append(t_but);
}

QList<QRectF> TestScene2::handleEvent(int x, int y, bool release) {
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
