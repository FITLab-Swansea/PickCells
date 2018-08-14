#ifndef CELLSSTATES_H
#define CELLSSTATES_H

#include <QObject>
#include <QList>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "cell.h"
#include "appscreen.h"

#include <QDebug>
#include <iostream>
using namespace std;

typedef QList<QList<QList<QList<Cell*> > > > PCDevices; // devices, layers (from bottom to top), rows, columns
typedef QList<QList<QList<Cell*>  > > PCDevice;
typedef QList<QList<Cell*> > PCLayer;
typedef QList<Cell*> PCRow;

class CellsStates : public QObject
{
    Q_OBJECT
private:
    explicit CellsStates(QObject *parent = nullptr);

public:
    static CellsStates* getInstance();
    void initialize();

    void setJsonStates(QJsonObject jsonObject) { json_set = true; json_states = jsonObject; }
    void updateStates();

    int getNbDevices() { return _nb_devices; }
    int getDeviceWidth(int id) { return _w_devices[id]; }
    int getDeviceHeight(int id) { return _h_devices[id]; }
    int getDeviceDepth(int id) { return _d_devices[id]; }
    Cell* getCell(int device, int layer, int row, int col) { return (*_cell_states)[device][layer][row][col]; }

    QList<AppScreen*>* getLastAppSreens() { return _last_app_screens; }
    void resetLastAppSreens() { _last_app_screens = NULL; }
    QList<AppScreen*>* getAppSreens() { return _app_screens; }

    void setDebug(bool val);
    bool getDebug() { return _debug; }
    void setApplication(int ind);

    bool getCellTopLeft(QString id, int *x, int *y);

    QList<Brick*> _bricks_to_delete;

    void setKeyWord(QString keyword) { kw = keyword; }
    QString getKeyWord() { return kw; }

signals:
    void new_configuration(QString);
    void visualUpdate(QString,QPixmap*);

public slots:
    void relayVisualUpdate(QString str, QPixmap*pix);

private:
    static CellsStates *_cs;

    void resetCurrentStates();

    bool _debug;
    int _appli;

    QList<Cell*> _available_cells;

    //QList<QList<QList<QList<Cell*> > > >  _cell_states; // entities, layers (from bottom to top), rows, columns
    PCDevices *_cell_states;
    int _nb_devices;
    QList<int> _w_devices;
    QList<int> _h_devices;
    QList<int> _d_devices;


    QList<AppScreen*> *_last_app_screens;
    QList<AppScreen*> *_app_screens;

    bool json_set;
    QJsonObject json_states;

    QString kw;
};

#endif // CELLSSTATES_H
