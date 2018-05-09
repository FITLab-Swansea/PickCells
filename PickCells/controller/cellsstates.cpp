#include "cellsstates.h"

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
    _debug_conf = 0;
}

void CellsStates::initialize() {

    // dummy for now
    _available_cells.clear();
    for (int k = 0; k < 2*4*8; k++) {
        Cell *c = new Cell();
        c->initialize(k);
        _available_cells.append(c);
    }

    _cell_states = NULL;
    _last_app_screens = NULL;
    _app_screens = NULL;
    updateStates();
}

void CellsStates::updateStates() {
    // dummy for now
    _last_app_screens = _app_screens;
    resetCurrentStates();
    _cell_states = new PCDevices();
    _app_screens = new QList<AppScreen*>();

    _w_devices.clear();
    _h_devices.clear();
    _d_devices.clear();

    QString configuration = "";

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
