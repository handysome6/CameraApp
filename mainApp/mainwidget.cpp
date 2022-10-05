#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "i2cworker.h"
#include "imuworker.h"
#include "guimeasure.h"

#include <QDebug>
#include <QThread>
#include <QWindow>
#include <QFileDialog>
#include <QString>

const char* camera_path = "/home/jetson/CameraApp/example/camera_model.json";
std::string filename = "/home/jetson/CameraApp/build/sbs_100316001664784025.jpg";


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

    //startI2CWorker();
    //startIMUWorker();
}

MainWidget::~MainWidget()
{
    delete ui;
}


void MainWidget::startI2CWorker()
{
    QThread* thread = new QThread;
    I2CWorker* i2cWorker = new I2CWorker();
    i2cWorker->moveToThread(thread);
    connect(thread, SIGNAL(started()), i2cWorker, SLOT(readTemp()));
    connect(i2cWorker, SIGNAL(tempSignal(float)), this, SLOT(on_tempUpdated(float)));
    thread->start();
}

void MainWidget::startIMUWorker()
{
    QThread* thread = new QThread;
    IMUWorker* imuWorker = new IMUWorker();
    imuWorker->moveToThread(thread);
    connect(thread, SIGNAL(started()), imuWorker, SLOT(readXYZ()));
    connect(
        imuWorker, SIGNAL(xyzSignal(float, float, float)), 
        this, SLOT(on_xyzUpdated(float, float, float))
    );
    thread->start();
}

void MainWidget::on_tempUpdated(float temp)
{
    tempInfo = QString("T: %1 %2 \n").arg(
        QString::number(temp), 
        QString::fromUtf8("°C")
    );
    ui->infoArea->setText(tempInfo + xyzInfo);
}

void MainWidget::on_xyzUpdated(float x, float y, float z)
{
    xyzInfo = QString("X: %1\nY: %2\nZ: %3\n").arg(
        QString::number(x), 
        QString::number(y), 
        QString::number(z)
    );
    ui->infoArea->setText(tempInfo + xyzInfo);
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
                                                "/home/jetson/CameraApp",
                                                tr("Images (*.png *.jpg)"));
    if (!fileName.isEmpty())
    {
        qDebug() << "Selected photo: " << fileName;
        std::string img_path = fileName.toStdString();
        // call External measure code... TODO
        GuiMeasure *meausre = new GuiMeasure(img_path, camera_path);
        meausre->show();
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
        GuiMeasure *meausre = new GuiMeasure(img_path, camera_path);
        meausre->show();
    }
}
