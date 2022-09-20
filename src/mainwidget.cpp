#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "measure.h"

#include <QDebug>
#include <QThread>
#include <QWindow>
#include <QFileDialog>

std::string camera_path = "/home/jetson/CameraApp/example/camera_model.json";


MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    this->showMaximized();
    

    QThread* thread = new QThread;
    cameraHolder = new CameraHolder();
    cameraHolder->moveToThread(thread);
    connect(thread, SIGNAL(started()), cameraHolder, SLOT(initAll()));
    connect(cameraHolder, SIGNAL(xWindowReady(int)), this, SLOT(on_xWindowReady(int)));
    connect(cameraHolder, SIGNAL(captureSucceed(QString)), this, SLOT(on_captureSucceed(QString)));
    // connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
    // connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    // connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();

}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::on_xWindowReady(int wid)
{
    qDebug() << ">>> Fetched x11 window ID: " << wid << " <<<";
    
    QWindow *pWin = QWindow::fromWinId(wid);
    pWin->setFlag(Qt::FramelessWindowHint);

    if (pWin != NULL)
    {
        pWid = QWidget::createWindowContainer(pWin, this, Qt::Widget);
        ui->horizontalLayout->replaceWidget(ui->placeholder, pWid);
        ui->horizontalLayout->update();
        pWid->setSizePolicy(QSizePolicy ::Expanding , QSizePolicy ::Expanding);
    }
    QThread::msleep(10);
}

void MainWidget::on_captureSucceed(QString img_path)
{
    qDebug() << "Capture Succeed!";
    this->img_path = img_path.toStdString();
}


void MainWidget::on_openFile_clicked()
{
    // qDebug() << "width: " << pWid->width();
    // qDebug() << "height: " << pWid->height();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select a sbs photo..."),
                                                "/home/jetson/qtprojects/CameraApp",
                                                tr("Images (*.png *.jpg)"));
    if (!fileName.isEmpty())
    {
        qDebug() << "Selected photo: " << fileName;
        std::string img_path = fileName.toStdString();
        // call External measure code... TODO
        measure(img_path, camera_path.c_str());
    }
}

void MainWidget::on_takePhoto_clicked()
{
    qDebug() << "Capturing photo...";
    // cameraHolder->sendRequest = false;
    // QThread::sleep(1);
    // emit captureSignal();
    cameraHolder->captureFrame();
}

void MainWidget::on_measure_clicked()
{
    qDebug() << "measure button clicked! ";
    if (!img_path.empty())
    {
        qDebug() << "Measure new photo: " << img_path.c_str();
        // call External measure code... TODO
        measure(img_path, camera_path.c_str());
    }
}
