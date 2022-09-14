#include "mainwidget.h"
#include "ui_mainwidget.h"

#include <QDebug>
#include <QThread>
#include <QWindow>
#include <QFileDialog>


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
    connect(cameraHolder, SIGNAL(captureSucceed(std::string)), this, SLOT(captureFrame()));
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

void MainWidget::on_captureSucceed(std::string img_path)
{
    this->img_path = img_path;
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
        // call External measure code... TODO
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
    if (img_path)
    {
        qDebug() << "Measure new photo: " << img_path;
        // call External measure code... TODO
    }
}
