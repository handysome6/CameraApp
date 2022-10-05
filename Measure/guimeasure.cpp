#include "guimeasure.h"
#include "ui_guimeasure.h"
#include "crosswidget.h"
#include "stereorectify.h"
#include "automatcher.h"

#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QTransform>
#include <QDebug>
#include <QTimer>
#include <fstream>
#include <QThread>
#include <Qt>

GuiMeasure::GuiMeasure(const string& filename, const char* cameraPath, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GuiMeasure)
{
    ui->setupUi(this);
    ui->finish->setDisabled(true);

    /* load camera model*/
    ifstream f(cameraPath);
    json cameraModel = json::parse(f);
    qDebug() << "Good";

    /* load image */
    Mat imgLR = imread(filename);
    StereoRectify rectifier = StereoRectify(cameraModel, imgLR, leftImg, rightImg);
    qDebug() << "Image rectifiy finished";

    /* retrive Q for measure */
    for(int i = 0; i< 4; i++)
        for(int j = 0; j < 4; j++)
        {
            Q[i][j] = rectifier.Q.at<double>(i,j);;
            Q[i][j] = (float) Q[i][j] ;
        }

    /* setup view */
    leftView = new MyGraphicsView();
    rightView = new MyGraphicsView();
    ui->gridLayout->replaceWidget(ui->widget, leftView);

    showMaximized();

//    QTransform matrix = QTransform();
//    matrix.scale(0.5, 0.5);
//    view->setTransform(matrix);
}

void GuiMeasure::showEvent( QShowEvent* event )
{
    event->accept();

    /* initialize UI */
    loadLeftImage();
    loadRightImage();
    on_point1_clicked();
    setWindowTitle("Ruler Measure - LEFT");

    /* fit full photo into view */
    QTimer::singleShot(50, leftView, &MyGraphicsView::previewMode);

    /* create the cross widget*/
    crossWidget = new CrossWidget(this);
    ui->gridLayout->addWidget(crossWidget, 0,0);
    QTimer::singleShot(100, this, SLOT(widgetSizeMove()));
    crossWidget->lower();

    /* slots control crossWidget */
    connect(leftView, &MyGraphicsView::showCrossWidget,
            [this](){ this->crossWidget->raise(); });
    connect(leftView, &MyGraphicsView::hideCrossWidget,
            [this](){ this->crossWidget->lower(); });
    connect(rightView, &MyGraphicsView::showCrossWidget,
            [this](){ this->crossWidget->raise(); });
    connect(rightView, &MyGraphicsView::hideCrossWidget,
            [this](){ this->crossWidget->lower(); });

    /* slots control finish */
    connect(leftView, &MyGraphicsView::finishPicking,
            [this](){ this->ui->finish->setDisabled(false); });
    connect(rightView, &MyGraphicsView::finishPicking,
            [this](){ this->ui->finish->setDisabled(false); });

}

void GuiMeasure::loadLeftImage()
{
    /* load the left image */
    Mat rgbMat;
    cvtColor (leftImg,rgbMat,COLOR_BGR2RGB);
    QImage qImage((const uchar*)rgbMat.data,
                  rgbMat.cols, rgbMat.rows, rgbMat.cols*(rgbMat.channels ()),
                  QImage::Format_RGB888);
    QPixmap pixmap = QPixmap::fromImage (qImage);
    QGraphicsPixmapItem* item = new QGraphicsPixmapItem(pixmap);

    auto leftScene = new QGraphicsScene();
    leftScene->addItem(item);
    leftView->setScene(leftScene);
}
void GuiMeasure::loadRightImage()
{
    /* load the right image */
    Mat rgbMat;
    cvtColor (rightImg,rgbMat,COLOR_BGR2RGB);
    QImage qImage((const uchar*)rgbMat.data,
                  rgbMat.cols, rgbMat.rows, rgbMat.cols*(rgbMat.channels ()),
                  QImage::Format_RGB888);
    QPixmap pixmap = QPixmap::fromImage (qImage);
    QGraphicsPixmapItem* item = new QGraphicsPixmapItem(pixmap);

    auto rightScene = new QGraphicsScene();
    rightScene->addItem(item);
    rightView->setScene(rightScene);
}

