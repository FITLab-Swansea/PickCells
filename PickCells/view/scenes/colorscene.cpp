#include "colorscene.h"
#include <math.h>

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
    QList<QString> swatches_id = swatches.keys();
    for (int k = 0; k < swatches_id.length(); k++) {
        if (swatches[swatches_id[k]]->swatche_visible) {
            _scene->addItem(swatches[swatches_id[k]]);
            swatches[swatches_id[k]]->update();
        }
    }
}

void ColorScene::unsetSwatches() {
    QList<QString> swatches_id = swatches.keys();
    for (int k = 0; k < swatches_id.length(); k++) {
        _scene->removeItem(swatches[swatches_id[k]]);
    }
}

void ColorScene::handle_configuration() {
    handle_configuration_scene();

    qDebug() << "handle_configuration";

    unsetSwatches();
    QList<QString> swatches_id = swatches.keys();
    for (int k = 0; k < swatches_id.length(); k++) {
        swatches[swatches_id[k]]->swatche_visible = false;
    }

    QList<QList<QString > > masters_and_autos;
    QList<QList<QString > > masters;
    QList<QList<QString > > autos;
    CellsStates *states = CellsStates::getInstance();
    int offset = 0;
    for (int device = 0; device < states->getNbDevices(); device++ ) {
        QList<QString > cur_masters_and_autos;
        QList<QString > cur_masters;
        QList<QString > cur_autos;

        offset += 1;
        for (int row = 0; row < states->getDeviceHeight(device); row++) {
            for (int col = 0; col < states->getDeviceWidth(device); col++) {
                for (int layer = states->getDeviceDepth(device)-1; layer >=0; layer--) {
                    Cell *c = states->getCell(device, layer, row, col);
                    if (c != NULL) {
                        if (swatches.contains(c->getCellId())) {
                            swatches[c->getCellId()]->initializeSize(_brick_size, _brick_size);
                        } else {
                            Swatch *swt = new Swatch(NULL, _brick_size, _brick_size);
                            swt->setZValue(5);
                            swatches[c->getCellId()] = swt;
                            swatches[c->getCellId()]->swatche_state = SwatchState::Auto;
                            swatches[c->getCellId()]->setColorBackground(QColor("#2F3E4E"), QColor("#2F3E4E"));
                        }

                        swatches[c->getCellId()]->setPos((col+offset)*_brick_size, (row+1)*_brick_size);
                        swatches[c->getCellId()]->setAction("tap:"+c->getCellId());
                        swatches[c->getCellId()]->setLongAction("master:"+c->getCellId());
                        swatches[c->getCellId()]->swatche_visible = true;
                        swatches[c->getCellId()]->swatche_pos = QPair<int, int>(col,row);
                        swatches[c->getCellId()]->processed = false;
                        swatches[c->getCellId()]->update();
                        if (swatches[c->getCellId()]->swatche_state == SwatchState::Auto) {
                            swatches[c->getCellId()]->color_to_be_set = true;
                            cur_autos.append(c->getCellId());
                        } else {
                            swatches[c->getCellId()]->color_to_be_set = false;
                            cur_masters.append(c->getCellId());
                        }
                        cur_masters_and_autos.append(c->getCellId());
                        break;
                    }
                }
            }
        }
        offset += states->getDeviceWidth(device);

        masters_and_autos.append(cur_masters_and_autos);
        masters.append(cur_masters);
        autos.append(cur_autos);
    }

    // Update colors
    for (int device = 0; device < masters.length(); device++) {
        if (masters.length()) {
            QList<QString> to_process_masters = masters[device];
            QList<QString> to_process_autos = autos[device];

            QList<QString> new_masters = to_process_masters;

            while (new_masters.length()) {
                QList<QString> old_masters = new_masters;
                new_masters.clear();

                // Hue
                qDebug() << "####### Hue";
                to_process_masters = old_masters;
                while (to_process_masters.length()) {
                    QString cur_master = to_process_masters.first();
                    to_process_masters.pop_front();
                    int x = swatches[cur_master]->swatche_pos.first;
                    int y = swatches[cur_master]->swatche_pos.second;
                    qDebug() << "master " << cur_master << " " << x << " " << y;
                    swatches[cur_master]->processed = true;

                    // top
                    QString top_swatch = "";
                    int top_y = -9999;
                    int min_y = y;
                    for (int k = 0; k < masters_and_autos[device].length(); k++) {
                        int _x = swatches[masters_and_autos[device][k]]->swatche_pos.first;
                        int _y = swatches[masters_and_autos[device][k]]->swatche_pos.second;
                        if ((_x == x) && (_y < y)) {
                            if (swatches[masters_and_autos[device][k]]->processed || (swatches[masters_and_autos[device][k]]->swatche_state == SwatchState::Master)) {
                                if (_y > top_y) {
                                    top_y = _y;
                                    top_swatch = masters_and_autos[device][k];
                                }
                            }
                            if (_y < min_y) {
                                min_y = _y;
                            }
                        }
                    }
                    if (top_y == -9999) { top_y = y; }
                    qDebug() << "   -> top         " << top_swatch;
                    qDebug() << "      top_y min_y " << top_y << " " << min_y;

                    // bottom
                    QString bot_swatch = "";
                    int bot_y = 9999;
                    int max_y = y;
                    for (int k = 0; k < masters_and_autos[device].length(); k++) {
                        int _x = swatches[masters_and_autos[device][k]]->swatche_pos.first;
                        int _y = swatches[masters_and_autos[device][k]]->swatche_pos.second;
                        if ((_x == x) && (_y > y)) {
                            if (swatches[masters_and_autos[device][k]]->processed || (swatches[masters_and_autos[device][k]]->swatche_state == SwatchState::Master)) {
                                if (_y < bot_y) {
                                    bot_y = _y;
                                    bot_swatch = masters_and_autos[device][k];
                                }
                            }
                            if (_y > max_y) {
                                max_y = _y;
                            }
                        }
                    }
                    if (bot_y == 9999) { bot_y = y; }
                    qDebug() << "   -> bot         " << bot_swatch;
                    qDebug() << "      bot_y max_y " << bot_y << " " << max_y;

                    // set auto only one master
                    if ((bot_swatch == QString("")) && (top_swatch == QString(""))) {
                        for (int k = 0; k < to_process_autos.length(); k++) {
                            int _x = swatches[to_process_autos[k]]->swatche_pos.first;
                            int _y = swatches[to_process_autos[k]]->swatche_pos.second;
                            if (!swatches[to_process_autos[k]]->processed && (_x == x)) {
//                                if (!swatches[to_process_autos[k]]->processed) {
                                    new_masters.append(to_process_autos[k]);
//                                }
                                swatches[to_process_autos[k]]->processed = true;
                                qreal h_m, s_m, v_m;
                                swatches[cur_master]->background.getHsvF(&h_m, &s_m, &v_m);

                                float ratio = 1.0 / (max_y - min_y + 1);
                                QColor col;
                                qreal h = h_m - ratio*(y - _y);
                                h = (h>1?h-1.0:h);
                                h = (h<0?h+1.0:h);
                                qreal s = s_m;
                                qreal v = v_m;
                                col.setHsvF(h,s,v);
                                swatches[to_process_autos[k]]->setColorBackground(col,col);

                                qDebug() << "      set auto only one master";
                                qDebug() << "      setting   " << to_process_autos[k] << " " << _y;
                                qDebug() << "          ratio " << ratio;
                                qDebug() << "          hsv m " << h_m << " " << s_m << " " << v_m;
                                qDebug() << "          hsv   " << h << " " << s << " " << v;
                            }
                        }
                    } else {
                        // set auto above master -- when below
                        if ((top_swatch == QString("")) && (bot_swatch != QString(""))) {
                            for (int k = 0; k < to_process_autos.length(); k++) {
                                int _x = swatches[to_process_autos[k]]->swatche_pos.first;
                                int _y = swatches[to_process_autos[k]]->swatche_pos.second;
                                if (!swatches[to_process_autos[k]]->processed && (_x == x) && (_y < y) /*&& (_y > top_y)*/) {
//                                    if (!swatches[to_process_autos[k]]->processed) {
                                        new_masters.append(to_process_autos[k]);
//                                    }
                                    swatches[to_process_autos[k]]->processed = true;
                                    qreal h_m, s_m, v_m;
                                    swatches[cur_master]->background.getHsvF(&h_m, &s_m, &v_m);
                                    qreal h_t, s_t, v_t;
                                    swatches[bot_swatch]->background.getHsvF(&h_t, &s_t, &v_t);
                                    int top_y = min_y - 1;

                                    float ratio = ((float)(top_y - _y)) / ((float)(top_y - y));

                                    QColor col;
                                    h_t = (h_t>h_m?h_t-1.0:h_t);
                                    qreal h = h_t + ratio*(h_m - h_t);
                                    h = (h>1?h-1.0:h);
                                    h = (h<0?h+1.0:h);
                                    qreal s = min(s_m,s_t) + ratio*(max(s_m,s_t) - min(s_m,s_t));
                                    qreal v = min(v_m,s_t) + ratio*(max(v_m,v_t) - min(v_m,v_t));
                                    col.setHsvF(h,s,v);
                                    swatches[to_process_autos[k]]->setColorBackground(col,col);

                                    qDebug() << "      set auto above master -- when below";
                                    qDebug() << "      setting   " << to_process_autos[k] << " " << _y;
                                    qDebug() << "          ratio " << ratio;
                                    qDebug() << "          hsv m " << h_m << " " << s_m << " " << v_m;
                                    qDebug() << "          hsv t " << h_t << " " << s_t << " " << v_t;
                                    qDebug() << "          hsv   " << h << " " << s << " " << v;
                                }
                            }
                        }

                        // set auto below master -- when above
                        if ((bot_swatch == QString("")) && (top_swatch != QString(""))) {
                            for (int k = 0; k < to_process_autos.length(); k++) {
                                int _x = swatches[to_process_autos[k]]->swatche_pos.first;
                                int _y = swatches[to_process_autos[k]]->swatche_pos.second;
                                if (!swatches[to_process_autos[k]]->processed && (_x == x) && (_y > y) /*&& (_y < bot_y)*/) {
//                                    if (!swatches[to_process_autos[k]]->processed) {
                                        new_masters.append(to_process_autos[k]);
//                                    }
                                    swatches[to_process_autos[k]]->processed = true;
                                    qreal h_m, s_m, v_m;
                                    swatches[cur_master]->background.getHsvF(&h_m, &s_m, &v_m);
                                    qreal h_b, s_b, v_b;
                                    swatches[top_swatch]->background.getHsvF(&h_b, &s_b, &v_b);
                                    int bot_y = max_y + 1;

                                    float ratio = ((float)(y - _y)) / ((float)(y - bot_y));

                                    QColor col;
                                    h_b = (h_b<h_m?h_b+1.0:h_b);
                                    qreal h = h_m + ratio*(h_b - h_m);
                                    h = (h>1?h-1.0:h);
                                    h = (h<0?h+1.0:h);
                                    qreal s = min(s_m,s_b) + ratio*(max(s_m,s_b) - min(s_m,s_b));
                                    qreal v = min(v_m,s_b) + ratio*(max(v_m,v_b) - min(v_m,v_b));
                                    col.setHsvF(h,s,v);
                                    swatches[to_process_autos[k]]->setColorBackground(col,col);

                                    qDebug() << "      set auto below master -- when above";
                                    qDebug() << "      setting   " << to_process_autos[k] << " " << _y;
                                    qDebug() << "          ratio " << ratio;
                                    qDebug() << "          hsv m " << h_m << " " << s_m << " " << v_m;
                                    qDebug() << "          hsv b " << h_b << " " << s_b << " " << v_b;
                                    qDebug() << "          hsv   " << h << " " << s << " " << v;
                                }
                            }
                        }

                        // set auto between master -- top
                        if (top_swatch != QString("")) {
                            for (int k = 0; k < to_process_autos.length(); k++) {
                                int _x = swatches[to_process_autos[k]]->swatche_pos.first;
                                int _y = swatches[to_process_autos[k]]->swatche_pos.second;
                                if (!swatches[to_process_autos[k]]->processed && (_x == x) && (_y < y) && (_y > top_y)) {
//                                    if (!swatches[to_process_autos[k]]->processed) {
                                        new_masters.append(to_process_autos[k]);
//                                    }
                                    swatches[to_process_autos[k]]->processed = true;
                                    qreal h_m, s_m, v_m;
                                    swatches[cur_master]->background.getHsvF(&h_m, &s_m, &v_m);
                                    qreal h_t, s_t, v_t;
                                    swatches[top_swatch]->background.getHsvF(&h_t, &s_t, &v_t);
                                    int top_y = swatches[top_swatch]->swatche_pos.second;

                                    float ratio = ((float)(top_y - _y)) / ((float)(top_y - y));

                                    QColor col;
                                    h_t = (h_t>h_m?h_t-1.0:h_t);
                                    qreal h = h_t + ratio*(h_m - h_t);
                                    h = (h>1?h-1.0:h);
                                    h = (h<0?h+1.0:h);
                                    qreal s = min(s_m,s_t) + ratio*(max(s_m,s_t) - min(s_m,s_t));
                                    qreal v = min(v_m,s_t) + ratio*(max(v_m,v_t) - min(v_m,v_t));
                                    col.setHsvF(h,s,v);
                                    swatches[to_process_autos[k]]->setColorBackground(col,col);

                                    qDebug() << "      set auto between master -- top";
                                    qDebug() << "      setting   " << to_process_autos[k] << " " << _y;
                                    qDebug() << "          ratio " << ratio;
                                    qDebug() << "          hsv m " << h_m << " " << s_m << " " << v_m;
                                    qDebug() << "          hsv t " << h_t << " " << s_t << " " << v_t;
                                    qDebug() << "          hsv   " << h << " " << s << " " << v;
                                }
                            }
                        }

                        // set auto between master -- bot
                        if (bot_swatch != QString("")) {
                            for (int k = 0; k < to_process_autos.length(); k++) {
                                int _x = swatches[to_process_autos[k]]->swatche_pos.first;
                                int _y = swatches[to_process_autos[k]]->swatche_pos.second;
                                if (!swatches[to_process_autos[k]]->processed && (_x == x) && (_y > y) && (_y < bot_y)) {
//                                    if (!swatches[to_process_autos[k]]->processed) {
                                        new_masters.append(to_process_autos[k]);
//                                    }
                                    swatches[to_process_autos[k]]->processed = true;
                                    qreal h_m, s_m, v_m;
                                    swatches[cur_master]->background.getHsvF(&h_m, &s_m, &v_m);
                                    qreal h_b, s_b, v_b;
                                    swatches[bot_swatch]->background.getHsvF(&h_b, &s_b, &v_b);
                                    int bot_y = swatches[bot_swatch]->swatche_pos.second;

                                    float ratio = ((float)(y - _y)) / ((float)(y - bot_y));

                                    QColor col;
                                    h_b = (h_b<h_m?h_b+1.0:h_b);
                                    qreal h = h_m + ratio*(h_b - h_m);
                                    h = (h>1?h-1.0:h);
                                    h = (h<0?h+1.0:h);
                                    qreal s = min(s_m,s_b) + ratio*(max(s_m,s_b) - min(s_m,s_b));
                                    qreal v = min(v_m,s_b) + ratio*(max(v_m,v_b) - min(v_m,v_b));
                                    col.setHsvF(h,s,v);
                                    swatches[to_process_autos[k]]->setColorBackground(col,col);

                                    qDebug() << "      set auto between master -- bot";
                                    qDebug() << "      setting   " << to_process_autos[k] << " " << _y;
                                    qDebug() << "          ratio " << ratio;
                                    qDebug() << "          hsv m " << h_m << " " << s_m << " " << v_m;
                                    qDebug() << "          hsv b " << h_b << " " << s_b << " " << v_b;
                                    qDebug() << "          hsv   " << h << " " << s << " " << v;
                                }
                            }
                        }
                    }
                }

                // Value
                qDebug() << "####### Value";
                to_process_masters = old_masters;
                while (to_process_masters.length()) {
                    QString cur_master = to_process_masters.first();
                    to_process_masters.pop_front();
                    int x = swatches[cur_master]->swatche_pos.first;
                    int y = swatches[cur_master]->swatche_pos.second;
                    qDebug() << "master " << cur_master << " " << x << " " << y;
                    swatches[cur_master]->processed = true;

                    // left
                    QString left_swatch = "";
                    int left_x = -9999;
                    int min_x = x;
                    for (int k = 0; k < masters_and_autos[device].length(); k++) {
                        int _x = swatches[masters_and_autos[device][k]]->swatche_pos.first;
                        int _y = swatches[masters_and_autos[device][k]]->swatche_pos.second;
                        if ((_y == y) && (_x < x)) {
                            if (swatches[masters_and_autos[device][k]]->processed || (swatches[masters_and_autos[device][k]]->swatche_state == SwatchState::Master)) {
                                if (_x > left_x) {
                                    left_x = _x;
                                    left_swatch = masters_and_autos[device][k];
                                }
                            }
                            if (_x < min_x) {
                                min_x = _x;
                            }
                        }
                    }
                    if (left_x == -9999) { left_x = x; }
                    qDebug() << "   -> left         " << left_swatch;
                    qDebug() << "      left_x min_x " << left_x << " " << min_x;

                    // right
                    QString right_swatch = "";
                    int right_x = 9999;
                    int max_x = x;
                    for (int k = 0; k < masters_and_autos[device].length(); k++) {
                        int _x = swatches[masters_and_autos[device][k]]->swatche_pos.first;
                        int _y = swatches[masters_and_autos[device][k]]->swatche_pos.second;
                        if ((_y == y) && (_x > x)) {
                            if (swatches[masters_and_autos[device][k]]->processed || (swatches[masters_and_autos[device][k]]->swatche_state == SwatchState::Master)) {
                                if (_x < right_x) {
                                    right_x = _x;
                                    right_swatch = masters_and_autos[device][k];
                                }
                            }
                            if (_x > max_x) {
                                max_x = _x;
                            }
                        }
                    }
                    if (right_x == 9999) { right_x = x; }
                    qDebug() << "   -> right         " << right_swatch;
                    qDebug() << "      right_x max_x " << right_x << " " << max_x;


                    // set auto between master -- left
                    if (left_swatch != QString("")) {
                        for (int k = 0; k < to_process_autos.length(); k++) {
                            int _x = swatches[to_process_autos[k]]->swatche_pos.first;
                            int _y = swatches[to_process_autos[k]]->swatche_pos.second;
                            if (!swatches[to_process_autos[k]]->processed && (_y == y) && (_x < x) && (_x > left_x)) {
//                                if (!swatches[to_process_autos[k]]->processed) {
                                    new_masters.append(to_process_autos[k]);
//                                }
                                swatches[to_process_autos[k]]->processed = true;
                                qreal h_m, s_m, v_m;
                                swatches[cur_master]->background.getHsvF(&h_m, &s_m, &v_m);
                                qreal h_l, s_l, v_l;
                                swatches[left_swatch]->background.getHsvF(&h_l, &s_l, &v_l);

                                float ratio = ((float)(left_x - _x)) / ((float)(left_x - x));

                                QColor col;
                                v_l = (v_l>v_m?v_l-1.0:v_l);
                                qreal v = v_l + ratio*(v_m - v_l);
                                v = (v>1?v-1.0:v);
                                v = (v<0?v+1.0:v);
                                qreal s = min(s_m,s_l) + ratio*(max(s_m,s_l) - min(s_m,s_l));
                                qreal h = min(h_m,h_l) + ratio*(max(h_m,h_l) - min(h_m,h_l));
                                col.setHsvF(h,s,v);
                                swatches[to_process_autos[k]]->setColorBackground(col,col);

                                qDebug() << "      set auto between master -- left";
                                qDebug() << "      setting   " << to_process_autos[k] << " " << _x;
                                qDebug() << "          ratio " << ratio;
                                qDebug() << "          hsv m " << h_m << " " << s_m << " " << v_m;
                                qDebug() << "          hsv l " << h_l << " " << s_l << " " << v_l;
                                qDebug() << "          hsv   " << h << " " << s << " " << v;
                            }
                        }
                    }

                    // set auto between master -- right
                    if (right_swatch != QString("")) {
                        for (int k = 0; k < to_process_autos.length(); k++) {
                            int _x = swatches[to_process_autos[k]]->swatche_pos.first;
                            int _y = swatches[to_process_autos[k]]->swatche_pos.second;
                            if (!swatches[to_process_autos[k]]->processed && (_y == y) && (_x > x) && (_x < right_x)) {
//                                if (!swatches[to_process_autos[k]]->processed) {
                                    new_masters.append(to_process_autos[k]);
//                                }
                                swatches[to_process_autos[k]]->processed = true;
                                qreal h_m, s_m, v_m;
                                swatches[cur_master]->background.getHsvF(&h_m, &s_m, &v_m);
                                qreal h_r, s_r, v_r;
                                swatches[right_swatch]->background.getHsvF(&h_r, &s_r, &v_r);

                                qDebug() << x << " -- " << _x << " -- " << right_x;
                                float ratio = ((float)(x - _x)) / ((float)(x - right_x));

                                QColor col;
                                v_r = (v_r<v_m?v_r+1.0:v_r);
                                qreal v = v_m + ratio*(v_r - v_m);
                                v = (v>1?v-1.0:v);
                                v = (v<0?v+1.0:v);
                                qreal s = min(s_m,s_r) + ratio*(max(s_m,s_r) - min(s_m,s_r));
                                qreal h = min(h_m,h_r) + ratio*(max(h_m,h_r) - min(h_m,h_r));
                                col.setHsvF(h,s,v);
                                swatches[to_process_autos[k]]->setColorBackground(col,col);

                                qDebug() << "      set auto between master -- right";
                                qDebug() << "      setting   " << to_process_autos[k] << " " << _x;
                                qDebug() << "          ratio " << ratio;
                                qDebug() << "          hsv m " << h_m << " " << s_m << " " << v_m;
                                qDebug() << "          hsv r " << h_r << " " << s_r << " " << v_r;
                                qDebug() << "          hsv   " << h << " " << s << " " << v;
                            }
                        }
                    }

                    // set auto no master -- left
                    if (left_swatch == QString("")) {
                        for (int k = 0; k < to_process_autos.length(); k++) {
                            int _x = swatches[to_process_autos[k]]->swatche_pos.first;
                            int _y = swatches[to_process_autos[k]]->swatche_pos.second;
                            if (!swatches[to_process_autos[k]]->processed && (_y == y) && (_x < x)) {
//                                if (!swatches[to_process_autos[k]]->processed) {
                                    new_masters.append(to_process_autos[k]);
//                                }
                                swatches[to_process_autos[k]]->processed = true;
                                qreal h_m, s_m, v_m;
                                swatches[cur_master]->background.getHsvF(&h_m, &s_m, &v_m);

                                float ratio = ((float)(min_x - _x)) / ((float)(min_x - x));

                                QColor col;
                                qreal v = ratio*v_m;
                                qreal s = s_m;
                                qreal h = h_m;
                                col.setHsvF(h,s,v);
                                swatches[to_process_autos[k]]->setColorBackground(col,col);

                                qDebug() << "      set auto no master -- left";
                                qDebug() << "      setting   " << to_process_autos[k] << " " << _x;
                                qDebug() << "          ratio " << ratio;
                                qDebug() << "          hsv m " << h_m << " " << s_m << " " << v_m;
                                qDebug() << "          hsv   " << h << " " << s << " " << v;
                            }
                        }
                    }

                    // set auto no master -- right
                    if (right_swatch == QString("")) {
                        for (int k = 0; k < to_process_autos.length(); k++) {
                            int _x = swatches[to_process_autos[k]]->swatche_pos.first;
                            int _y = swatches[to_process_autos[k]]->swatche_pos.second;
                            if (!swatches[to_process_autos[k]]->processed && (_y == y) && (_x > x)) {
//                                if (!swatches[to_process_autos[k]]->processed) {
                                    new_masters.append(to_process_autos[k]);
//                                }
                                swatches[to_process_autos[k]]->processed = true;
                                qreal h_m, s_m, v_m;
                                swatches[cur_master]->background.getHsvF(&h_m, &s_m, &v_m);

                                float ratio = ((float)(_x - max_x)) / ((float)(x - max_x));

                                QColor col;
                                qreal v = v_m + (1.0-ratio)*(1.0-v_m);
                                qreal s = s_m;
                                qreal h = h_m;
                                col.setHsvF(h,s,v);
                                swatches[to_process_autos[k]]->setColorBackground(col,col);

                                qDebug() << "      set auto no master -- right";
                                qDebug() << "      setting   " << to_process_autos[k] << " " << _x;
                                qDebug() << "          ratio " << ratio;
                                qDebug() << "          hsv m " << h_m << " " << s_m << " " << v_m;
                                qDebug() << "          hsv   " << h << " " << s << " " << v;
                            }
                        }
                    }
                }

                qDebug() << new_masters;
            }
        }
    }
    // -------------

    setSwatches();
}

