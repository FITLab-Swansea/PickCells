#ifndef CELL_H
#define CELL_H

#include <QObject>
#include <QPixmap>

#include "../view/brick.h"

#include <QDebug>
#include <iostream>
using namespace std;

class Cell : public QObject
{
    Q_OBJECT
public:
    explicit Cell(QObject *parent = nullptr);
    void initialize(QString id);

    QString getCellId() { return _cell_id; }

    bool getSeen() { return _seen; }
    void setSeen(bool val) { _seen = val; }

    QPixmap getPixmap() { return _pixmap; }
    void setPixmap(QPixmap val);

    void setAvatar(Brick *avatar) { _avatar = avatar; }
    Brick* avatar() { return _avatar; }

    void setAvatarPixmap(QPixmap val);

    void setDebug(bool val);

signals:

public slots:

private:
    QString _cell_id;

    bool _seen;
    QPixmap _pixmap;

    bool _debug;
    QPixmap _pixmap_debug;

    Brick *_avatar;
};

#endif // CELL_H