void GuiMeasure::widgetSizeMove()
{
    QPoint centrePos;
    if (SELECTING_LEFT)
        centrePos = leftView->viewport()->mapTo(this, leftView->viewport()->rect().center());
    else
        centrePos = rightView->viewport()->mapTo(this, rightView->viewport()->rect().center());

    crossWidget->move(centrePos - crossWidget->rect().center());
}

bool GuiMeasure::event(QEvent *event)
{
    switch (event->type())
    {
    case QEvent::Resize:
    case QEvent::Move:
        QTimer::singleShot(10, this, SLOT(widgetSizeMove()));
        break;
    default:
        break;
    }

    return QWidget::event(event);
}

void GuiMeasure::on_point1_clicked()
{
    ui->point1->setChecked(true);
    ui->point2->setChecked(false);
    if (SELECTING_LEFT)
    {
        leftView->previewMode();
        leftView->SELECTING_POINT = 1;
    }else
    {
        rightView->previewMode();
        rightView->SELECTING_POINT = 1;
    }
    qDebug() << "selecting Point 1...";
}

void GuiMeasure::on_point2_clicked()
{
    ui->point2->setChecked(true);
    ui->point1->setChecked(false);
    if (SELECTING_LEFT)
    {
        leftView->previewMode();
        leftView->SELECTING_POINT = 2;
    }else
    {
        rightView->previewMode();
        rightView->SELECTING_POINT = 2;
    }
    qDebug() << "selecting Point 2...";
}

void GuiMeasure::on_finish_clicked()
{
    if (SELECTING_LEFT)
    {
        if (leftView->point1.isNull() || leftView->point2.isNull())
            qDebug() << "not enough point";
        else
        {
            /* take to next scene */
            ui->finish->setDisabled(true);
            QThread::msleep(100);
            qDebug() << "start autoMatcher..." << leftView->point1 << leftView->point2;
            autoMatch();

            SELECTING_LEFT = false;
            ui->gridLayout->replaceWidget(leftView, rightView);
            setWindowTitle("Ruler Measure - RIGHT");
            QTimer::singleShot(50, this, SLOT(widgetSizeMove()));
            QTimer::singleShot(10, this, SLOT(on_point1_clicked()));
        }
    }
    else
    {
        if (rightView->point1.isNull() || rightView->point2.isNull())
            qDebug() << "not enough point";
        else
        {
            qDebug() << "start measuring..." << rightView->point1 << rightView->point2;
            /* measure */
            measureSegment();
            showResult();
        }
    }

}

void PointsToKeyPoints(vector<Point2f>pts, vector<KeyPoint>& kpts)
{
    for (size_t i = 0; i < pts.size(); i++) {
        kpts.push_back(KeyPoint(pts[i], 1.f));
    }
}

void GuiMeasure::autoMatch()
{
    vector<Point2f> points1;
    vector<Point2f> points2;
    points1.push_back(Point2f(leftView->point1.x(),leftView->point1.y()));
    points2.push_back(Point2f(leftView->point2.x(),leftView->point2.y()));
    TermCriteria termcrit(TermCriteria::COUNT | TermCriteria::EPS, 50, 0.00001);
    cvtColor(leftImg, leftGray, COLOR_BGR2GRAY);
    cvtColor(rightImg, rightGray, COLOR_BGR2GRAY);
    cornerSubPix(leftGray, points1, Size(3, 3), Size(-1, -1), termcrit);
    cornerSubPix(leftGray, points2, Size(3, 3), Size(-1, -1), termcrit);
    vector<KeyPoint> left_keypoint1;
    PointsToKeyPoints(points1,left_keypoint1);
    vector<KeyPoint> left_keypoint2;
    PointsToKeyPoints(points2, left_keypoint2);

    cout<<"start autoMatcher"<<endl;
    Automatcher autoMatcher = Automatcher(this->leftImg, this->rightImg, "SIFT");
    vector <int> point1 = {0,0};
    autoMatcher.match(autoMatcher, left_keypoint1, true, point1);
    vector <int> point2 = {0,0};
    autoMatcher.match(autoMatcher, left_keypoint2, true, point2);

    QPoint matchedPoint1 = QPoint(point1[0], point1[1]);
    QPoint matchedPoint2 = QPoint(point2[0], point2[1]);
    qDebug() << matchedPoint1 << matchedPoint2;

    rightView->updatePoint(matchedPoint1, 1);
    rightView->updatePoint(matchedPoint2, 2);
}

