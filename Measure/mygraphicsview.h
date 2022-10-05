#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H

#include <QObject>
#include <QGraphicsView>
#include <QDebug>
#include <QMimeData>
#include <QDrag>
#include <Qt>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>

enum class Mode {PREVIEW, ZOOMIN};


class MyGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    MyGraphicsView();
    ~MyGraphicsView();

    void previewMode();
    void zoominMode(QPointF const&);
    void drawPoint();
    void removePoint();
    void updatePoint(QPointF const&, int);
    int SELECTING_POINT = 1;
    QPointF point1, point2;

signals:
    void showCrossWidget();
    void hideCrossWidget();
    void finishPicking();

protected:
    void mousePressEvent(QMouseEvent  *event) override;
    void mouseMoveEvent(QMouseEvent * event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
//    void dragEnterEvent(QDragEnterEvent  *event) override;
//    void dragMoveEvent(QDragMoveEvent  *event) override;
//    void dropEvent(QDropEvent *event) override;


private:
    Mode mode = Mode::PREVIEW;
    bool mouseDragging = false;
    QPointF lastPoint;
};


#endif // MYGRAPHICSVIEW_H
