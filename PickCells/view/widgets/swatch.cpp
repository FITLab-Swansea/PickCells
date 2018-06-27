#include "swatch.h"

Swatch::Swatch(QGraphicsItem *parent, int w, int h)
    : QGraphicsItem(parent) {

    initializeSize(w, h);

    offset = 1;

    action = "";

    pen_background = QColor("#98BE61");
    background = QColor("#98BE61");
}

Swatch::~Swatch() {
}

void Swatch::initializeSize(int w, int h) {
    width = w;
    height = h;

    path = QPainterPath();
    path.addRect(2,2,width-4,height-4);
}

QRectF Swatch::boundingRect() const {
    return QRectF(-offset,-offset,width+offset*2,height+offset*2);
}

void Swatch::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setRenderHint(QPainter::HighQualityAntialiasing);

    painter->fillPath(path,QBrush(background));

    painter->setPen(QPen(pen_background,4));
    painter->drawPath(path);
}

QPainterPath Swatch::shape() const {
    return path;
}

void Swatch::setColorBackground(QColor pen, QColor backgd) {
    pen_background = pen;
    background = backgd;
    update();
}
