#include "rt_title.h"
#include <QTimer>
#include <QtMath>
#include <QDateTime>
#include <QPainter>
#include <QDebug>

rt_title::rt_title(QWidget *parent) : QWidget(parent)
{
    margin = 10;
    H_speed_scale = 1;
    connect(this, SIGNAL(H_Scale_changed()), this, SLOT(update()));
}

void rt_title::paintEvent(QPaintEvent *event)
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
    QColor bgColor(255,250,240,255);
    QColor Color_HeightValue(200, 100, 0, 255);     // �߶�ֵ������ɫ
    QColor Color_MyGreen(155, 255, 200, 255);
    QColor Color_MyRed(255, 155, 155, 255);
//    QColor Color_MyGray(200, 200, 200, 255);
    QColor Color_MyGray(216, 216, 216, 50);
    QColor Color_HDir(32, 56, 100, 255);
    QColor Color_HSpeed(0, 176, 240, 255);

    // ȷ�����ų߶�
    int w = width();
    int h = height();
    if(qAbs(h-HSizeHint)<3)
        h = HSizeHint;         // ���������иߵ�΢С���
    int baseSize = qMin(w/11, h);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.translate(0, h/2);     // ƽ������ϵ,��ֱ�����ƶ���ˮƽ����
    painter.scale(baseSize / 200.0, baseSize / 200.0);       // ����

    // ���Ʊ������γ���
    QRectF rectF;
    rectF.setX(0);
    rectF.setY(-100);
    rectF.setWidth(w * 200 / baseSize);
    rectF.setHeight(200);
    painter.setPen(Qt::NoPen);
    painter.setBrush(bgColor);
    painter.drawRect(rectF);

    // ���û�ͼ����
    QFont font = painter.font();
    font.setBold(true);
    font.setFamily("Microsoft YaHei");
    font.setPointSize(35);
    painter.setFont(font);

    // ����̽��߶ȱ���
    painter.setBrush(Color_HeightValue);
    painter.setPen(Qt::white);
    rectF.setX(margin);
    rectF.setY(-80);
    rectF.setWidth(300-2*margin);
    rectF.setHeight(160);
    QString str = QString::fromLocal8Bit("̽��߶�");
    painter.drawRect(rectF);
    painter.drawText(rectF, Qt::AlignCenter, str);


    painter.translate(700, 0);    // ƽ������ϵԭ�㣬���Ƶ�����ֵ��ʼ��
    //����ˮƽ���ٱ���
    rectF.setX(margin);
    rectF.setY(-80);
    rectF.setWidth(w*200/baseSize - 1000 -2*margin);
    rectF.setHeight(160);
    str = QString::fromLocal8Bit("ˮƽ����");
    painter.setPen(Qt::NoPen);
    painter.setBrush(Color_HSpeed.darker(100+(H_speed_scale-1)*40*(H_speed_scale-1)));
    painter.drawRect(rectF);

    painter.setPen(Qt::white);
    painter.drawText(rectF, Qt::AlignCenter, str);


    painter.translate(-400, 0);    // ƽ������ϵԭ�㣬���Ƶ�����ֵ��ʼ��

    //����ˮƽ�������
    // ����Բ�α���
    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    painter.drawEllipse(QPointF(400, 0), 100, 100);
    painter.setBrush(Color_HDir);
    painter.drawEllipse(QPointF(400, 0), 80, 80);
    painter.restore();

    rectF.setX(margin);
    rectF.setY(-80);
    rectF.setWidth(400-margin);
    rectF.setHeight(160);
    str = QString::fromLocal8Bit("ˮƽ����");
    painter.setPen(Qt::NoPen);
    painter.setBrush(Color_HDir);
    painter.drawRect(rectF);

    painter.setPen(Qt::white);
    painter.drawText(rectF, Qt::AlignCenter, str);


    painter.translate(w*200/baseSize - 600, 0);    // ƽ������ϵԭ�㣬���Ƶ�����ֵ��ʼ��
    //���ƴ�ֱ�����
    rectF.setX(margin);
    rectF.setY(-80);
    rectF.setWidth(300 -2*margin);
    rectF.setHeight(160);
    str = QString::fromLocal8Bit("��ֱ��");
    painter.setPen(Qt::NoPen);
    painter.setBrush(Color_MyGray);
//    painter.setBrush(Qt::black);
    painter.drawRect(rectF);

    //     ���ƴ�ֱ����������
    QPoint VS_Triangle_P[3] = {         // ��ֱ����ָ��λ�� - ����������
        QPoint(margin, -80),
        QPoint(margin+50, 80),
        QPoint(margin, 80)
    };

    QPoint VS_Triangle_N[3] = {         // ��ֱ����ָ��λ�� - ����������
        QPoint(margin, 80),
        QPoint(margin+50, -80),
        QPoint(margin, -80)
    };
    painter.setBrush(Color_MyGreen);
    painter.drawConvexPolygon(VS_Triangle_N, 3);
    painter.setBrush(Color_MyRed);
    painter.drawConvexPolygon(VS_Triangle_P, 3);

    QPoint VS_Triangle_PP[3] = {         // ��ֱ����ָ��λ�� - ����������
        QPoint(300-margin, -80),
        QPoint(250-margin, 80),
        QPoint(300-margin, 80)
    };

    QPoint VS_Triangle_NN[3] = {         // ��ֱ����ָ��λ�� - ����������
        QPoint(300-margin, 80),
        QPoint(250-margin, -80),
        QPoint(300-margin, -80)
    };
    painter.setBrush(Color_MyRed);
    painter.drawConvexPolygon(VS_Triangle_PP, 3);
    painter.setBrush(Color_MyGreen);
    painter.drawConvexPolygon(VS_Triangle_NN, 3);

    painter.setPen(Qt::black);
    painter.drawText(rectF, Qt::AlignCenter, str);

}

// ���ÿؼ��߶ȵĲο�ֵ
void rt_title::setHSizeHint(int hhh)
{
    HSizeHint = hhh;
}

void rt_title::setHSpeed_Scale(const int scale)
{
    if(H_speed_scale != scale)
    {
        H_speed_scale = scale;
        emit H_Scale_changed();
    }
}
