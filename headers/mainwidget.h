#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "cameraholder.h"
#include <QWidget>

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
    void on_openFile_clicked();
    void on_takePhoto_clicked();

signals:
    void captureSignal();

private:
    Ui::MainWidget *ui;
    CameraHolder* cameraHolder;
    QWidget *pWid;
};

#endif // MAINWIDGET_H
