#include "cell.h"

Cell::Cell(QObject *parent) : QObject(parent) {

}

void Cell::initialize(int id) {
    _cell_id = id;
    _seen = false;

    _avatar = NULL;

    _pixmap = QPixmap(200,200);
    _pixmap.fill(QColor("#2277FF"));

    _pixmap_debug = QPixmap(200,200);
    _pixmap_debug.fill(QColor("#00FFFFFF"));
    QPainter painter(&_pixmap_debug);
    painter.setFont(QFont("Verdana", 30, QFont::Bold));
    painter.setPen(QColor("#AA000000"));
    painter.drawText(QPoint(30,60),QString::number(_cell_id));

    _debug = false;
}

void Cell::setPixmap(QPixmap val) {
    _pixmap = val;

    if (_debug) {
        QPainter painter(&val);
        painter.drawPixmap(0,0,val.width(),val.height(),_pixmap_debug);
    }
    _avatar->setPixmap(val);
}

void Cell::setAvatarPixmap(QPixmap val) {
    if (_debug) {
        QPainter painter(&val);
        painter.drawPixmap(0,0,val.width(),val.height(),_pixmap_debug);
    }
    _avatar->setPixmap(val);
}

void Cell::setDebug(bool val) {
    _debug = val;
    setPixmap(_pixmap);
}
