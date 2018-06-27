#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->debug_button,SIGNAL(clicked(bool)),this,SLOT(handle_debug_button(bool)));
    connect(ui->appli,SIGNAL(currentIndexChanged(int)),this,SLOT(handle_debug_conf_button(int)));
    connect(ui->image_button,SIGNAL(clicked(bool)),this,SLOT(handle_image_button(bool)));

    CellsStates *states = CellsStates::getInstance();

    connect(states, SIGNAL(new_configuration(QString)), ui->app_view, SLOT(handle_new_configuration(QString)));
    connect(states, SIGNAL(new_configuration(QString)), ui->cells_view, SLOT(handle_new_configuration(QString)));
    connect(states, SIGNAL(visualUpdate(QString,QPixmap*)), this, SLOT(handleVisualUpdate(QString,QPixmap*)));


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
//            QPixmap pix(":/imgs/settings.png");
//            pix = pix.scaled(128,128);
//            QByteArray data(128*128*3 + 2 + 1,'\n');
//            data[0] = (unsigned int) 128;
//            data[1] = (unsigned int) 128;
//            int k = 2;
//            for (int pix_x = 0; pix_x < pix.width(); pix_x++) {
//                for (int pix_y = 0; pix_y < pix.height(); pix_y++) {
//                    QRgb pixel = pix.toImage().pixel(pix_x,pix_y);
//                    data[k] = (unsigned int) ((pixel >> 16) & 0xff); k++;
//                    data[k] = (unsigned int) ((pixel >> 8) & 0xff); k++;
//                    data[k] = (unsigned int) (pixel & 0xff); k++;
//                }
//            }
//            socket->write(data);
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
    states->setApplication(ind);
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

    if (msg.length()>5) {
        if (msg.left(5) == "conf:") {
            msg = msg.right(msg.length()-5);

            QJsonDocument jsonResponse = QJsonDocument::fromJson(QString(msg).toUtf8());
            QJsonObject jsonObject = jsonResponse.object();
            CellsStates *states = CellsStates::getInstance();
            states->setJsonStates(jsonObject);
            states->updateStates();
        }
    } else {
        qDebug() << msg;
    }
}

void MainWindow::handleVisualUpdate(QString str, QPixmap * pix_ptr) {
    if (socket->isOpen()) {
        int require_w = 64;//128;
        int require_h = 64;//128;
        QPixmap pix = pix_ptr->scaled(require_w,require_h);
        int size_id = str.length() + 3 + 2;
        // "img:charoftheid:whrgbrgbrgb...
        QByteArray data(require_w*require_h*3 + 2 + size_id + 1,'\n');

        data[0] = 'i';
        data[1] = 'm';
        data[2] = 'g';
        data[3] = ':';
        for (int k = 0; k < str.length(); k++) {
            data[4+k] = str.toStdString().c_str()[k];
        }
        data[size_id-1] = ':';

        data[size_id+0] = (unsigned int) require_w;
        data[size_id+1] = (unsigned int) require_h;
        int k = 2;
        for (int pix_x = 0; pix_x < pix.width(); pix_x++) {
            for (int pix_y = 0; pix_y < pix.height(); pix_y++) {
                QRgb pixel = pix.toImage().pixel(pix_x,pix_y);
                data[size_id+k] = (unsigned int) ((pixel >> 16) & 0xff); k++;
                data[size_id+k] = (unsigned int) ((pixel >> 8) & 0xff); k++;
                data[size_id+k] = (unsigned int) (pixel & 0xff); k++;
            }
        }
        socket->write(data);
    }
}
