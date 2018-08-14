#include "button.h"

// http://www.ios-blog.co.uk/wp-content/uploads/2014/05/HTPressableButtonColorScheme.png
// Blue
// #4C98D5
// #3E80B4

// Red
// #D75745
// #B34334

// Asphalt
// #37495C
// #2F3E4E

// Green
// #ACD176
// #98BE61


Button::Button(QGraphicsItem *parent, int w, int h)
    : QGraphicsItem(parent) {
    width = w;
    height = h;

    offset = 1;
    path.addRoundRect(0,0,width,height,2);

    with_background = true;
    active = false;
    has_icon = false;
    action = "";

    pen_background = QColor("#98BE61");
    background = QColor("#98BE61");
    active_background = QColor("#ACD176");
}

Button::~Button() {
}

QRectF Button::boundingRect() const {
    return QRectF(-offset,-offset,width+offset*2,height+offset*2);
}

void Button::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setRenderHint(QPainter::HighQualityAntialiasing);

    if (with_background) {
        if (!active) {
            painter->fillPath(path,QBrush(active_background));
        } else {
            painter->fillPath(path,QBrush(background));
        }

        if (has_icon) {
            painter->drawPixmap(x_icn, y_icn, w_icn, h_icn, icon);
        }

        painter->setPen(QPen(pen_background,1));
        painter->drawPath(path);
    } else {
        if (has_icon) {
            if (!active) {
                painter->setOpacity(1.0);
            } else {
                painter->setOpacity(0.5);
            }
            painter->drawPixmap(x_icn, y_icn, w_icn, h_icn, icon);
        }
    }
}

QPainterPath Button::shape() const {
    return path;
}

void Button::setColorBackground(QColor pen, QColor backgd, QColor active_backgd) {
    pen_background = pen;
    background = backgd;
    active_background = active_backgd;
}

void Button::setActive(bool val) {
    active = val;
    update();
}

void Button::setIcon(QPixmap icn) {
    icon = icn;
    has_icon = true;

    x_icn = 1;
    y_icn = 1;
    w_icn = width-2;
    h_icn = height-2;
}
