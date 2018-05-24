#ifndef BRICK_H
#define BRICK_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QPainter>
#include <QPixmap>

#include <QDebug>
#include <iostream>
using namespace std;

class Brick : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    Brick(QGraphicsItem * parent = 0);
    ~Brick();

    void initialize(int brick_size = 20, int iso_view = 5);
    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    virtual QPainterPath shape() const;

    void setSeen(bool val) { _is_seen = val; }

    void setPixmap(QPixmap val) { _pixmap = val; }

private:
    int _brick_size;
    int _iso_view;
    bool _is_seen;
    QPixmap _pixmap;

    int offset;
    QPainterPath path;
    QPainterPath side1;
    QPainterPath side2;
};

#endif // BRICK_H
