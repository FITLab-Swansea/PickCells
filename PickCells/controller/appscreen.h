#ifndef APPSCREEN_H
#define APPSCREEN_H

#include <QObject>
#include <QGraphicsRectItem>

#include <QDebug>
#include <iostream>
using namespace std;

class AppScreen : public QObject
{
    Q_OBJECT
public:
    explicit AppScreen(QObject *parent = nullptr);
    void initialize();

    void setRect(int x, int y, int w, int h) { _x = x; _y = y; _width = w; _height = h; }
    int getX() { return _x; }
    int getY() { return _y; }
    int getW() { return _width; }
    int getH() { return _height; }

    void setTopLeft(int device, int layer, int row, int col) { _device = device; _layer = layer; _row = row; _col = col; }
    int getTLDevice() { return _device; }
    int getTLLayer() { return _layer; }
    int getTLRow() { return _row; }
    int getTLCol() { return _col; }

    void setDepth(int val) { _depth = val; }
    int getDepth() { return _depth; }

    void setHasContent(bool val) { _has_content = val; }
    bool hasContent() { return _has_content; }

    QGraphicsRectItem *avatar;
signals:

public slots:

private:
    int _x;
    int _y;
    int _width;
    int _height;
    int _depth;
    bool _has_content;

    int _device;
    int _layer;
    int _row;
    int _col;
};

#endif // APPSCREEN_H