void get_world_coord_Q(float Qmatrix[4][4], Point2f& left, Point2f& right, float* world_coord)
{
    float x = left.x;
    float y = left.y;
    float d = left.x - right.x;
    float vec[4] = { x,y,d,1.0 };
    int r1 = 4;
    int c1 = 4;
    float result[4] = {0,0,0,0};

    for (int i = 0; i < r1; ++i)
    {
        for (int k = 0; k < c1; ++k)
        {
            result[i] += Qmatrix[i][k] * vec[k];
        }
    }
    for (int i = 0; i < 3; ++i)
    {
        //cout << result[i]<<endl;
        world_coord[i] = result[i]/ result[3];
        //cout << "world" <<i<<endl<< world_coord[i] << endl;
    }
    //cout << result[3] << endl;
}

void GuiMeasure::measureSegment()
{
    /* left img */
    pointsLeft.push_back(Point2f(leftView->point1.x(),leftView->point1.y()));
    pointsLeft.push_back(Point2f(leftView->point2.x(),leftView->point2.y()));
    TermCriteria termcrit(TermCriteria::COUNT | TermCriteria::EPS, 50, 0.00001);
    cornerSubPix(leftGray, pointsLeft, Size(3, 3), Size(-1, -1), termcrit);

    /* right img */
    pointsRight.push_back(Point2f(rightView->point1.x(),rightView->point1.y()));
    pointsRight.push_back(Point2f(rightView->point2.x(),rightView->point2.y()));
    cornerSubPix(rightGray, pointsRight, Size(3, 3), Size(-1, -1), termcrit);

    float world_coord1[3];
    float world_coord2[3];
    get_world_coord_Q(Q, pointsLeft[0], pointsRight[0], world_coord1);
    get_world_coord_Q(Q, pointsLeft[1], pointsRight[1], world_coord2);
    cout << "Point1 coords(x,y,z): (" << world_coord1[0] << ", " << world_coord1[1] << ", " << world_coord1[2] << ')' << endl;
    cout << "Point2 coords(x,y,z): (" << world_coord2[0] << ", " << world_coord2[1] << ", " << world_coord2[2] << ')' << endl;
    segment_len = sqrt(pow((world_coord1[0] - world_coord2[0]), 2) + pow((world_coord1[1] - world_coord2[1]), 2) + pow((world_coord1[2] - world_coord2[2]), 2));

    qDebug() << "Measure done" << segment_len;
}

