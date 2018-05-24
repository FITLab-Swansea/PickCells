#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->debug_button,SIGNAL(clicked(bool)),this,SLOT(handle_debug_button(bool)));
    connect(ui->debug_conf,SIGNAL(currentIndexChanged(int)),this,SLOT(handle_debug_conf_button(int)));
    connect(ui->image_button,SIGNAL(clicked(bool)),this,SLOT(handle_image_button(bool)));

    CellsStates *states = CellsStates::getInstance();

    connect(states, SIGNAL(new_configuration(QString)), ui->app_view, SLOT(handle_new_configuration(QString)));
    connect(states, SIGNAL(new_configuration(QString)), ui->cells_view, SLOT(handle_new_configuration(QString)));


    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(socket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));

}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_Escape) {
        QApplication::quit();
    } else if (event->key() == Qt::Key_C) {
        if (socket->isOpen()) {
            socket->close();
        }

        qDebug() << "Connecting,..";
        socket->connectToHost("localhost", 9001);
        if(!socket->waitForConnected(1000)) {
            qDebug() << "Error: " << socket->errorString();
        }
    } else if (event->key() == Qt::Key_S) {
        if (socket->isOpen()) {
            QPixmap pix(":/imgs/settings.png");
            pix = pix.scaled(128,128);
            QImage img = pix.toImage();
            QByteArray data(128*128*3 + 2,'\0');
            data[0] = (unsigned int) 128;
            data[1] = (unsigned int) 128;
            int k = 2;
            for (int pix_x = 0; pix_x < pix.width(); pix_x++) {
                for (int pix_y = 0; pix_y < pix.height(); pix_y++) {
                    QRgb pixel = pix.toImage().pixel(pix_x,pix_y);
                    data[k] = (unsigned int) ((pixel >> 16) & 0xff); k++;
                    data[k] = (unsigned int) ((pixel >> 8) & 0xff); k++;
                    data[k] = (unsigned int) (pixel & 0xff); k++;
                }
            }
            socket->write(data);
        }
    }
}

void MainWindow::handle_debug_button(bool checked) {
    CellsStates *states = CellsStates::getInstance();
    states->setDebug(checked);
    ui->cells_view->sceneUpdate();
    ui->app_view->sceneUpdate();
}

void MainWindow::handle_debug_conf_button(int ind) {
    CellsStates *states = CellsStates::getInstance();
    states->setDebugConf(ind);
}

void MainWindow::handle_image_button(bool checked) {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), "/Users/alixgoguey/", tr("Image Files (*.png *.jpg *.bmp)"));
    if (fileName.size() > 5) {
        ui->cells_view->setImage(fileName);
    }
}

void MainWindow::connected() {
    qDebug() << "Connected!";
}

void MainWindow::disconnected() {
    qDebug() << "Disconnected!";
}

void MainWindow::bytesWritten(qint64 bytes) {
    qDebug() << "We wrote: " << bytes;
}

void MainWindow::readyRead() {
    qDebug() << "Reading...";
    QByteArray msg = socket->readAll();
    qDebug() << msg;

    if (msg.length()>5) {
        if (msg.left(5) == "conf:") {
            msg = msg.right(msg.length()-5);

            QJsonDocument jsonResponse = QJsonDocument::fromJson(QString(msg).toUtf8());
            QJsonObject jsonObject = jsonResponse.object();
            CellsStates *states = CellsStates::getInstance();
            states->setJsonStates(jsonObject);
            states->updateStates();
        }
    }
}
