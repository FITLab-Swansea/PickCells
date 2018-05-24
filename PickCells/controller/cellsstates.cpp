#include "cellsstates.h"

bool orderSmartWatches(const QPair<int, int> &p1, const QPair<int, int> &p2) {
    return p1.first > p2.first;
}

CellsStates* CellsStates::_cs = NULL;

CellsStates *CellsStates::getInstance() {
     if(_cs == NULL) {
         _cs = new CellsStates();
         _cs->initialize();
     }
     return _cs;
}

CellsStates::CellsStates(QObject *parent) : QObject(parent) {
    _debug = false;
    // _debug_conf = 0;
    _debug_conf = 2;
}

void CellsStates::initialize() {

    // dummy for now
    _available_cells.clear();
//    for (int k = 0; k < 2*4*8; k++) {
//        Cell *c = new Cell();
//        c->initialize(k);
//        _available_cells.append(c);
//    }

    _cell_states = NULL;
    _last_app_screens = NULL;
    _app_screens = NULL;

    json_set = false;

    updateStates();
}

void CellsStates::updateStates() {
    _last_app_screens = _app_screens;
    resetCurrentStates();
    _cell_states = new PCDevices();
    _app_screens = new QList<AppScreen*>();

    _w_devices.clear();
    _h_devices.clear();
    _d_devices.clear();
    _nb_devices = 0;

    QString configuration = "";


    if (json_set) {
        // qDebug() << json_states << "\n";

        // Update available SmartWatches
        QJsonArray watches = json_states["watches"].toArray();
        QList<Cell*> _currrent_cells;
        for (int k = 0; k < watches.size(); k ++) {
            int watch_id = watches[k].toInt();
            qDebug() << "  -> watch " << k << " " << watch_id;

            bool available = false;
            for (int l = 0; l < _available_cells.size(); l++) {
                if ((_available_cells[l] != NULL) && (_available_cells[l]->getCellId() == watch_id)) {
                    available = true;
                    _currrent_cells.append(_available_cells[l]);
                    _available_cells[l] = NULL;
                }
            }
            if (!available) {
                Cell *c = new Cell();
                c->initialize(watch_id);
                _currrent_cells.append(c);
            }
        }
        for (int l = 0; l < _available_cells.size(); l++) {
            if (_available_cells[l] != NULL) {
                delete _available_cells[l];
            }
        }
        _available_cells = _currrent_cells;

        // Update devices (must be sorted from most number of tiles to lowest)
        qDebug() << "devices" << json_states["devices"];
        QJsonArray devices = json_states["devices"].toArray();

        QList<QPair<int,int> > order;
        for (int k = 0; k < devices.size(); k ++) {
            QJsonObject device = devices[k].toObject();
            QJsonObject connections = device["connections"].toObject();
            QPair<int, int> nb_and_place(connections.keys().size(),k);
            order.append(nb_and_place);
        }
        qSort(order.begin(), order.end(), orderSmartWatches);

        _nb_devices = 0;
        int cur_depth = 0;
        int x_offset = 0;
        for (int k = 0; k < order.size(); k ++) {
            _nb_devices++;
            int place = order[k].second;

            qDebug() << "  -> device " << place;
            QJsonObject device = devices[place].toObject();
            qDebug() << "      -> min_x " << device["min_x"].toInt() << "  max_x " << device["max_x"].toInt();
            qDebug() << "      -> min_y " << device["min_y"].toInt() << "  max_y " << device["max_y"].toInt();
            qDebug() << "      -> min_z " << device["min_z"].toInt() << "  max_z " << device["max_z"].toInt();


            _cell_states->append(PCDevice());
            int device_w = device["max_x"].toInt() - device["min_x"].toInt() + 1;
            int device_h = device["max_y"].toInt() - device["min_y"].toInt() + 1;
            int device_d = device["max_z"].toInt() - device["min_z"].toInt() + 1;
            _w_devices.append(device_w);
            _h_devices.append(device_h);
            _d_devices.append(device_d);

            for (int layer = 0; layer < device_d; layer++) {
                (*_cell_states)[k].append(PCLayer());
                for (int row = 0; row < device_h; row++) {
                    (*_cell_states)[k][layer].append(PCRow());
                    for (int col = 0; col < device_w; col++) {
                        (*_cell_states)[k][layer][row].append(NULL);
                    }
                }
            }

            int start_x = device["min_x"].toInt();
            int start_y = device["min_y"].toInt();
            int start_z = device["min_z"].toInt();
            qDebug() << "      -> connections";
            QJsonObject connections = device["connections"].toObject();
            connections.keys().size();
            foreach(const QString& key, connections.keys()) {
                QJsonObject con = connections[key].toObject();
                qDebug() << "          -> " << key.toInt();
                qDebug() << "             " << con;

                int x = con["x"].toInt() - start_x;
                int y = con["y"].toInt() - start_y;
                int z = con["z"].toInt() - start_z;

                for (int l = 0; l < _available_cells.size(); l++) {
                    if (_available_cells[l]->getCellId() == key.toInt()) {
                        _available_cells[l]->setSeen(false);
                        (*_cell_states)[k][z][y][x] = _available_cells[l];
                    }
                }
            }

            for (int row = 0; row < device_h; row++) {
                for (int col = 0; col < device_w; col++) {
                    for (int layer = device_d-1; layer >= 0; layer--) {
                        if ((*_cell_states)[k][layer][row][col] != NULL) {
                            (*_cell_states)[k][layer][row][col]->setSeen(true);
                        }
                    }
                }
            }

            x_offset; device_h; device_w; device_d;
            AppScreen *as = new AppScreen();
            as->initialize();
            as->setRect(x_offset,0,device_w,device_h);
            as->setTopLeft(_nb_devices-1,device_d-1,0,0);
            as->setDepth(cur_depth); cur_depth++;
            as->setHasContent(true);
            _app_screens->append(as);

            for (int row = 0; row < device_h; row++) {
                for (int col = 0; col < device_w; col++) {
                    int top_layer = -1; // 1 = top layer, 0 = other layer, -1 = no layer
                    int screen_layer;
                    for (int layer = device_d-1; layer >= 0; layer--) {
                        screen_layer = layer;
                        if ((*_cell_states)[k][layer][row][col] != NULL) {
                            if (layer == device_d-1) {
                                top_layer = 1;
                            } else {
                                top_layer = 0;
                            }
                            break;
                        }
                    }
                    if (top_layer != 1) {
                        as = new AppScreen();
                        as->initialize();
                        as->setRect(x_offset+col,row,1,1);
                        as->setTopLeft(_nb_devices-1,screen_layer,row,col);
                        as->setDepth(cur_depth); cur_depth++;
                        as->setHasContent(top_layer != -1);
                        _app_screens->append(as);
                    }
                }
            }
            x_offset += device_w + 1;
        }
    }







    // dummy for now
    if (_debug_conf == 0) {
        //
        // 1 1 1 1
        // 1 1 1 0     0
        // 1 1 1 1     0
        // 1 1 1 1     0
        // 1 1 1 1
        // 1 1 1 1
        // 1 1 1 1
        // 1 1   1
        //
        AppScreen *as = new AppScreen();
        as->initialize();
        as->setRect(0,0,4,8);
        as->setTopLeft(0,1,0,0);
        as->setDepth(0);
        as->setHasContent(true);
        _app_screens->append(as);
        as = new AppScreen();
        as->initialize();
        as->setRect(3,1,1,1);
        as->setTopLeft(0,0,1,3);
        as->setDepth(1);
        as->setHasContent(true);
        _app_screens->append(as);
        as = new AppScreen();
        as->initialize();
        as->setRect(2,7,1,1);
        as->setTopLeft(0,0,7,2);
        as->setDepth(2);
        as->setHasContent(false);
        _app_screens->append(as);
        as = new AppScreen();
        as->initialize();
        as->setRect(5,0,1,3);
        as->setTopLeft(1,0,0,0);
        as->setDepth(3);
        as->setHasContent(true);
        _app_screens->append(as);
        // assume _app_screens is sorted from bottom to top

        for (int device = 0; device < 1; device++ ) {
            _cell_states->append(PCDevice());
            _w_devices.append(0);
            _h_devices.append(0);
            _d_devices.append(0);

            for (int layer = 0; layer < 2; layer++) {
                _d_devices[device]++;
                (*_cell_states)[device].append(PCLayer());

                for (int row = 0; row < 8; row++) {
                    _h_devices[device] = max(_h_devices[device], row+1);
                    (*_cell_states)[device][layer].append(PCRow());

                    for (int col = 0; col < 4; col++) {

                        if ((layer == 1 && row == 1 && col == 3) ||
                            (layer == 0 && row == 7 && col == 2) ||
                            (layer == 1 && row == 7 && col == 2)) {
                            (*_cell_states)[device][layer][row].append(NULL);
                        } else {
                            if ((layer == 1) ||
                                (layer == 0 && row == 1 && col == 3)) {
                                _available_cells[layer*8*4 + row*4 + col]->setSeen(true);
                            }
                            _w_devices[device] = max(_w_devices[device], col+1);
                            (*_cell_states)[device][layer][row].append(_available_cells[layer*8*4 + row*4 + col]);
                        }
                    }
                }
            }
        }

        _cell_states->append(PCDevice());
        (*_cell_states)[1].append(PCLayer());
        (*_cell_states)[1][0].append(PCRow());
        (*_cell_states)[1][0].append(PCRow());
        (*_cell_states)[1][0].append(PCRow());

        _available_cells[1*8*4 + 1*4 + 3]->setSeen(true);
        _available_cells[0*8*4 + 7*4 + 2]->setSeen(true);
        _available_cells[1*8*4 + 7*4 + 2]->setSeen(true);
        (*_cell_states)[1][0][0].append(_available_cells[1*8*4 + 1*4 + 3]);
        (*_cell_states)[1][0][1].append(_available_cells[0*8*4 + 7*4 + 2]);
        (*_cell_states)[1][0][2].append(_available_cells[1*8*4 + 7*4 + 2]);

        _w_devices.append(1);
        _h_devices.append(3);
        _d_devices.append(1);

        _nb_devices = 2;
        configuration = "conf_test1";

    } else if (_debug_conf == 1) {
        //
        // 1 1 1 1
        // 1 2 2 1
        // 2 0 0 2
        // 2 0 0 2
        // 2 0 0 2
        // 1 0 0 1
        // 1 1 1 1
        // 1 1 1 1
        //
        AppScreen *as = new AppScreen();
        as->initialize();
        as->setRect(0,0,4,8);
        as->setTopLeft(0,1,0,0);
        as->setDepth(0);
        as->setHasContent(true);
        _app_screens->append(as);
        as = new AppScreen();
        as->initialize();
        as->setRect(1,2,2,4);
        as->setTopLeft(0,0,2,1);
        as->setDepth(1);
        as->setHasContent(true);
        _app_screens->append(as);
        as = new AppScreen();
        as->initialize();
        as->setRect(1,1,2,1);
        as->setTopLeft(0,2,1,1);
        as->setDepth(2);
        as->setHasContent(true);
        _app_screens->append(as);
        as = new AppScreen();
        as->initialize();
        as->setRect(0,2,1,3);
        as->setTopLeft(0,2,2,0);
        as->setDepth(2);
        as->setHasContent(true);
        _app_screens->append(as);
        as = new AppScreen();
        as->initialize();
        as->setRect(3,2,1,3);
        as->setTopLeft(0,2,2,3);
        as->setDepth(2);
        as->setHasContent(true);
        _app_screens->append(as);
        // assume _app_screens is sorted from bottom to top

        for (int device = 0; device < 1; device++ ) {
            _cell_states->append(PCDevice());
            _w_devices.append(0);
            _h_devices.append(0);
            _d_devices.append(0);

            for (int layer = 0; layer < 2; layer++) {
                _d_devices[device]++;
                (*_cell_states)[device].append(PCLayer());

                for (int row = 0; row < 8; row++) {
                    _h_devices[device] = max(_h_devices[device], row+1);
                    (*_cell_states)[device][layer].append(PCRow());

                    for (int col = 0; col < 4; col++) {

                        if ((layer == 1 && row == 2 && col == 1) ||
                            (layer == 1 && row == 3 && col == 1) ||
                            (layer == 1 && row == 4 && col == 1) ||
                            (layer == 1 && row == 5 && col == 1) ||
                            (layer == 1 && row == 2 && col == 2) ||
                            (layer == 1 && row == 3 && col == 2) ||
                            (layer == 1 && row == 4 && col == 2) ||
                            (layer == 1 && row == 5 && col == 2)) {
                            (*_cell_states)[device][layer][row].append(NULL);
                            _available_cells[(layer-1)*8*4 + row*4 + col]->setSeen(true);
                        } else {
                            if (layer == 1) {
                                _available_cells[layer*8*4 + row*4 + col]->setSeen(true);
                            }
                            _w_devices[device] = max(_w_devices[device], col+1);
                            (*_cell_states)[device][layer][row].append(_available_cells[layer*8*4 + row*4 + col]);
                        }
                    }
                }
            }
        }

        (*_cell_states)[0].append(PCLayer());
        (*_cell_states)[0][2].append(PCRow()); // NULL NULL NULL NULL 0
        (*_cell_states)[0][2].append(PCRow()); // NULL cell cell NULL 1
        (*_cell_states)[0][2].append(PCRow()); // cell NULL NULL cell 2
        (*_cell_states)[0][2].append(PCRow()); // cell NULL NULL cell 3
        (*_cell_states)[0][2].append(PCRow()); // cell NULL NULL cell 4
        (*_cell_states)[0][2].append(PCRow()); // NULL NULL NULL NULL 5
        (*_cell_states)[0][2].append(PCRow()); // NULL NULL NULL NULL 6
        (*_cell_states)[0][2].append(PCRow()); // NULL NULL NULL NULL 7

        for (int k = 0; k < 4; k ++) { (*_cell_states)[0][2][0].append(NULL); } // 0
        for (int k = 0; k < 4; k ++) { (*_cell_states)[0][2][5].append(NULL); } // 5
        for (int k = 0; k < 4; k ++) { (*_cell_states)[0][2][6].append(NULL); } // 6
        for (int k = 0; k < 4; k ++) { (*_cell_states)[0][2][7].append(NULL); } // 7


        _available_cells[1*8*4 + 1*4 + 1]->setSeen(false); _available_cells[1*8*4 + 1*4 + 2]->setSeen(false);
        _available_cells[1*8*4 + 2*4 + 0]->setSeen(false); _available_cells[1*8*4 + 2*4 + 3]->setSeen(false);
        _available_cells[1*8*4 + 3*4 + 0]->setSeen(false); _available_cells[1*8*4 + 3*4 + 3]->setSeen(false);
        _available_cells[1*8*4 + 4*4 + 0]->setSeen(false); _available_cells[1*8*4 + 4*4 + 3]->setSeen(false);
        (*_cell_states)[0][2][1].append(NULL);
        (*_cell_states)[0][2][1].append(_available_cells[1*8*4 + 5*4 + 1]); _available_cells[1*8*4 + 5*4 + 1]->setSeen(true);
        (*_cell_states)[0][2][1].append(_available_cells[1*8*4 + 5*4 + 2]); _available_cells[1*8*4 + 5*4 + 2]->setSeen(true);
        (*_cell_states)[0][2][1].append(NULL);
        (*_cell_states)[0][2][2].append(_available_cells[1*8*4 + 2*4 + 1]); _available_cells[1*8*4 + 2*4 + 1]->setSeen(true);
        (*_cell_states)[0][2][2].append(NULL);
        (*_cell_states)[0][2][2].append(NULL);
        (*_cell_states)[0][2][2].append(_available_cells[1*8*4 + 2*4 + 2]); _available_cells[1*8*4 + 2*4 + 2]->setSeen(true);
        (*_cell_states)[0][2][3].append(_available_cells[1*8*4 + 3*4 + 1]); _available_cells[1*8*4 + 3*4 + 1]->setSeen(true);
        (*_cell_states)[0][2][3].append(NULL);
        (*_cell_states)[0][2][3].append(NULL);
        (*_cell_states)[0][2][3].append(_available_cells[1*8*4 + 3*4 + 2]); _available_cells[1*8*4 + 3*4 + 2]->setSeen(true);
        (*_cell_states)[0][2][4].append(_available_cells[1*8*4 + 4*4 + 1]); _available_cells[1*8*4 + 4*4 + 1]->setSeen(true);
        (*_cell_states)[0][2][4].append(NULL);
        (*_cell_states)[0][2][4].append(NULL);
        (*_cell_states)[0][2][4].append(_available_cells[1*8*4 + 4*4 + 2]); _available_cells[1*8*4 + 4*4 + 2]->setSeen(true);

        _d_devices[0]++;

        _nb_devices = 1;
        configuration = "conf_test2";
    }

    emit new_configuration(configuration);
}

void CellsStates::setDebug(bool val) {
    if (val != _debug) {
        _debug = val;
        for (int k = 0; k < _available_cells.size(); k++) {
            _available_cells[k]->setDebug(_debug);
        }
    }
}

void CellsStates::setDebugConf(int ind) {
    _debug_conf = ind;
    updateStates();
}

void CellsStates::resetCurrentStates() {
    // all cells are kept, because no duplication so just reset the pointer
    if (_cell_states != NULL) {
        delete _cell_states;
    }
    _cell_states = NULL;

//    // reset the screen geometry
//    if (_app_screens != NULL) {
//        for (int k = 0; k < _app_screens->size(); k++) {
//            delete (*_app_screens)[k];
//        }
//        _app_screens->clear();
//        delete _app_screens;
//    }
//    _app_screens = NULL;
}
