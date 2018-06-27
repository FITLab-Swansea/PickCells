#include "appsview.h"

AppsView::AppsView(QWidget *parent) : QGraphicsView(parent) {
    setFrameStyle(0);

    _scene = new QGraphicsScene();
    setScene(_scene);
    _scene->setBackgroundBrush(QColor("#444444"));

    _mouse_ptr = NULL;

    _general_update_timer = new QTimer(this);
    connect(_general_update_timer, SIGNAL(timeout()), this, SLOT(handle_general_update()));

    connect(&_test_scene1, SIGNAL(action(QString)), this, SLOT(handle_actions(QString)));

    connect(&_test_scene2, SIGNAL(action(QString)), this, SLOT(handle_actions(QString)));
    _app_mapping["test_scene"] = &_test_scene1;
    _app_mapping["color_scene"] = &_test_scene2;

    _configuration_mapping[QPair<QString, QString>("app_test","")] = "";
    _configuration_mapping[QPair<QString, QString>("app_color","")] = "";
}

AppsView::~AppsView() {

}

void AppsView::resizeEvent(QResizeEvent* e) {
    updateStates();
}

void AppsView::updateStates() {
    CellsStates *states = CellsStates::getInstance();

    QList<AppScreen*>* l_as = states->getLastAppSreens();
    if (l_as != NULL) {
        for (int k = 0; k < l_as->size(); k++) {
            if ((*l_as)[k]->avatar != NULL) {
                _scene->removeItem((*l_as)[k]->avatar);
                delete (*l_as)[k]->avatar;
                (*l_as)[k]->avatar = NULL;
            }
        }
        delete l_as;
        states->resetLastAppSreens();
    } else {
        l_as = states->getAppSreens();
        if (l_as != NULL) {
            for (int k = 0; k < l_as->size(); k++) {
                if ((*l_as)[k]->avatar != NULL) {
                    _scene->removeItem((*l_as)[k]->avatar);
                    delete (*l_as)[k]->avatar;
                    (*l_as)[k]->avatar = NULL;
                }
            }
        }
    }

    setSceneRect(0, 0, width(), height());
    _scene->setBackgroundBrush(QColor("#444444"));

    if (_mouse_ptr == NULL) {
        int ptr_size = 10;
        int brush_size = 2;

        _mouse_ptr = new QGraphicsEllipseItem();
        _mouse_ptr->setRect(-ptr_size-brush_size, -ptr_size-brush_size,
                            (ptr_size+brush_size)*2+1, (ptr_size+brush_size)*2+1);
        _mouse_ptr->setPen(QPen(Qt::white, brush_size, Qt::SolidLine));
        _mouse_ptr->setBrush(QBrush(Qt::black,Qt::NoBrush));
        _scene->addItem(_mouse_ptr);
        _mouse_ptr->setZValue(10);
        _mouse_ptr->hide();
    }

    QList<AppScreen*>* as = states->getAppSreens();
    if (as->size() > 0) {

        int w_workspace = 0;
        int h_workspace = 0;
        for (int k = 0; k < as->size(); k++) {
            AppScreen *s = (*as)[k];
            w_workspace = max(w_workspace,s->getX()+s->getW());
            h_workspace = max(h_workspace,s->getY()+s->getH());
        }

        _brick_size = min(width() / (w_workspace+2), height() / (h_workspace+2));

        for (int k = 0; k < as->size(); k++) {
            AppScreen *s = (*as)[k];

            if (s->avatar == NULL) {
                QGraphicsRectItem *rect_item = new QGraphicsRectItem();
                rect_item->setRect(0, 0, s->getW()*_brick_size, s->getH()*_brick_size);
                rect_item->setPos((1+s->getX())*_brick_size, (1+s->getY())*_brick_size);
                if (s->hasContent()) {
                    rect_item->setPen(QPen());
                    //rect_item->setBrush(QBrush(QColor(50+k*(100/as->size()), 100, 255 - k*(100/as->size())),Qt::SolidPattern));
                    rect_item->setBrush(QBrush(QColor("#2277FF")));
                } else {
                    rect_item->setPen(QPen(Qt::black, 1, Qt::SolidLine));
                    rect_item->setBrush(QBrush(Qt::black,Qt::DiagCrossPattern));
                }
                _scene->addItem(rect_item);
                s->avatar = rect_item;
            }
        }

        _test_scene1.unsetWidgets(_scene);
        _test_scene1.initializeScene(_brick_size);
        _test_scene1.setWidgets(_scene);
        _cur_scene = &_test_scene1;
        _cur_scene_name = "conf_test1";

        _test_scene2.unsetWidgets(_scene);
        _test_scene2.initializeScene(_brick_size);

        _general_update_timer->stop();
        _general_update_timer->start(500);
    }
}

