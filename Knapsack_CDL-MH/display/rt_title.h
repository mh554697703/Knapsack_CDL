#ifndef RT_TITLE_H
#define RT_TITLE_H

#include <QWidget>
#include <QRectF>
#include <QDebug>
class rt_title : public QWidget
{
    Q_OBJECT
public:
    explicit rt_title(QWidget *parent = nullptr);
public:
    void setHSizeHint(int hhh);
    void setHSpeed_Scale(const int scale);
signals:
    void H_Scale_changed();
public slots:

private:
    int margin;
    int HSizeHint;      // Widget高度
    int H_speed_scale;  // 水平风速scale，决定了水平风速标题栏的背景颜色
protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // RT_TITLE_H
