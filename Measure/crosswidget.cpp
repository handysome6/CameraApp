#include "crosswidget.h"
#include <Qt>
#include <QPaintEvent>
#include <QPainter>
#include <QString>
#include <QFontMetrics>
#include <QSize>

CrossWidget::CrossWidget(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
}

void CrossWidget::paintEvent(QPaintEvent*)
{
    resize(QSize(200, 200));
    QPainter painter(this);
    qreal r = 1;
    painter.setPen(QPen(Qt::red, r*2, Qt::SolidLine, Qt::RoundCap));
    painter.drawLine(0, 100-r, 200, 100-r);
    painter.drawLine(100-r, 0, 100-r, 200);
}
