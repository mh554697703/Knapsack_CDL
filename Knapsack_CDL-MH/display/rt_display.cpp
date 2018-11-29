#include "rt_display.h"
#include <QtMath>
#include <QPainter>
#include <QDebug>

rt_display::rt_display(QWidget *parent) : QWidget(parent)
{
    H_speed = 0;
    H_direction = 0;
    V_speed = 0;
    H_speed_scale = 1;
    HSizeHint = 0;
    H = 0;
    connect(this, SIGNAL(data_changed()), this, SLOT(update())); //update会自动调用paintEvent()来重新绘图更新显示
}

QRectF rt_display::textRectF(double radius, int pointSize, double angle)
{
    QRectF rectF;
    rectF.setX(radius*qCos(angle*M_PI/180.0) - pointSize*2);
    rectF.setY(radius*qSin(angle*M_PI/180.0) - pointSize/2.0);
    rectF.setWidth(pointSize*4);
    rectF.setHeight(pointSize);
    return rectF;
}

void rt_display::paintEvent(QPaintEvent *event)
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
    QColor Color_HDir(32, 56, 100, 255);
    QColor Color_HeightValue(200, 100, 0, 255);     // 高度值字体颜色
    QColor Color_HSpeed(0, 176, 240, 255);
    QColor MarklineColor(0, 127, 127, 150);
    QColor NeedleColor(255, 255, 255, 255);

    // 确定缩放尺度
    int w = width();
    int h = height();

    h = HSizeHint;         // 消除各个行高的微小差别
//        qDebug()<<"h = "<<h<<"    w == "<<w;
    int baseSize = qMin(w/11, h);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.translate(0, h/2);     // 平移坐标系,垂直方向移动至水平中线
    painter.scale(baseSize / 200.0, baseSize / 200.0);       // 缩放
//    qDebug()<<"baseSize = "<<baseSize;
//    qDebug()<<"baseSize / 200.0 = "<<baseSize / 200.0;

    // 绘制背景矩形长条
    QRectF rectF;
    rectF.setX(0);
    rectF.setY(-100);
    rectF.setWidth(w * 200 / baseSize);
    rectF.setHeight(200);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255,250,240));
//    painter.drawRect(rectF);

    // 设置绘图字体
    QFont font = painter.font();
    font.setBold(true);
    font.setFamily("Microsoft YaHei");
    font.setPointSize(35);
    painter.setFont(font);

    //绘制高度坐标值

    painter.setPen(Color_HeightValue);
    rectF.setX(0);
    rectF.setY(-100);
    rectF.setWidth(300);
    rectF.setHeight(200);
    QString str =  QString::number(H, 'f', 2);
    str += "m";
    painter.drawText(rectF, Qt::AlignCenter, str);

    painter.translate(300, 0);    // 平移坐标系原点，右移到风向值起始点

    // 绘制风向大小文本
    rectF.setX(0);
    rectF.setY(-100);
    rectF.setWidth(300);
    rectF.setHeight(200);

    painter.setPen(Color_HDir);
    str =  QString::number(H_direction, 'f', 2);
    str += QString::fromLocal8Bit("°");
    painter.drawText(rectF, Qt::AlignCenter, str);

    // 圆盘向右平移
    painter.translate(400, 0);    // 平移坐标系原点，右移到风向圆盘中心点
    // 绘制矩形背景
    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(Color_HSpeed.darker(100+(H_speed_scale-1)*40*(H_speed_scale-1)));
    painter.drawRect(0, -80, 380, 160);
    painter.restore();
    // 绘制圆形背景
    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    painter.drawEllipse(QPointF(0, 0), 100, 100);
    painter.setBrush(Color_HDir);
    painter.drawEllipse(QPointF(0, 0), 80, 80);
    painter.restore();

    // 绘制方向刻度线 （360度 / 8 = 45度）
    for (int i = 0; i < 8; ++i)
    {
        painter.drawLine(88, 0, 96, 0);
        painter.rotate(45.0);
    }

    // 绘制方向角度文本
    painter.save();
    int radius = 100;
    font.setPointSize(5);
    painter.setFont(font);
    int pointSize = 20;
    painter.setPen(Qt::white);

    int AngleMark = 0;
    for (int i = 0; i < 4; ++i) {
        AngleMark = (i * 90+90)%360;
        painter.drawText(textRectF(radius*0.6, pointSize, i * 90), Qt::AlignCenter, QString::number(AngleMark));
    }
    painter.restore();

    // 绘制细刻度线 （360度 / 16 = 22.5度）
    painter.setPen(MarklineColor);
    for (int j = 0; j < 16; ++j) {
        if ((j % 2) != 0)
            painter.drawLine(92, 0, 96, 0);
        painter.rotate(22.5);
    }

    // 绘制方向指针
    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(NeedleColor);
    painter.rotate(H_direction);                        //*********读入风向，绘制方向指针。。
    painter.drawConvexPolygon(Needle_Points, 4);
    painter.restore();

    // 绘制水平风速大小文本
    rectF.setX(100);
    rectF.setY(-100);
    rectF.setWidth(200);
    rectF.setHeight(200);
    painter.setPen(Qt::white);
    painter.drawText(rectF, Qt::AlignCenter, QString::number(H_speed, 'f', 2));

    painter.translate(400, 0);    // 平移坐标系原点，右移到风速条起始点
    // 绘制水平风速矩形条
    painter.save();
    painter.setPen(Qt::NoPen);
    //H_speed_scale = qCeil((qreal)(H_speed) / 10);     // 调试用
    painter.setBrush(Color_HSpeed.darker(100+(H_speed_scale-1)*40*(H_speed_scale-1)));
    painter.drawRect(0, -80, H_speed/H_speed_scale*(w*200/baseSize-1480)/10, 160);
    painter.restore();

    // 绘制垂直风速三角形
    QPoint VS_Triangle_P[3] = {         // 垂直方向指针位置 - 向上三角形
        QPoint(0, -80),
        QPoint(80, 80),
        QPoint(0, 80)
    };

    QPoint VS_Triangle_N[3] = {         // 垂直方向指针位置 - 向下三角形
        QPoint(0, 80),
        QPoint(80, -80),
        QPoint(0, -80)
    };

    painter.save();
    painter.translate(H_speed/H_speed_scale*(w*200/baseSize-1480)/10+5, 0);
