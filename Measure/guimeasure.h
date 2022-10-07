#ifndef GUIMEASURE_H
#define GUIMEASURE_H

#include <QWidget>
#include <QMouseEvent>
#include <QPointF>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <string>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <nlohmann/json.hpp>

#include "mygraphicsview.h"
#include "crosswidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class GuiMeasure; }
QT_END_NAMESPACE

using namespace std;
using namespace cv;
using json = nlohmann::json;

class GuiMeasure : public QWidget
{
    Q_OBJECT

public:
    GuiMeasure(const string& filename,
             const char* cameraPath,
             QWidget *parent = nullptr);
    ~GuiMeasure();

protected:
    void showEvent( QShowEvent* event ) override;
    bool event(QEvent *event) override;
    void loadLeftImage();
    void loadRightImage();
    void autoMatch();
    void measureSegment();
    void showResult();

private slots:
    void widgetSizeMove();

    void on_point1_clicked();

    void on_point2_clicked();

    void on_finish_clicked();

private:
    Ui::GuiMeasure *ui;
    MyGraphicsView *leftView = nullptr;
    MyGraphicsView *rightView = nullptr;
    CrossWidget* crossWidget;
    bool SELECTING_LEFT = true;

    uint8_t* rgb_image;
    Mat leftImg, rightImg;
    Mat leftGray, rightGray;

    float Q[4][4] = {0};
    vector<Point2f> pointsLeft;
    vector<Point2f> pointsRight;
    qreal segment_len;

};
#endif // GUIMEASURE_H
