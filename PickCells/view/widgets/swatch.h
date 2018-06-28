#ifndef SWATCH_H
#define SWATCH_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QPainter>
#include <QPixmap>
#include <QTime>

#include <QDebug>
#include <iostream>
using namespace std;

enum SwatchState {
    Auto       = 0,
    Master     = 1
};

enum ColorDimension {
    None       = 0,
    Hue        = 1,
    Saturation = 2,
    Value      = 3
};

enum Reach {
    Not        = 0,
    Horizontal = 1,
    Vertical   = 2,
    BothDir    = 3
};

class Swatch : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    Swatch(QGraphicsItem * parent = 0, int w = 20, int h = 20);
    ~Swatch();
    void initializeSize(int w, int h);
    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    virtual QPainterPath shape() const;

    bool isActive() { return active; }
    void setActive(bool val) { active = val; }

    void setColorBackground(QColor pen, QColor backgd);
    QColor getColorBackground() { return background; }

    QString getAction() { return action; }
    void setAction(QString val) { action = val; }

    QString getLongAction() { return long_action; }
    void setLongAction(QString val) { long_action = val; }

    int getTime() { return timer.elapsed(); }
    void startTime() { timer.restart(); }

    void setColorChanged(bool val) { color_changed = val; color_can_change = val; }
    bool getColorChanged() { return color_changed; }
    bool colorCanChange(int x, int y);
    void setInitialXY(int x, int y);
    int getDx(int x) { return x - _init_x; }
    int getDy(int y) { return y - _init_y; }

    void changeColor(int x, int y);

    bool swatche_visible;
    SwatchState swatche_state;

    bool color_to_be_set;
    bool processed;
    QPair<int, int> swatche_pos;

    QColor background;

    Reach reachable;

private:
    int width;
    int height;

    int offset;
    QPainterPath path;

    bool active;

    QColor pen_background;

    QString action;
    QString long_action;

    bool color_changed;
    QTime timer;
    int _init_x;
    int _init_y;
    QColor init_background;
    ColorDimension cur_dimension;

    bool color_can_change;
};

#endif // SWATCH_H
