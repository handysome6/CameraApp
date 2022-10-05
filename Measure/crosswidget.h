#ifndef CROSSWIDGET_H
#define CROSSWIDGET_H
#include <QWidget>

class CrossWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CrossWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent*) override;

signals:

};

#endif // CROSSWIDGET_H
