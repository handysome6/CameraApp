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

private slots:
    void on_xWindowReady(int x);
    void on_captureSucceed(QString);
    void on_openFile_clicked();
    void on_takePhoto_clicked();
    void on_measure_clicked();

signals:
    void captureSignal();

private:
    Ui::MainWidget *ui;
    CameraHolder* cameraHolder;
    QWidget *pWid;
    std::string img_path;
};

#endif // MAINWIDGET_H
