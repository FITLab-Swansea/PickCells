#include "appsview.h"

AppsView::AppsView(QWidget *parent) : QGraphicsView(parent) {
    setFrameStyle(0);

    _scene = new QGraphicsScene();
    setScene(_scene);
    _scene->setBackgroundBrush(QColor("#444444"));

    _mouse_ptr = NULL;

    _general_update_timer = new QTimer(this);
    connect(_general_update_timer, SIGNAL(timeout()), this, SLOT(handle_general_update()));

    connect(&_test_app, SIGNAL(action(QString)), this, SLOT(handle_actions(QString)));
    connect(&_test_app, SIGNAL(display_info(QString)), this, SLOT(handle_display_info(QString)));
    connect(&_color_app, SIGNAL(action(QString)), this, SLOT(handle_actions(QString)));
    connect(&_color_app, SIGNAL(display_info(QString)), this, SLOT(handle_display_info(QString)));
    connect(&_social_app, SIGNAL(action(QString)), this, SLOT(handle_actions(QString)));
    connect(&_social_app, SIGNAL(display_info(QString)), this, SLOT(handle_display_info(QString)));
    connect(&_lock_app, SIGNAL(action(QString)), this, SLOT(handle_actions(QString)));
    connect(&_lock_app, SIGNAL(display_info(QString)), this, SLOT(handle_display_info(QString)));
    connect(&_art_app, SIGNAL(action(QString)), this, SLOT(handle_actions(QString)));
    connect(&_art_app, SIGNAL(display_info(QString)), this, SLOT(handle_display_info(QString)));
    connect(&_check_app, SIGNAL(action(QString)), this, SLOT(handle_actions(QString)));
    connect(&_check_app, SIGNAL(display_info(QString)), this, SLOT(handle_display_info(QString)));
    _app_mapping["app_test"] = &_test_app;
    _app_mapping["app_color"] = &_color_app;
    _app_mapping["app_social"] = &_social_app;
    _app_mapping["app_lock"] = &_lock_app;
    _app_mapping["app_art"] = &_art_app;
    _app_mapping["app_check"] = &_check_app;

    _configuration_mapping[QPair<QString, QString>("app_test","")] = "";
    _configuration_mapping[QPair<QString, QString>("app_color","")] = "";
    _configuration_mapping[QPair<QString, QString>("app_social","")] = "";
    _configuration_mapping[QPair<QString, QString>("app_lock","")] = "";
    _configuration_mapping[QPair<QString, QString>("app_art","")] = "";
    _configuration_mapping[QPair<QString, QString>("app_check","")] = "";

    _cur_scene = &_test_app;
    _cur_scene_name = "app_test";
    _next_configuration = NULL;
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
                    if (_next_configuration != NULL) {
                        rect_item->setBrush(QBrush(_next_configuration->getBackgroundColor()));
                    } else {
                        rect_item->setBrush(QBrush(_cur_scene->getBackgroundColor()));
                    }
                } else {
                    rect_item->setPen(QPen(Qt::black, 1, Qt::SolidLine));
                    rect_item->setBrush(QBrush(Qt::black,Qt::DiagCrossPattern));
                }
                _scene->addItem(rect_item);
                s->avatar = rect_item;
            }
        }

        _test_app.setWidgets(_scene);
        _test_app.initializeScene(_brick_size);
        _test_app.unsetWidgets(_scene);

        _color_app.setWidgets(_scene);
        _color_app.initializeScene(_brick_size);
        _color_app.unsetWidgets(_scene);

        _social_app.setWidgets(_scene);
        _social_app.initializeScene(_brick_size);
        _social_app.unsetWidgets(_scene);

        _lock_app.setWidgets(_scene);
        _lock_app.initializeScene(_brick_size);
        _lock_app.unsetWidgets(_scene);

        _art_app.setWidgets(_scene);
        _art_app.initializeScene(_brick_size);
        _art_app.unsetWidgets(_scene);

        _check_app.setWidgets(_scene);
        _check_app.initializeScene(_brick_size);
        _check_app.unsetWidgets(_scene);

        _cur_scene->setWidgets(_scene);

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

void AppsView::handle_new_touchframe(QJsonObject jsonObject) {
    CellsStates *states = CellsStates::getInstance();
    int x, y;
    bool success = states->getCellTopLeft(jsonObject["IMEI"].toString(), &x, &y);

    if (success) {
        float cell_x = (x + jsonObject["x"].toDouble()/240.0)*_brick_size;
        float cell_y = (y + jsonObject["y"].toDouble()/240.0)*_brick_size;
        if (jsonObject["type"].toInt() == 0) {
            QMouseEvent *event = new QMouseEvent(QEvent::MouseButtonPress, QPointF(cell_x, cell_y), Qt::LeftButton, Qt::LeftButton, 0);
            mousePressEvent(event);
        } else if (jsonObject["type"].toInt() == 2) {
            QMouseEvent *event = new QMouseEvent(QEvent::MouseMove, QPointF(cell_x, cell_y), Qt::LeftButton, Qt::LeftButton, 0);
            mouseMoveEvent(event);
        } else if (jsonObject["type"].toInt() == 1) {
            QMouseEvent *event = new QMouseEvent(QEvent::MouseButtonRelease, QPointF(cell_x, cell_y), Qt::LeftButton, Qt::LeftButton, 0);
            mouseReleaseEvent(event);
        }
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
            if (_cur_scene_name != action) {
                _cur_scene->unsetWidgets(_scene);
                _cur_scene_name = action;
                _cur_scene = _app_mapping[action];
                _cur_scene->setWidgets(_scene);
            }
            _cur_scene->handle_configuration();
            handle_general_update();
        } else {
            //qDebug() << "Error action!" << action << "does not exist";
            _cur_scene->handle_action(action);
            handle_general_update();
        }
    }
}

void AppsView::handle_new_configuration(QString configuration) {
    if (_app_mapping.contains(configuration)) {
        _next_configuration = _app_mapping[configuration];
    } else {
        _next_configuration = NULL;
    }
    updateStates();

    QPair<QString, QString> key(configuration,_cur_scene_name);
    if (_configuration_mapping.contains(key)) {
        handle_actions(_configuration_mapping[key]);
    } else if (_app_mapping.contains(configuration)) {
        handle_actions(configuration);
    } else {
        qDebug() << "Error configuration!" << key << "does not exist";
    }

}

void AppsView::handle_display_info(QString val) {
    emit display_info(val);
}

void AppsView::updateMouseEvent(QMouseEvent *event, bool release) {
    QList<QRectF> rect_to_update;
    if (_cur_scene != NULL) {
        qDebug() << event->modifiers();
        rect_to_update = _cur_scene->handleEvent(event->x(), event->y(), release, event->modifiers() == Qt::ControlModifier);
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
    for (int k = as->length()-1; k >= 0; k--) {
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