void ColorScene::handle_action(QString action) {
    handle_action_scene(action);

    QList<QString> act_id = action.split(':');
    if (act_id.length() == 2) {
        if (act_id[0] == QString("master")) {
            if (swatches[act_id[1]]->swatche_state == SwatchState::Auto) {
                swatches[act_id[1]]->swatche_state = SwatchState::Master;
                swatches[act_id[1]]->setColorBackground(QColor("#99495C"), swatches[act_id[1]]->background);
            } else {
                swatches[act_id[1]]->swatche_state = SwatchState::Auto;
                swatches[act_id[1]]->setColorBackground(QColor("#2F3E4E"), swatches[act_id[1]]->background);
            }
            handle_configuration();
        } else {
            if (act_id[0] == QString("tap")) {
//                if (swatches[act_id[1]]->swatche_state == SwatchState::Master) {
//                    QColor rand_col;
//                    rand_col.setRed(rand() % 256);
//                    rand_col.setGreen(rand() % 256);
//                    rand_col.setBlue(rand() % 256);
//                    swatches[act_id[1]]->setColorBackground(QColor("#99495C"), rand_col);
//                }
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

    handle_configuration();
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
    QList<QString> swatches_id = swatches.keys();
    for (int k = 0; k < swatches_id.length(); k++) {
        if (swatches[swatches_id[k]]->swatche_visible && swatches[swatches_id[k]]->isActive()) {
            QRectF bbox(swatches[swatches_id[k]]->x(),
                        swatches[swatches_id[k]]->y(),
                        swatches[swatches_id[k]]->boundingRect().width(),
                        swatches[swatches_id[k]]->boundingRect().height());
            if (release) {
                swatches[swatches_id[k]]->setActive(false);
                if (bbox.contains(x,y)) {
                    int elapse_time = swatches[swatches_id[k]]->getTime();
                    //if (elapse_time < 500) {
                    //    emit action(swatches[swatches_id[k]]->getAction());
                    //} else {
                    //    emit action(swatches[swatches_id[k]]->getLongAction());
                    //}
                    if (elapse_time >= 300 && !swatches[swatches_id[k]]->colorCanChange(x,y)) {
                        if (!swatches[swatches_id[k]]->getColorChanged()) {
                            emit action(swatches[swatches_id[k]]->getLongAction());
                        } else {
                            //qDebug() << "conf color update";
                        }
                    }
                }
                handle_configuration();
                to_update.append(bbox);
                break;
            } else if (swatches[swatches_id[k]]->swatche_state == SwatchState::Master && swatches[swatches_id[k]]->colorCanChange(x,y)) {
                if (swatches[swatches_id[k]]->getColorChanged() || swatches[swatches_id[k]]->colorCanChange(x,y)) {
                    swatches[swatches_id[k]]->changeColor(x,y);
                    to_update.append(bbox);
                }
            }
            swatch_active = true;
        }
    }
    if (!swatch_active && !release) {
        QList<QString> swatches_id = swatches.keys();
        for (int k = 0; k < swatches_id.length(); k++) {
            QRectF bbox(swatches[swatches_id[k]]->x(),
                        swatches[swatches_id[k]]->y(),
                        swatches[swatches_id[k]]->boundingRect().width(),
                        swatches[swatches_id[k]]->boundingRect().height());
            if (bbox.contains(x,y)) {
                swatches[swatches_id[k]]->setActive(true);
                swatches[swatches_id[k]]->startTime();
                swatches[swatches_id[k]]->setInitialXY(x,y);
                swatches[swatches_id[k]]->setColorChanged(false);
                to_update.append(bbox);
                break;
            }
        }
    }
    return to_update;
}
