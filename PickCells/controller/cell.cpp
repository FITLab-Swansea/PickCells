#include "cell.h"

Cell::Cell(QObject *parent) : QObject(parent) {

}

void Cell::initialize(QString id) {
    _cell_id = id;
    _seen = false;

    _avatar = NULL;

    _pixmap = QPixmap(200,200);
    _pixmap.fill(QColor("#2277FF"));

    _pixmap_debug = QPixmap(200,200);
    _pixmap_debug.fill(QColor("#00FFFFFF"));
    QPainter painter(&_pixmap_debug);
//    painter.setFont(QFont("Verdana", 20, QFont::Bold));
    QFont font = painter.font();
    font.setFamily("Verdana");
    font.setPixelSize(20);
    painter.setFont(font);
    painter.setPen(QColor("#AA000000"));
//    painter.drawText(QPoint(30,60),_cell_id);
    const QRect rectangle = QRect(30, 30, 200, 200);
    painter.drawText(rectangle, 0, _cell_id.right(3));


    _debug = false;
}

void Cell::setPixmap(QPixmap val) {
    _pixmap = val;

    if (_debug) {
        QPainter painter(&_pixmap);
        painter.drawPixmap(0,0,_pixmap.width(),_pixmap.height(),_pixmap_debug);
    }

    _avatar->setPixmap(_pixmap);
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
    emit visualUpdate(_cell_id,_avatar->getPixmap());
}

void Cell::sendNewVisual() {
    qDebug() << "updating cell " << _cell_id;
    emit visualUpdate(_cell_id,_avatar->getPixmap());
}