void GuiMeasure::showResult()
{
    int showRange= 200;
    int width = leftImg.size().width;
    int height = leftImg.size().height;
    Rect areaL1(max(int(pointsLeft[0].x) - showRange,0), max(int(pointsLeft[0].y) - showRange, 0),
        min(2 * showRange, width - max(int(pointsLeft[0].x) - showRange,0)),
        min(2 * showRange, height - max(int(pointsLeft[0].y) - showRange,0)));
    Rect areaL2(max(int(pointsLeft[1].x) - showRange,0), max(int(pointsLeft[1].y) - showRange, 0),
        min(2 * showRange, width - max(int(pointsLeft[1].x) - showRange,0)),
        min(2 * showRange, height - max(int(pointsLeft[1].y) - showRange,0)));
    Rect areaR1(max(int(pointsRight[0].x) - showRange,0), max(int(pointsRight[0].y) - showRange, 0),
        min(2 * showRange, width - max(int(pointsRight[0].x) - showRange,0)),
        min(2 * showRange, height - max(int(pointsRight[0].y) - showRange,0)));
    Rect areaR2(max(int(pointsRight[1].x) - showRange,0), max(int(pointsRight[1].y) - showRange, 0),
        min(2 * showRange, width - max(int(pointsRight[1].x) - showRange,0)),
        min(2 * showRange, height - max(int(pointsRight[1].y) - showRange,0)));
    Mat imgL1 = leftImg.clone()(areaL1);
    Mat imgL2 = leftImg.clone()(areaL2);
    Mat imgR1 = rightImg.clone()(areaR1);
    Mat imgR2 = rightImg.clone()(areaR2);
    Mat tleft = leftImg.clone();
    line(tleft, Point(int(pointsLeft[0].x), int(pointsLeft[0].y)), Point(int(pointsLeft[1].x), int(pointsLeft[1].y)),
        Scalar(0, 0, 255), 10, LINE_8);
    circle(tleft, Point(int(pointsLeft[0].x), int(pointsLeft[0].y)), 15, Scalar(0, 0, 255), -1);
    circle(tleft, Point(int(pointsLeft[1].x), int(pointsLeft[1].y)), 15, Scalar(0, 0, 255), -1);
    line(imgL1, Point(0, imgL1.size().height / 2), Point(imgL1.size().width / 2, imgL1.size().height / 2), Scalar(0, 0, 255), 5, LINE_8);
    line(imgL1, Point(imgL1.size().width / 2, imgL1.size().height / 2), Point(imgL1.size().width / 2, 0), Scalar(0, 0, 255), 5, LINE_8);
    line(imgL2, Point(0, imgL2.size().height / 2), Point(imgL2.size().width / 2, imgL2.size().height / 2), Scalar(0, 0, 255), 5, LINE_8);
    line(imgL2, Point(imgL2.size().width / 2, imgL2.size().height / 2), Point(imgL2.size().width / 2, 0), Scalar(0, 0, 255), 5, LINE_8);
    line(imgR1, Point(0, imgR1.size().height / 2), Point(imgR1.size().width / 2, imgR1.size().height / 2), Scalar(0, 0, 255), 5, LINE_8);
    line(imgR1, Point(imgR1.size().width / 2, imgR1.size().height / 2), Point(imgR1.size().width / 2, 0), Scalar(0, 0, 255), 5, LINE_8);
    line(imgR2, Point(0, imgR1.size().height / 2), Point(imgR1.size().width / 2, imgR1.size().height / 2), Scalar(0, 0, 255), 5, LINE_8);
    line(imgR2, Point(imgR1.size().width / 2, imgR1.size().height / 2), Point(imgR1.size().width / 2, 0), Scalar(0, 0, 255), 5, LINE_8);

    cv::resize(imgL1, imgL1, Size(height/2,height/2));
    cv::resize(imgL2, imgL2, Size(height/2,height/2));
    cv::resize(imgR1, imgR1, Size(height/2,height/2));
    cv::resize(imgR2, imgR2, Size(height/2,height/2));
    Mat point1, point2, Result;
    vconcat(imgL1, imgR1, point1);
    vconcat(imgL2, imgR2, point2);
    hconcat(point1,tleft,Result);
    hconcat(Result,point2,Result);
    string showLength ="Length of segment: ";
    string length = to_string(segment_len);
    length = length.substr(0, length.length()-4);
    showLength.append(length);
    showLength.append("mm.");

    putText(Result, showLength, Point(height/2 + 100,200), FONT_HERSHEY_SIMPLEX, 3, cv::Scalar(0, 255, 255), 10, 0, 0);

    /* load the result image */
    Mat rgbMat;
    cvtColor (Result,rgbMat,COLOR_BGR2RGB);
    QImage qImage((const uchar*)rgbMat.data,
                  rgbMat.cols, rgbMat.rows, rgbMat.cols*(rgbMat.channels ()),
                  QImage::Format_RGB888);
    QPixmap pixmap = QPixmap::fromImage (qImage);
    QGraphicsPixmapItem* item = new QGraphicsPixmapItem(pixmap);

    QGraphicsView* resultView = new QGraphicsView();
    resultView->setScene(new QGraphicsScene());
    resultView->scene()->addItem(item);
    resultView->showMaximized();
    QTimer::singleShot(150, [resultView, item](){
        resultView->fitInView(item, Qt::KeepAspectRatio);
    });
    resultView->setWindowTitle("Measure Result");

    this->close();
}
