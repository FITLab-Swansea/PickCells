#include "swatch.h"
#include <math.h>

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

bool Swatch::colorCanChange(int x, int y) {
    float ratio_dx = (x - _init_x) / ((float) width);
    float ratio_dy = (y - _init_y) / ((float) height);
    //qDebug() << abs(ratio_dx) << "    " << abs(ratio_dy);
    color_can_change = color_can_change || (abs(ratio_dx) > 0.05) ||  (abs(ratio_dy) > 0.05);
    return color_can_change;
}

void Swatch::setInitialXY(int x, int y) {
    _init_x = x;
    _init_y = y;
    init_background = background;
    cur_dimension = ColorDimension::None;
}

void Swatch::changeColor(int x, int y) {
//    float ratio_x = (x - pos().x()) / ((float) width);
    float ratio_y = (y - pos().y()) / ((float) height);
    float ratio_dx = (x - _init_x) / ((float) width);
    float ratio_dy = (y - _init_y) / ((float) height);

    if (cur_dimension == ColorDimension::None) {
        if (abs(ratio_dx) < abs(ratio_dy)) {
            cur_dimension = ColorDimension::Hue;
        } else {
            if (ratio_y < 0.5) {
                cur_dimension = ColorDimension::Saturation;
            } else {
                cur_dimension = ColorDimension::Value;
            }
        }
    }

    QColor col = init_background;
    qreal h, s, v;
    col.getHsvF(&h,&s,&v);
    if (cur_dimension == ColorDimension::Hue) {
        h += ratio_dy;
        while (h > 1) {
            h -= 1;
        }
        while (h < 0) {
            h += 1;
        }
    } else if (cur_dimension == ColorDimension::Saturation) {
        s += ratio_dx;
        s = (s>1?1:s);
        s = (s<0?0:s);
    } else if (cur_dimension == ColorDimension::Value) {
        v += ratio_dx;
        v = (v>1?1:v);
        v = (v<0?0:v);
    }
    col.setHsvF(h,s,v);

    background = col;
    color_changed = true;
    update();
}
