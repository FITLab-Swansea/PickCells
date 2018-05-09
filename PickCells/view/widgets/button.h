#ifndef BUTTON_H
#define BUTTON_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QPainter>
#include <QPixmap>

#include <QDebug>
#include <iostream>
using namespace std;

class Button : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    Button(QGraphicsItem * parent = 0, int w = 20, int h = 20);
    ~Button();
    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    virtual QPainterPath shape() const;

    void setBackground(bool val) { with_background = val; }

    bool isActive() { return active; }
    void setActive(bool val);

    void setIcon(QPixmap icn);

    QString getAction() { return action; }
    void setAction(QString val) { action = val; }

private:
    int width;
    int height;

    int offset;
    QPainterPath path;

    bool with_background;
    bool active;
    bool has_icon;
    QPixmap icon;
    int x_icn;
    int y_icn;
    int w_icn;
    int h_icn;

    QString action;
};

#endif // BUTTON_H
