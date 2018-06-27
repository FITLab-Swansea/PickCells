#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QFileDialog>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QRgb>

#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "controller/cellsstates.h"

#include <QDebug>
#include <iostream>
using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void keyPressEvent(QKeyEvent *event);

public slots:
    void handle_debug_button(bool checked);
    void handle_debug_conf_button(int ind);
    void handle_image_button(bool checked);

    void connected();
    void disconnected();
    void bytesWritten(qint64 bytes);
    void readyRead();

    void handleVisualUpdate(QString str, QPixmap * pix_ptr);

    void dummy_touch();

private:
    Ui::MainWindow *ui;

    QTcpSocket *socket;

    QList<QJsonObject> list;
};

#endif // MAINWINDOW_H
