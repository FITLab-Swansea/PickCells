#include "cellsview.h"

CellsView::CellsView(QWidget *parent) : QGraphicsView(parent) {
    setFrameStyle(0);

    _scene = new QGraphicsScene();
    setScene(_scene);
    _scene->setBackgroundBrush(QColor("#FFFFFF"));
}

CellsView::~CellsView() {

}

void CellsView::resizeEvent(QResizeEvent* e) {
    updateStates();
}

void CellsView::updateStates() {
    CellsStates *states = CellsStates::getInstance();

    for (int k = 0; k < states->_bricks_to_delete.size(); k++) {
        if (states->_bricks_to_delete[k] != NULL) {
            _scene->removeItem(states->_bricks_to_delete[k]);
            delete states->_bricks_to_delete[k];
        }
    }
    states->_bricks_to_delete.clear();

    for (int device = 0; device < states->getNbDevices(); device++ ) {
        for (int layer = 0; layer < states->getDeviceDepth(device); layer++) {
            for (int row = 0; row < states->getDeviceHeight(device); row++) {
                for (int col = 0; col < states->getDeviceWidth(device); col++) {
                    Cell *c = states->getCell(device, layer, row, col);
                    if (c != NULL) {
                        if (c->avatar() != NULL) {
                            _scene->removeItem(c->avatar());
                            delete c->avatar();
                            c->setAvatar(NULL);
                        }
                    }
                }
            }
        }
    }

    setSceneRect(0, 0, width(), height());
    _scene->setBackgroundBrush(QColor("#FFFFFF"));

    int btw_off = 10;
    int x_device_off = 10;
    int y_device_off = 10;

    int w_workspace = 0;
    int h_workspace = 0;
    for (int device = 0; device < states->getNbDevices(); device++ ) {
        w_workspace += states->getDeviceWidth(device) + 1;
        h_workspace = max(h_workspace,states->getDeviceHeight(device)+1);
    }

    if ((w_workspace > 0) && (h_workspace > 0)) {
        int brick_size = min(width()/w_workspace,height()/h_workspace);
        int layer_off = brick_size / 4;
        for (int device = 0; device < states->getNbDevices(); device++ ) {
            for (int layer = 0; layer < states->getDeviceDepth(device); layer++) {
                for (int row = 0; row < states->getDeviceHeight(device); row++) {
                    for (int col = 0; col < states->getDeviceWidth(device); col++) {
                        Cell *c = states->getCell(device, layer, row, col);
                        if (c != NULL) {
                            if (c->avatar() == NULL) {
                                c->setAvatar(new Brick(NULL));
                                c->avatar()->setSeen(c->getSeen());
                                c->setAvatarPixmap(c->getPixmap());
                                _scene->addItem(c->avatar());
                            }
                            c->avatar()->initialize(brick_size, layer_off);
                            c->avatar()->setPos(x_device_off + brick_size*col + layer_off*(states->getDeviceDepth(device)-1 - layer),
                                                y_device_off + brick_size*row + layer_off*(states->getDeviceDepth(device)-1 - layer));
                        }
                    }
                }
            }
            x_device_off += btw_off + brick_size*states->getDeviceWidth(device) + states->getDeviceDepth(device)*layer_off;
        }
    }

    _scene->update();
}

void CellsView::setImage(QString file_path) {
    CellsStates *states = CellsStates::getInstance();

    int id_largest_device = -1;
    int nb_tiles = 0;
    for (int device = 0; device < states->getNbDevices(); device++ ) {
        int nb = states->getDeviceHeight(device) * states->getDeviceWidth(device);
        if (nb_tiles < nb) {
            nb_tiles = nb;
            id_largest_device = device;
        }
    }

    QList<Cell*> to_update;
    if (id_largest_device > -1) {
        int device = id_largest_device;

        QPixmap image(file_path);
        int w_img = image.width();
        int h_img = image.height();
        int w_tile = w_img / states->getDeviceWidth(device);
        int h_tile = h_img / states->getDeviceHeight(device);

        for (int row = 0; row < states->getDeviceHeight(device); row++) {
            for (int col = 0; col < states->getDeviceWidth(device); col++) {
                int layer = states->getDeviceDepth(device) - 1;
                Cell *c = NULL;
                while (c == NULL && layer >= 0) {
                    c = states->getCell(device, layer, row, col);
                    layer--;
                }
                if (c != NULL) {
                    c->setPixmap(image.copy(col*w_tile, row*h_tile, w_tile, h_tile));
                    to_update.append(c);
                }
            }
        }
        _scene->update();
    }

    for (int k = 0; k < to_update.size(); k++) {
        to_update[k]->sendNewVisual();
    }
}

void CellsView::handle_new_configuration(QString configuration) {
    updateStates();
}
