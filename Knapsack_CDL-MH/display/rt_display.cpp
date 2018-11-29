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
    connect(this, SIGNAL(data_changed()), this, SLOT(update())); //update���Զ�����paintEvent()�����»�ͼ������ʾ
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

    // ����ָ��λ�� - �����
    static const QPoint Needle_Points[4] = {
        QPoint(40, -70),
        QPoint(0, -40),
        QPoint(-40, -70),
        QPoint(0, 80)
    };

    // ���������ɫ
    QColor Color_HDir(32, 56, 100, 255);
    QColor Color_HeightValue(200, 100, 0, 255);     // �߶�ֵ������ɫ
    QColor Color_HSpeed(0, 176, 240, 255);
    QColor MarklineColor(0, 127, 127, 150);
    QColor NeedleColor(255, 255, 255, 255);

    // ȷ�����ų߶�
    int w = width();
    int h = height();

    h = HSizeHint;         // ���������иߵ�΢С���
//        qDebug()<<"h = "<<h<<"    w == "<<w;
    int baseSize = qMin(w/11, h);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.translate(0, h/2);     // ƽ������ϵ,��ֱ�����ƶ���ˮƽ����
    painter.scale(baseSize / 200.0, baseSize / 200.0);       // ����
//    qDebug()<<"baseSize = "<<baseSize;
//    qDebug()<<"baseSize / 200.0 = "<<baseSize / 200.0;

    // ���Ʊ������γ���
    QRectF rectF;
    rectF.setX(0);
    rectF.setY(-100);
    rectF.setWidth(w * 200 / baseSize);
    rectF.setHeight(200);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255,250,240));
//    painter.drawRect(rectF);

    // ���û�ͼ����
    QFont font = painter.font();
    font.setBold(true);
    font.setFamily("Microsoft YaHei");
    font.setPointSize(35);
    painter.setFont(font);

    //���Ƹ߶�����ֵ

    painter.setPen(Color_HeightValue);
    rectF.setX(0);
    rectF.setY(-100);
    rectF.setWidth(300);
    rectF.setHeight(200);
    QString str =  QString::number(H, 'f', 2);
    str += "m";
    painter.drawText(rectF, Qt::AlignCenter, str);

    painter.translate(300, 0);    // ƽ������ϵԭ�㣬���Ƶ�����ֵ��ʼ��

    // ���Ʒ����С�ı�
    rectF.setX(0);
    rectF.setY(-100);
    rectF.setWidth(300);
    rectF.setHeight(200);

    painter.setPen(Color_HDir);
    str =  QString::number(H_direction, 'f', 2);
    str += QString::fromLocal8Bit("��");
    painter.drawText(rectF, Qt::AlignCenter, str);

    // Բ������ƽ��
    painter.translate(400, 0);    // ƽ������ϵԭ�㣬���Ƶ�����Բ�����ĵ�
    // ���ƾ��α���
    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(Color_HSpeed.darker(100+(H_speed_scale-1)*40*(H_speed_scale-1)));
    painter.drawRect(0, -80, 380, 160);
    painter.restore();
    // ����Բ�α���
    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    painter.drawEllipse(QPointF(0, 0), 100, 100);
    painter.setBrush(Color_HDir);
    painter.drawEllipse(QPointF(0, 0), 80, 80);
    painter.restore();

    // ���Ʒ���̶��� ��360�� / 8 = 45�ȣ�
    for (int i = 0; i < 8; ++i)
    {
        painter.drawLine(88, 0, 96, 0);
        painter.rotate(45.0);
    }

    // ���Ʒ���Ƕ��ı�
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

    // ����ϸ�̶��� ��360�� / 16 = 22.5�ȣ�
    painter.setPen(MarklineColor);
    for (int j = 0; j < 16; ++j) {
        if ((j % 2) != 0)
            painter.drawLine(92, 0, 96, 0);
        painter.rotate(22.5);
    }

    // ���Ʒ���ָ��
    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(NeedleColor);
    painter.rotate(H_direction);                        //*********������򣬻��Ʒ���ָ�롣��
    painter.drawConvexPolygon(Needle_Points, 4);
    painter.restore();

    // ����ˮƽ���ٴ�С�ı�
    rectF.setX(100);
    rectF.setY(-100);
    rectF.setWidth(200);
    rectF.setHeight(200);
    painter.setPen(Qt::white);
    painter.drawText(rectF, Qt::AlignCenter, QString::number(H_speed, 'f', 2));

    painter.translate(400, 0);    // ƽ������ϵԭ�㣬���Ƶ���������ʼ��
    // ����ˮƽ���پ�����
    painter.save();
    painter.setPen(Qt::NoPen);
    //H_speed_scale = qCeil((qreal)(H_speed) / 10);     // ������
    painter.setBrush(Color_HSpeed.darker(100+(H_speed_scale-1)*40*(H_speed_scale-1)));
    painter.drawRect(0, -80, H_speed/H_speed_scale*(w*200/baseSize-1480)/10, 160);
    painter.restore();

    // ���ƴ�ֱ����������
    QPoint VS_Triangle_P[3] = {         // ��ֱ����ָ��λ�� - ����������
        QPoint(0, -80),
        QPoint(80, 80),
        QPoint(0, 80)
    };

    QPoint VS_Triangle_N[3] = {         // ��ֱ����ָ��λ�� - ����������
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

    // ���ƴ�ֱ�����ı�
    painter.translate(w*200/baseSize-1400, 0);    // ƽ������ϵԭ�㣬���Ƶ���ֱ������ʼ��
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
    painter.drawText(rectF, Qt::AlignCenter, QString::number(V_speed, 'f', 2)); //// ����2λС��
    painter.restore();
}

// ���ø߶Ȳ���ֵ
void rt_display::setHeight(const double h)
{
    if(H != h)
    {
        H = h;
        emit data_changed();
    }
}

// ����ˮƽ����ֵ
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

// ����ˮƽ����0~360��
void rt_display::setHdirection(const double dir)
{
    if(H_direction != dir)
    {
        H_direction = dir;
        emit data_changed();
    }
}

// ���ô�ֱ����ֵ��������
void rt_display::setVSpeed(const double sp)
{
    if(V_speed != sp)
    {
        V_speed = sp;
        emit data_changed();
    }
}

// ���ÿؼ��߶ȵĲο�ֵ
void rt_display::setHSizeHint(int hhh)
{
    HSizeHint = hhh;
}

