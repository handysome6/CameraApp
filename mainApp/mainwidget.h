#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "cameraholder.h"
#include <QWidget>
#include <string>
#include <QString>

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

protected:
    void startI2CWorker();
    void startIMUWorker();

private slots:
    void on_xWindowReady(int x);
    void on_captureSucceed(QString);
    void on_openFile_clicked();
    void on_takePhoto_clicked();
    void on_measure_clicked();
    void on_tempUpdated(float);
    void on_xyzUpdated(float, float, float);

signals:
    void captureSignal();

private:
    Ui::MainWidget *ui;
    CameraHolder* cameraHolder;
    QWidget *pWid;
    std::string img_path;
    QString tempInfo;
    QString xyzInfo;
};

#endif // MAINWIDGET_H
