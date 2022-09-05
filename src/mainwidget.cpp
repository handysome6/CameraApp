#include "mainwidget.h"
#include "ui_mainwidget.h"

#include <QDebug>
#include <QThread>
#include <QWindow>


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
    // connect(this, SIGNAL(captureSignal()), worker, SLOT(captureFrame()));
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

void MainWidget::on_openFile_clicked()
{
    qDebug() << "width: " << pWid->width();
    qDebug() << "height: " << pWid->height();

}

void MainWidget::on_takePhoto_clicked()
{
    qDebug() << "Capturing photo...";
    // cameraHolder->sendRequest = false;
    // QThread::sleep(1);
    // emit captureSignal();
    cameraHolder->captureFrame();
}