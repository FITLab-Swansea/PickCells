#include "swatch.h"
#include <math.h>

Swatch::Swatch(QGraphicsItem *parent, int w, int h)
    : QGraphicsItem(parent) {

    initializeSize(w, h);

    offset = 1;

    action = "";

    pen_background = QColor("#98BE61");
    background = QColor("#98BE61");

    cchecker_val = QString("XX.XXX");
    cchecker_state = QString("NONE");
    bg_background = QColor("black");
    txt_background = QColor("white");
}

Swatch::~Swatch() {
}

void Swatch::initializeSize(int w, int h) {
    width = w;
    height = h;

    path = QPainterPath();
    path.addRect(2,2,width-4,height-4);

    top_cchecker_path = QPainterPath();
    top_cchecker_path.addRect(0,height*0.2,width,height*0.3);

    bot_cchecker_path = QPainterPath();
    bot_cchecker_path.addRect(0,height*0.5,width,height*0.3);

    bg_rectangle = QRect(2,0,width-2,height*0.2);
    top_rectangle = QRect(0,height*0.2,width,height*0.3);
    bot_rectangle = QRect(0,height*0.5,width,height*0.3);
    txt_rectangle = QRect(0,height*0.8,width-2,height*0.2);
}

QRectF Swatch::boundingRect() const {
    return QRectF(-offset,-offset,width+offset*2,height+offset*2);
}

void Swatch::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setRenderHint(QPainter::HighQualityAntialiasing);

    if (swatche_state == SwatchState::CChecker) {
        painter->fillPath(path,QBrush(QColor("white")));
        painter->setPen(QPen(QColor("white"),4));
        painter->drawPath(path);

        QFont font = painter->font();
        font.setFamily("Verdana");

        font.setPixelSize(10);
        painter->setFont(font);

        if (cchecker_state == QString("NONE")) {
            painter->fillPath(top_cchecker_path,QBrush(QColor("black")));
            painter->setPen(QColor("white"));
            painter->drawText(top_rectangle, Qt::AlignHCenter | Qt::AlignVCenter, cchecker_val);
        } else if (cchecker_state == QString("YES")) {
            painter->fillPath(top_cchecker_path,QBrush(QColor("green")));
            painter->setPen(QColor("black"));
            painter->drawText(top_rectangle, Qt::AlignHCenter | Qt::AlignVCenter, cchecker_val);
        } if (cchecker_state == QString("SORTOF")) {
            painter->fillPath(top_cchecker_path,QBrush(QColor("orange")));
            painter->setPen(QColor("black"));
            painter->drawText(top_rectangle, Qt::AlignHCenter | Qt::AlignVCenter, cchecker_val);
        } else if (cchecker_state == QString("NO")) {
            painter->fillPath(top_cchecker_path,QBrush(QColor("red")));
            painter->setPen(QColor("white"));
            painter->drawText(top_rectangle, Qt::AlignHCenter | Qt::AlignVCenter, cchecker_val);
        }

        font.setPixelSize(8);
        painter->setFont(font);
        painter->fillPath(bot_cchecker_path,QBrush(bg_background));
        painter->setPen(txt_background);
        painter->drawText(bot_rectangle, Qt::AlignHCenter | Qt::AlignVCenter, QString("lorem ipsum"));

        painter->setPen(QColor("black"));
        painter->drawText(bg_rectangle, Qt::AlignLeft | Qt::AlignVCenter, QString("background"));
        painter->drawText(txt_rectangle, Qt::AlignRight | Qt::AlignVCenter, QString("text"));

    } else {
        painter->fillPath(path,QBrush(background));
        painter->setPen(QPen(pen_background,4));
        painter->drawPath(path);
    }
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

float getLuminance(float r, float g, float b){
    if (r <= 0.03928) {
        r = r / 12.92;
    } else {
        r = pow(((r+0.055)/1.055), 2.4);
    }

    if (g <= 0.03928) {
        g = g / 12.92;
    } else {
        g = pow(((g+0.055)/1.055), 2.4);
    }

    if (b <= 0.03928) {
        b = b / 12.92;
    } else {
        b = pow(((b+0.055)/1.055), 2.4);
    }

    return (0.2126 * r) + (0.7152 * g) + (0.0722 * b);
}

void Swatch::checkColor(QColor bg, QColor txt) {
    bg_background = bg;
    txt_background = txt;

    // perform math for WCAG1
    float brightnessThreshold = 125.0;
    float colorThreshold = 500.0;

    float bY = ((bg.redF() * 299.0) + (bg.greenF() * 587.0) + (bg.blueF() * 114.0)) * 255.0 / 1000.0;
    float fY = ((txt.redF() * 299.0) + (txt.greenF() * 587.0) + (txt.blueF() * 114.0)) * 255.0 / 1000.0;

    float brightnessDifference = abs(bY-fY);

    float colorDifference = (max (txt.redF(), bg.redF()) - min (txt.redF(), bg.redF())) +
                            (max (txt.greenF(), bg.greenF()) - min (txt.greenF(), bg.greenF())) +
                            (max (txt.blueF(), bg.blueF()) - min (txt.blueF(), bg.blueF()));
    colorDifference *= 255.0;



    float ratio = 1.0;
    float l1 = getLuminance(txt.redF(), txt.greenF(), txt.blueF());
    float l2 = getLuminance(bg.redF(), bg.greenF(), bg.blueF());

    if (l1 >= l2) {
        ratio = (l1 + .05) / (l2 + .05);
    } else {
        ratio = (l2 + .05) / (l1 + .05);
    }
    cchecker_val = QString::number(ratio,'g',2);

    if ((brightnessDifference >= brightnessThreshold) && (colorDifference >= colorThreshold))	{
        cchecker_state = QString("YES");
    } else if ((brightnessDifference >= brightnessThreshold) || (colorDifference >= colorThreshold)){
        cchecker_state = QString("SORTOF");
    } else {
        cchecker_state = QString("NO");
    }

    update();
}
