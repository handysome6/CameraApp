#include "mygraphicsview.h"

#include <QScrollBar>
#include <QGraphicsEllipseItem>
#include <QGraphicsItem>

MyGraphicsView::MyGraphicsView()
{
    /* aesthetic */
    setCursor(Qt::CursorShape::CrossCursor);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

MyGraphicsView::~MyGraphicsView()
{
    delete this->scene();
};

void MyGraphicsView::previewMode()
{
//    qDebug() << "entered preview mode";
    fitInView(this->sceneRect(), Qt::KeepAspectRatio);

    mode = Mode::PREVIEW;
    emit hideCrossWidget();
    removePoint();
    drawPoint();
}

void MyGraphicsView::zoominMode(QPointF const& zoomPoint)
{
//    qDebug() << "entered zoomin mode" << zoomPoint;
    this->resetTransform();
    this->centerOn(zoomPoint);

    mode = Mode::ZOOMIN;
    emit showCrossWidget();
    removePoint();
}

void MyGraphicsView::drawPoint()
{
    const int radius = 30;
    if (!point1.isNull())
    {
        auto circle = new QGraphicsEllipseItem(point1.x()-radius, point1.y()-radius,
                                               2*radius, 2*radius);
        circle->setBrush(Qt::red);
        scene()->addItem(circle);
    }
    if (!point2.isNull())
    {
        auto circle = new QGraphicsEllipseItem(point2.x()-radius, point2.y()-radius,
                                               2*radius, 2*radius);
        circle->setBrush(Qt::red);
        scene()->addItem(circle);
    }
}

void MyGraphicsView::removePoint()
{
    for (QGraphicsItem* item : scene()->items())
    {
        if (item->type() == QGraphicsEllipseItem::Type)
            scene()->removeItem(item);
    }
}

void MyGraphicsView::updatePoint(QPointF const& p, int currentPoint)
{
    if (currentPoint == 1)
        point1 = p;
    else
        point2 = p;

    if (!(point1.isNull() || point2.isNull()))
        emit finishPicking();
}

void MyGraphicsView::mousePressEvent(QMouseEvent  *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (mode == Mode::PREVIEW)
        {
            QPointF p = mapToScene(event->pos());
            zoominMode(p);
            updatePoint(p, SELECTING_POINT);
        }else
        {
//            qDebug() << "dragging started @" << mapToScene(viewport()->rect().center());
            mouseDragging = true;
            lastPoint = event->pos();
            setCursor(Qt::CursorShape::OpenHandCursor);
        }
    }
}
void MyGraphicsView::mouseMoveEvent(QMouseEvent * event)
{
    /* No button here, Qt::NoButton */
    if ( mouseDragging )
    {
        QPointF delta = event->pos() - lastPoint;
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
        lastPoint = event->pos();
    }
}
void MyGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && mouseDragging)
    {
        const QPointF p = mapToScene(viewport()->rect().center());
        updatePoint(p, SELECTING_POINT);
//        qDebug() << "dragging ended @" << p;
        mouseDragging = false;
        setCursor(Qt::CursorShape::CrossCursor);
        previewMode();
    }
}
