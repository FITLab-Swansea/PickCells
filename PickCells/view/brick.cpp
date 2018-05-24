#include "brick.h"

Brick::Brick(QGraphicsItem *parent)
    : QGraphicsItem(parent) {
}

Brick::~Brick() {
}

void Brick::initialize(int brick_size, int iso_view) {
    _brick_size = brick_size;
    _iso_view = iso_view;

    offset = 1;
    path.moveTo(0,0);
    path.lineTo(brick_size,0);
    path.lineTo(brick_size+iso_view,iso_view);
    path.lineTo(brick_size+iso_view,brick_size+iso_view);
    path.lineTo(iso_view,brick_size+iso_view);
    path.lineTo(0,brick_size);
    path.lineTo(0,0);

    side1.moveTo(brick_size,0);
    side1.lineTo(brick_size+iso_view,iso_view);
    side1.lineTo(brick_size+iso_view,brick_size+iso_view);
    side1.lineTo(brick_size,brick_size);
    side1.lineTo(brick_size,0);

    side2.moveTo(brick_size,brick_size);
    side2.lineTo(brick_size+iso_view,brick_size+iso_view);
    side2.lineTo(iso_view,brick_size+iso_view);
    side2.lineTo(0,brick_size);
    side2.lineTo(brick_size,brick_size);

    _pixmap = QPixmap(100,100);
    _pixmap.fill(QColor("#2277FF"));
}

QRectF Brick::boundingRect() const {
    return QRectF(-offset,-offset,_brick_size+_iso_view+offset*2,_brick_size+_iso_view+offset*2);
}

void Brick::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setRenderHint(QPainter::HighQualityAntialiasing);

    // Sides
    painter->fillPath(side1,QBrush(QColor("#666666")));
    painter->fillPath(side2,QBrush(QColor("#AAAAAA")));
    painter->setPen(QPen(Qt::black,1));
    painter->drawPath(side1);
    painter->drawPath(side2);

    // Screen
    if (_is_seen) {
        painter->drawPixmap(0,0,_brick_size,_brick_size,_pixmap);
    } else {
        painter->fillRect(0,0,_brick_size,_brick_size,QBrush(QColor("#223388")));
    }
    painter->setPen(QPen(Qt::black,1));
    painter->drawRect(0,0,_brick_size,_brick_size);
}

QPainterPath Brick::shape() const {
    return path;
}