//    painter.translate((w*200/baseSize - 1400), 0);
    painter.setPen(Qt::NoPen);

    if(V_speed>=0)
    {
        painter.setBrush(Qt::red);
        int VT_num = qFloor(V_speed/3);
        double V_length = V_speed - 3 * VT_num;
        for(int i=0;i<VT_num;i++)
        {
            painter.drawConvexPolygon(VS_Triangle_P, 3);
            painter.translate(80,0);
        }
        VS_Triangle_P[1].setX(qRound(V_length*80/3));
        VS_Triangle_P[1].setY(qRound(V_length*80/1.5-80));
        VS_Triangle_P[2].setY(qRound(V_length*80/1.5-80));
        painter.drawConvexPolygon(VS_Triangle_P, 3);
    }
    else
    {
        painter.setBrush(QColor(0,176,0));
        int VT_num = qFloor(V_speed/(-3));
        double V_length = V_speed + 3 * VT_num;
        for(int i=0;i<VT_num;i++)
        {
            painter.drawConvexPolygon(VS_Triangle_N, 3);
            painter.translate(80,0);
        }
        VS_Triangle_N[1].setX(qRound(V_length*(-80)/3));
        VS_Triangle_N[1].setY(qRound(V_length*80/1.5+80));
        VS_Triangle_N[2].setY(qRound(V_length*80/1.5+80));
        painter.drawConvexPolygon(VS_Triangle_N, 3);
    }
    painter.restore();

    // 绘制垂直风速文本
    painter.translate(w*200/baseSize-1400, 0);    // 平移坐标系原点，右移到垂直风速起始点
    painter.save();
    rectF.setX(0);
    rectF.setY(-100);
    rectF.setWidth(300);
    rectF.setHeight(200);

    if(V_speed>=0)
    {
        painter.setPen(Qt::red);
    }
    else
    {
        painter.setPen(QColor(0,176,0));
    }
    painter.drawText(rectF, Qt::AlignCenter, QString::number(V_speed, 'f', 2)); //// 保留2位小数
    painter.restore();
}

// 设置高度层数值
void rt_display::setHeight(const double h)
{
    if(H != h)
    {
        H = h;
        emit data_changed();
    }
}

// 设置水平风速值
void rt_display::setHSpeed(const double sp, const int scale)
{
    bool need2update = false;
    if(H_speed != sp)
    {
        H_speed = sp;
        need2update = true;
    }
    if(H_speed_scale != scale)
    {
        H_speed_scale = scale;
        need2update = true;
    }
    if(need2update)
        emit data_changed();
//    qDebug()<<"sp = "<<H_speed<<"   SCALE= "<<H_speed_scale;
}

// 设置水平风向，0~360度
void rt_display::setHdirection(const double dir)
{
    if(H_direction != dir)
    {
        H_direction = dir;
        emit data_changed();
    }
}

// 设置垂直风速值，有正负
void rt_display::setVSpeed(const double sp)
{
    if(V_speed != sp)
    {
        V_speed = sp;
        emit data_changed();
    }
}

// 设置控件高度的参考值
void rt_display::setHSizeHint(int hhh)
{
    HSizeHint = hhh;
}

