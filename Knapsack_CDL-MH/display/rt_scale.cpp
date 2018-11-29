#include "rt_scale.h"
#include <QTimer>
#include <QtMath>
#include <QDateTime>
#include <QPainter>
#include <QDebug>

rt_scale::rt_scale(QWidget *parent) : QWidget(parent)
{
    margin = 10;
    Max_Scale = 10;     // 水平风速，满刻度 10m/s
    H_speed_scale = 1;
}

void rt_scale::paintEvent(QPaintEvent *event)
{

    Q_UNUSED(event);

    // 方向指针位置 - 多边形
    static const QPoint Needle_Points[4] = {
        QPoint(40, -70),
        QPoint(0, -40),
        QPoint(-40, -70),
        QPoint(0, 80)
    };

    // 定义各种颜色
    QColor bgColor(255,250,240,255);
    QColor Color_HeightValue(200, 100, 0, 255);     // 高度值字体颜色
    QColor Color_MyGreen(155, 255, 200, 255);
    QColor Color_MyRed(255, 155, 155, 255);
//    QColor Color_MyGray(200, 200, 200, 255);
    QColor Color_MyGray(216, 216, 216, 50);
    QColor Color_HDir(32, 56, 100, 255);
    QColor Color_HSpeed(0, 176, 240, 255);

    // 确定缩放尺度
    int w = width();
    int h = height();
    if(qAbs(h-HSizeHint)<3)
        h = HSizeHint;         // 消除各个行高的微小差别
    int baseSize = qMin(w/11, h);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.translate(0, h/2);     // 平移坐标系,垂直方向移动至水平中线
    painter.scale(baseSize / 200.0, baseSize / 200.0);       // 缩放

    // 绘制背景矩形长条
    QRectF rectF;
    rectF.setX(0);
    rectF.setY(-100);
    rectF.setWidth(w * 200 / baseSize);
    rectF.setHeight(200);
    painter.setPen(Qt::NoPen);
    painter.setBrush(bgColor);
//    painter.drawRect(rectF);

    // 设置绘图字体
    QFont font = painter.font();
    font.setBold(true);
    font.setFamily("Microsoft YaHei");
    font.setPointSize(35);
    painter.setFont(font);

    // 计算参数
    int bar_width = w * 200 / baseSize-1480;

    // 绘制矩形背景
    painter.translate(1100, 0);    // 平移坐标系原点，右移到...
//    rectF.setX(0);
//    rectF.setY(-100);
//    rectF.setWidth(bar_width);
//    rectF.setHeight(200);
//    painter.setBrush(Color_HSpeed);
//    painter.drawRect(rectF);

    // 绘制水平刻度线
    rectF.setX(0);
    rectF.setY(-100);
    rectF.setWidth(bar_width);
    rectF.setHeight(10);
    painter.setBrush(Qt::black);
    painter.drawRect(rectF);

    // 绘制垂直刻度线
    rectF.setX(0);
    rectF.setY(-100);
    rectF.setWidth(10);
    rectF.setHeight(40);
    painter.drawRect(rectF);

    rectF.setX(bar_width);
    rectF.setY(-100);
    rectF.setWidth(10);
    rectF.setHeight(40);
    painter.drawRect(rectF);

    rectF.setX(bar_width/2);
    rectF.setY(-100);
    rectF.setWidth(6);
    rectF.setHeight(30);
    painter.drawRect(rectF);

    // 绘制刻度数值
    painter.setPen(Qt::black);
    rectF.setX(-100);
    rectF.setY(-100);
    rectF.setWidth(200);
    rectF.setHeight(200);
    QString str = QString("0");

    painter.drawText(rectF, Qt::AlignCenter, str);
    rectF.setX(bar_width/2-100);
    rectF.setY(-100);
    rectF.setWidth(200);
    rectF.setHeight(200);
    str = QString("%1").arg(Max_Scale/2);

    painter.drawText(rectF, Qt::AlignCenter, str);
    rectF.setX(bar_width-100);
    rectF.setY(-100);
    rectF.setWidth(200);
    rectF.setHeight(200);
    str = QString("%1").arg(Max_Scale);
    painter.drawText(rectF, Qt::AlignCenter, str);

    painter.drawText(rectF, Qt::AlignCenter, str);
    rectF.setX(bar_width-100+100);
    rectF.setY(-100);
    rectF.setWidth(200);
    rectF.setHeight(200);
    str = QString("m/s");
    painter.drawText(rectF, Qt::AlignCenter, str);

}

// 设置控件高度的参考值
void rt_scale::setHSizeHint(int hhh)
{
    HSizeHint = hhh;
}

void rt_scale::setH_speed_scale(int scale)
{
    H_speed_scale = scale;
    Max_Scale = 10 * H_speed_scale;
    update();
}