void AppsView::mouseMoveEvent(QMouseEvent *event) {
    if (_mouse_ptr != NULL) {
        resetCells();
        updateMouseEvent(event, false);
    }
}

void AppsView::mousePressEvent(QMouseEvent *event) {
    if (_mouse_ptr != NULL) {
        _mouse_ptr->show();
        updateMouseEvent(event, false);
    }
}

void AppsView::mouseReleaseEvent(QMouseEvent *event) {
    if (_mouse_ptr != NULL) {
        _mouse_ptr->hide();
        resetCells();
        updateMouseEvent(event, true);
    }
}

void AppsView::handle_general_update() {
    _general_update_timer->stop();
    bool ptr_in = _mouse_ptr->isVisible();
    if (ptr_in) { _mouse_ptr->hide(); }
    updateCells(QRectF(0,0,_scene->width(),_scene->height()), true);
    if (ptr_in) { _mouse_ptr->show(); }
}

void AppsView::handle_actions(QString action) {
    if (action != "") {
        if (_app_mapping.contains(action)) {
            _cur_scene->unsetWidgets(_scene);
            _cur_scene_name = action;
            _cur_scene = _app_mapping[action];
            _cur_scene->setWidgets(_scene);

            handle_general_update();
        } else {
            qDebug() << "Error action!" << action << "does not exist";
        }
    }
}

void AppsView::handle_new_configuration(QString configuration) {
    updateStates();

    QPair<QString, QString> key(configuration,_cur_scene_name);
    if (_configuration_mapping.contains(key)) {
        handle_actions(_configuration_mapping[key]);
    } else {
        qDebug() << "Error configuration!" << key << "does not exist";
    }
}

void AppsView::updateMouseEvent(QMouseEvent *event, bool release) {
    QList<QRectF> rect_to_update;
    if (_cur_scene) {
        rect_to_update = _cur_scene->handleEvent(event->x(), event->y(), release);
    }

    bool ptr_in = _mouse_ptr->isVisible();
    if (ptr_in) { _mouse_ptr->hide(); }
    for (int k = 0; k < rect_to_update.size(); k++) {
        updateCells(rect_to_update[k], true);
    }
    if (ptr_in) { _mouse_ptr->show(); }

    if (_mouse_ptr->isVisible()) {
        _mouse_ptr->setPos(event->x(), event->y());

        if (CellsStates::getInstance()->getDebug()) {
            QRectF bbox_ptr = _mouse_ptr->rect();
            bbox_ptr.setRect(bbox_ptr.x()+event->x(),
                             bbox_ptr.y()+event->y(),
                             bbox_ptr.width(),
                             bbox_ptr.height());
            updateCells(bbox_ptr,false);
        }
    }
}

void AppsView::updateCells(QRectF visual_change, bool persistent) {
    CellsStates *states = CellsStates::getInstance();
    QList<AppScreen*>* as = states->getAppSreens();

    QList<Cell*> to_update;
    for (int k = as->size()-1; k >= 0; k--) {
        AppScreen *s = (*as)[k];
        QRectF bbox_avt = s->avatar->rect();
        bbox_avt.setRect(bbox_avt.x()+(s->getX()+1)*_brick_size,
                         bbox_avt.y()+(s->getY()+1)*_brick_size,
                         bbox_avt.width(),
                         bbox_avt.height());
        if (bbox_avt.intersects(visual_change)) {
            for (int x = 0; x < s->getW(); x++) {
                for (int y = 0; y < s->getH(); y++) {
                    QRectF tile(bbox_avt.x() + x*_brick_size,
                                bbox_avt.y() + y*_brick_size,
                                _brick_size, _brick_size);
                    if (tile.intersects(visual_change)) {
                        Cell *c = states->getCell(s->getTLDevice(),
                                                  s->getTLLayer(),
                                                  s->getTLRow() + y,
                                                  s->getTLCol() + x);
                        if (c != NULL) {
                            if (persistent) {
                                c->setPixmap(grab(tile.toRect()));
                                c->setAvatarPixmap(c->getPixmap());
                            } else {
                                c->setAvatarPixmap(grab(tile.toRect()));
                                _to_restore.append(c);
                            }
                            c->avatar()->update();
                            to_update.append(c);
                        }
                    }
                }
            }
        }
    }
    _scene->update();

    for (int k = 0; k < to_update.size(); k++) {
        to_update[k]->sendNewVisual();
    }
}

void AppsView::resetCells() {
    for (int k = 0; k < _to_restore.size(); k++) {
        _to_restore[k]->setAvatarPixmap(_to_restore[k]->getPixmap());
        _to_restore[k]->avatar()->update();
        _to_restore[k]->sendNewVisual();
    }
    _to_restore.clear();
}

