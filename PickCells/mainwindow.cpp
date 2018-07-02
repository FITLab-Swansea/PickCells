#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->debug_button,SIGNAL(clicked(bool)),this,SLOT(handle_debug_button(bool)));
    connect(ui->appli,SIGNAL(currentIndexChanged(int)),this,SLOT(handle_debug_conf_button(int)));
    connect(ui->refresh_button,SIGNAL(clicked(bool)),this,SLOT(handle_refresh_button(bool)));
    connect(ui->image_button,SIGNAL(clicked(bool)),this,SLOT(handle_image_button(bool)));
    connect(ui->image_button,SIGNAL(clicked(bool)),this,SLOT(handle_image_button(bool)));

    connect(ui->app_view,SIGNAL(display_info(QString)),this,SLOT(handle_display_info(QString)));
    connect(ui->app_view,SIGNAL(put_into_clip_board(QString)),this,SLOT(handle_put_into_clip_board(QString)));

    CellsStates *states = CellsStates::getInstance();

    connect(states, SIGNAL(new_configuration(QString)), ui->app_view, SLOT(handle_new_configuration(QString)));
    connect(states, SIGNAL(new_configuration(QString)), ui->cells_view, SLOT(handle_new_configuration(QString)));
    connect(states, SIGNAL(visualUpdate(QString,QPixmap*)), this, SLOT(handleVisualUpdate(QString,QPixmap*)));


    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(socket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));

    QTimer::singleShot(1000, this, SLOT(bufferrizingImg()));
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
    } else if (event->key() == Qt::Key_T) {
        QJsonObject jsonObject;
        jsonObject.insert("IMEI","333");
        jsonObject.insert("x",60.74688720703125);
        jsonObject.insert("y",157.34439086914062);
        jsonObject.insert("id",0);
        jsonObject.insert("type",0);
        list.push_back(jsonObject);

        jsonObject.insert("x",80.63599395751953);
        jsonObject.insert("y",157.34439086914062);
        jsonObject.insert("type",2);
        list.push_back(jsonObject);

        jsonObject.insert("x",100.63599395751953);
        jsonObject.insert("y",157.34439086914062);
        list.push_back(jsonObject);

        jsonObject.insert("x",125.36514282226562);
        jsonObject.insert("y",154.35684204101562);
        list.push_back(jsonObject);

        jsonObject.insert("x",152.36514282226562);
        jsonObject.insert("y",154.35684204101562);
        list.push_back(jsonObject);

        jsonObject.insert("x",122.36514282226562);
        jsonObject.insert("y",154.35684204101562);
        list.push_back(jsonObject);

        jsonObject.insert("x",97.35906219482422);
        jsonObject.insert("y",153.97341918945312);
        list.push_back(jsonObject);

        jsonObject.insert("x",82.65560150146484);
        jsonObject.insert("y",153.36099243164062);
        jsonObject.insert("type",1);
        list.push_back(jsonObject);


        // NEW TOUCH
        jsonObject.insert("IMEI","888");
        jsonObject.insert("x",60.74688720703125);
        jsonObject.insert("y",157.34439086914062);
        jsonObject.insert("id",0);
        jsonObject.insert("type",0);
        list.push_back(jsonObject);

        jsonObject.insert("x",80.63599395751953);
        jsonObject.insert("y",157.34439086914062);
        jsonObject.insert("type",2);
        list.push_back(jsonObject);

        jsonObject.insert("x",100.63599395751953);
        jsonObject.insert("y",157.34439086914062);
        list.push_back(jsonObject);

        jsonObject.insert("x",125.36514282226562);
        jsonObject.insert("y",154.35684204101562);
        list.push_back(jsonObject);

        jsonObject.insert("x",152.36514282226562);
        jsonObject.insert("y",154.35684204101562);
        list.push_back(jsonObject);

        jsonObject.insert("x",122.36514282226562);
        jsonObject.insert("y",154.35684204101562);
        list.push_back(jsonObject);

        jsonObject.insert("x",97.35906219482422);
        jsonObject.insert("y",153.97341918945312);
        list.push_back(jsonObject);

        jsonObject.insert("x",82.65560150146484);
        jsonObject.insert("y",153.36099243164062);
        jsonObject.insert("type",1);
        list.push_back(jsonObject);

        // NEW TOUCH
        jsonObject.insert("IMEI","777");
        jsonObject.insert("x",10.0);
        jsonObject.insert("y",10.0);
        jsonObject.insert("id",0);
        jsonObject.insert("type",0);
        list.push_back(jsonObject);

        jsonObject.insert("x",120.0);
        jsonObject.insert("y",120.0);
        jsonObject.insert("type",2);
        list.push_back(jsonObject);

        jsonObject.insert("x",230.0);
        jsonObject.insert("y",230.0);
        jsonObject.insert("type",2);
        list.push_back(jsonObject);
        jsonObject.insert("x",230.0);
        jsonObject.insert("y",230.0);
        jsonObject.insert("type",1);
        list.push_back(jsonObject);

        // NEW TOUCH
        jsonObject.insert("IMEI","111");
        jsonObject.insert("x",10.0);
        jsonObject.insert("y",10.0);
        jsonObject.insert("id",0);
        jsonObject.insert("type",0);
        list.push_back(jsonObject);

        jsonObject.insert("x",120.0);
        jsonObject.insert("y",120.0);
        jsonObject.insert("type",2);
        list.push_back(jsonObject);

        jsonObject.insert("x",230.0);
        jsonObject.insert("y",230.0);
        jsonObject.insert("type",2);
        list.push_back(jsonObject);
        jsonObject.insert("x",230.0);
        jsonObject.insert("y",230.0);
        jsonObject.insert("type",1);
        list.push_back(jsonObject);

        QTimer::singleShot(250, this, SLOT(dummy_touch()));

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
//    ui->cells_view->sceneUpdate();
//    ui->app_view->sceneUpdate();
    ui->app_view->handle_general_update();
}

