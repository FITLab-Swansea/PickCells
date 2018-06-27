#include "colorscene.h"

ColorScene::ColorScene() : Scene() {
}

void ColorScene::setWidgets(QGraphicsScene *scene) {
    setWidgets_scene(scene);
    setSwatches();
    _scene = scene;
}

void ColorScene::unsetWidgets(QGraphicsScene *scene) {
    unsetWidgets_scene(scene);
    unsetSwatches();
    _scene = scene;
}

void ColorScene::setSwatches() {
    QList<QString> swatches_id = swatch_states.keys();
    for (int k = 0; k < swatches_id.length(); k++) {
        if (swatches_visible[swatches_id[k]]) {
            _scene->addItem(swatches[swatches_id[k]]);
        }
    }
}

void ColorScene::unsetSwatches() {
    QList<QString> swatches_id = swatch_states.keys();
    for (int k = 0; k < swatches_id.length(); k++) {
        _scene->removeItem(swatches[swatches_id[k]]);
    }
}

void ColorScene::handle_configuration() {
    handle_configuration_scene();
    qDebug() << "vamos a manejar el conf";

    unsetSwatches();
    QList<QString> swatches_id = swatch_states.keys();
    for (int k = 0; k < swatches_id.length(); k++) {
        swatches_visible[swatches_id[k]] = false;
    }

    CellsStates *states = CellsStates::getInstance();
    int offset = 0;
    for (int device = 0; device < states->getNbDevices(); device++ ) {
        offset += 1;
        for (int row = 0; row < states->getDeviceHeight(device); row++) {
            for (int col = 0; col < states->getDeviceWidth(device); col++) {
                for (int layer = states->getDeviceDepth(device)-1; layer >=0; layer--) {
                    Cell *c = states->getCell(device, layer, row, col);
                    if (c != NULL) {
                        if (swatch_states.contains(c->getCellId())) {
                            swatches[c->getCellId()]->initializeSize(_brick_size, _brick_size);
                        } else {
                            swatch_states[c->getCellId()] = SwatchState::Auto;
                            Swatch *swt = new Swatch(NULL, _brick_size, _brick_size);
                            swt->setZValue(5);
                            swatches[c->getCellId()] = swt;
                            swatches[c->getCellId()]->setColorBackground(QColor("#37495C"), QColor("#2F3E4E"));
                        }

                        swatches[c->getCellId()]->setPos((col+offset)*_brick_size, (row+1)*_brick_size);
                        swatches[c->getCellId()]->setAction("randcolor:"+c->getCellId());
                        swatches[c->getCellId()]->setLongAction("master:"+c->getCellId());
                        swatches_visible[c->getCellId()] = true;
                        swatches[c->getCellId()]->update();
                        break;
                    }
                }
            }
        }
        offset += states->getDeviceWidth(device);
    }
    setSwatches();
}

void ColorScene::handle_action(QString action) {
    handle_action_scene(action);
    qDebug() << "vamos a manejar el action " << action;
    QList<QString> act_id = action.split(':');
    if (act_id.length() == 2) {
        if (act_id[0] == QString("master")) {
            if (swatch_states[act_id[1]] == SwatchState::Auto) {
                swatch_states[act_id[1]] = SwatchState::Master;
                swatches[act_id[1]]->setColorBackground(QColor("#99495C"), QColor("#2F3E4E"));
            } else {
                swatch_states[act_id[1]] = SwatchState::Auto;
                swatches[act_id[1]]->setColorBackground(QColor("#37495C"), QColor("#2F3E4E"));
            }
        } else {
            if (act_id[0] == QString("randcolor")) {
                if (swatch_states[act_id[1]] == SwatchState::Master) {
                    QColor rand_col;
                    rand_col.setRed(rand() % 256);
                    rand_col.setGreen(rand() % 256);
                    rand_col.setBlue(rand() % 256);
                    swatches[act_id[1]]->setColorBackground(QColor("#99495C"), rand_col);
                }
            }
        }
    }
}

void ColorScene::initializeScene(int brick_size) {
    deleteWidgets();

    // button on 0,0
    int offset = 5;

    Button *ref_but = new Button(NULL, brick_size-2*offset, brick_size-2*offset);
    ref_but->setPos(0*brick_size + offset, 0*brick_size + offset);
    ref_but->setZValue(5);
    ref_but->setIcon(QPixmap(":/imgs/refresh.png"));
    ref_but->setBackground(true);
    ref_but->setColorBackground(QColor("#37495C"), QColor("#2F3E4E"), QColor("#47596C"));
    ref_but->setAction("refresh");
    button_list.append(ref_but);

    _brick_size = brick_size;
}

QList<QRectF> ColorScene::handleEvent(int x, int y, bool release) {
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

    bool swatch_active = false;
    QList<QString> swatches_id = swatch_states.keys();
    for (int k = 0; k < swatches_id.length(); k++) {
        if (swatches_visible[swatches_id[k]] && swatches[swatches_id[k]]->isActive()) {
            if (release) {
                swatches[swatches_id[k]]->setActive(false);
                QRectF bbox(swatches[swatches_id[k]]->x(),
                            swatches[swatches_id[k]]->y(),
                            swatches[swatches_id[k]]->boundingRect().width(),
                            swatches[swatches_id[k]]->boundingRect().height());
                if (bbox.contains(x,y)) {
                    int elapse_time = swatches[swatches_id[k]]->getTime();
                    qDebug() << elapse_time;
                    if (elapse_time < 1000) {
                        emit action(swatches[swatches_id[k]]->getAction());
                    } else {
                        emit action(swatches[swatches_id[k]]->getLongAction());
                    }
                }
                to_update.append(bbox);
                break;
            }
            swatch_active = true;
        }
    }
    if (!swatch_active && !release) {
        QList<QString> swatches_id = swatch_states.keys();
        for (int k = 0; k < swatches_id.length(); k++) {
            QRectF bbox(swatches[swatches_id[k]]->x(),
                        swatches[swatches_id[k]]->y(),
                        swatches[swatches_id[k]]->boundingRect().width(),
                        swatches[swatches_id[k]]->boundingRect().height());
            if (bbox.contains(x,y)) {
                swatches[swatches_id[k]]->setActive(true);
                swatches[swatches_id[k]]->startTime();
                to_update.append(bbox);
                break;
            }
        }
    }
    return to_update;
}
