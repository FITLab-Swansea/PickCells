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

private:
    int width;
    int height;

    int offset;
    QPainterPath path;

    bool active;

    QColor pen_background;
    QColor background;

    QString action;
    QString long_action;

    QTime timer;
};

#endif // SWATCH_H