void MainWindow::handle_debug_conf_button(int ind) {
    CellsStates *states = CellsStates::getInstance();
    states->setApplication(ind);
    ui->app_view->handle_general_update();
}

void MainWindow::handle_refresh_button(bool checked) {
    ui->app_view->handle_general_update();
}

void MainWindow::handle_image_button(bool checked) {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), "/Users/alixgoguey/", tr("Image Files (*.png *.jpg *.bmp)"));
    if (fileName.size() > 5) {
        ui->cells_view->setImage(fileName);
    }
}

void MainWindow::handle_display_info(QString val) {
    ui->infos->setText(val);
}

void MainWindow::connected() {
    qDebug() << "Connected!";
}

void MainWindow::disconnected() {
    qDebug() << "Disconnected!";
}

void MainWindow::bytesWritten(qint64 bytes) {
    //qDebug() << "We wrote: " << bytes;
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
        } else if (msg.left(6) == "touch:") {
            msg = msg.right(msg.length()-6);

            QJsonDocument jsonResponse = QJsonDocument::fromJson(QString(msg).toUtf8());
            QJsonObject jsonObject = jsonResponse.object();
            ui->app_view->handle_new_touchframe(jsonObject);
        }
    } else {
        qDebug() << msg;
    }
}

void MainWindow::handleVisualUpdate(QString str, QPixmap * pix_ptr) {
    mutex.lock();
    buffer[str] = *pix_ptr;
    mutex.unlock();
}

void MainWindow::dummy_touch() {
    if (list.length() > 0) {
        ui->app_view->handle_new_touchframe(list[0]);
        list.pop_front();

        QTimer::singleShot(250, this, SLOT(dummy_touch()));
    }
}

void MainWindow::bufferrizingImg() {
    if (!socket->isOpen()) {
        QTimer::singleShot(1000, this, SLOT(bufferrizingImg()));
        return;
    }

    QList<QString> keys = buffer.keys();
    mutex.lock();
    for (int i = 0; i < keys.length(); i++) {
        QString str = keys[i];
        QPixmap pix_ptr = buffer[str];
        qDebug() << "Updating " << str;
        buffer.remove(str);

        int require_w = 64;//128;
        int require_h = 64;//128;
        qDebug() << pix_ptr;
        QPixmap pix = pix_ptr.scaled(require_w,require_h);
        // "img:charoftheid:whrgbrgbrgb...
        //int size_id = str.length() + 3 + 2;
        //QByteArray data(require_w*require_h*3 + 2 + size_id + 1,'\n');
        /*data[0] = 'i';
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
        }*/

        // "img:instop:charoftheid:instop:whrgbrgbrgb...:endstop:
        int size_id = str.length();
        int total_length = require_w*require_h*3 + 2 + size_id + 3 + 1 + 6 + 1 + 1 + 6 + 1 + + 1 + 7 + 1;
        QByteArray data(total_length,'\n');

        data[0] = 'i';
        data[1] = 'm';
        data[2] = 'g';
        data[3] = ':'; data[4] = 'i'; data[5] = 'n'; data[6] = 's'; data[7] = 't'; data[8] = 'o'; data[9] = 'p'; data[10] = ':';
        for (int k = 0; k < size_id; k++) {
            data[11+k] = str.toStdString().c_str()[k];
        }
        data[11+size_id] = ':'; data[12+size_id] = 'i'; data[13+size_id] = 'n'; data[14+size_id] = 's';
        data[15+size_id] = 't'; data[16+size_id] = 'o'; data[17+size_id] = 'p'; data[18+size_id] = ':';

        data[18+size_id+1+0] = (unsigned int) require_w;
        data[18+size_id+1+1] = (unsigned int) require_h;
        int k = 2;
        for (int pix_x = 0; pix_x < pix.width(); pix_x++) {
            for (int pix_y = 0; pix_y < pix.height(); pix_y++) {
                QRgb pixel = pix.toImage().pixel(pix_x,pix_y);
                data[18+size_id+1+k] = (unsigned int) ((pixel >> 16) & 0xff); k++;
                data[18+size_id+1+k] = (unsigned int) ((pixel >> 8) & 0xff); k++;
                data[18+size_id+1+k] = (unsigned int) (pixel & 0xff); k++;
            }
        }
        data[total_length-9] = ':'; data[total_length-8] = 'e'; data[total_length-7] = 'n'; data[total_length-6] = 'd';
        data[total_length-5] = 's'; data[total_length-4] = 't'; data[total_length-3] = 'o'; data[total_length-2] = 'p';
        data[total_length-1] = ':';
        socket->write(data);
    }
    mutex.unlock();
    QTimer::singleShot(200, this, SLOT(bufferrizingImg()));
}
