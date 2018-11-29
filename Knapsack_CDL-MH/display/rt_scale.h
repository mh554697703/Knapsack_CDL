#ifndef RT_SCALE_H
#define RT_SCALE_H

#include <QWidget>
#include <QRectF>
#include <QDebug>
class rt_scale : public QWidget
{
    Q_OBJECT
public:
    explicit rt_scale(QWidget *parent = nullptr);
public:
    void setHSizeHint(int hhh);
    void setH_speed_scale(int scale);
signals:

public slots:

private:
    int margin;
    int Max_Scale;
    int HSizeHint;      // Widget¸ß¶È
    int H_speed_scale;
protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // RT_SCALE_H
