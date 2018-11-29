#include "wind_display.h"
#include <QWidget>
#include <QtMath>

wind_display::wind_display(QWidget *parent) : QWidget(parent)
{
    Vlayout = new QVBoxLayout;
    my_title = new rt_title;
    my_scale = new rt_scale;
    Vlayout->addWidget(my_title);
    Vlayout->setStretch(0, 1);
    nLayers = 2;
    for (int i = 0; i < Max_nLayers; ++i)
    {
        // 设置100~1000米的nLayers个标签
        my_display[i] = new rt_display;
        my_display[i]->setHeight(0);
        my_display[i]->setHSpeed(0,1);
        my_display[i]->setHdirection(0);
        my_display[i]->setVSpeed(0);
        if (i < nLayers)
        {
            Vlayout->addWidget(my_display[i]);
            Vlayout->setStretch(i+1, 1);
        }
    }
    Vlayout->addWidget(my_scale);
    Vlayout->setStretch(nLayers+1, 1);

    setLayout(Vlayout);    //整体布局
    HSpeedScale = 1;
    setMinimumSize(240, 640);
}

wind_display::~wind_display()
{
    delete Vlayout;
    delete my_title;
    for (int i = 0; i < nLayers; ++i)
    {
        delete my_display[i];
    }
    delete my_scale;
}

void wind_display::setHSpeed(const double *sp)
{
    int MaxHSpeed = 0;
    for(int i=0;i<nLayers;i++)
        if(MaxHSpeed < *(sp+i))
            MaxHSpeed = *(sp+i);
    int New_HSpeedScale;
    if(MaxHSpeed<=10)
        New_HSpeedScale = 1;
    else
        New_HSpeedScale = qCeil((qreal)(MaxHSpeed) / 10);

    //
    for(int i=0;i<nLayers;i++)
        if(HSpeed[i] != sp[nLayers - i - 1])
        {
            HSpeed[i] = sp[nLayers - i - 1];
            my_display[i]->setHSpeed(HSpeed[i], New_HSpeedScale);
        }
    if(New_HSpeedScale != HSpeedScale)
    {
        HSpeedScale = New_HSpeedScale;
        my_scale->setH_speed_scale(HSpeedScale);
        my_title->setHSpeed_Scale(HSpeedScale);
    }
}

void wind_display::setHDirection(const double *dir)
{
    for(int i=0;i<nLayers;i++)
        if(H_direction[i] != dir[nLayers - i - 1])
        {
            H_direction[i] = dir[nLayers - i - 1];
            my_display[i]->setHdirection(H_direction[i]);
        }
}

void wind_display::setVSpeed(const double *sp)
{
    for(int i=0;i<nLayers;i++)
        if(VSpeed[i] != sp[nLayers - i - 1])
        {
            VSpeed[i] = sp[nLayers - i - 1];
            my_display[i]->setVSpeed(VSpeed[i]);
        }
}

void wind_display::setHeights(const double *h)
{
    for(int i=0;i<nLayers;i++)
        if(Heights[i] != h[nLayers - i - 1])
        {
            Heights[i] = h[nLayers - i - 1];
            my_display[i]->setHeight(Heights[i]);
        }
}

void wind_display::setSubSize()
{
    int h = height();
    h = int((h-99)/(nLayers+2));
    if(h<=0)
        h = 10;
    my_title->setHSizeHint(h);
    for(int i=0;i<nLayers;i++)
    {
        my_display[i]->setHSizeHint(h);
    }
    my_scale->setHSizeHint(h);
}

void wind_display::set_nLayers(int n)
{
    // 先remove所有标签
    if(n != nLayers)
    {
        Vlayout->removeWidget(my_scale);
        for (int i = 0; i < nLayers; ++i)
        {
            Vlayout->removeWidget(my_display[i]);
            my_display[i]->hide();
        }
        nLayers = n;
        // 再add需要的标签
        for (int i = 0; i < nLayers; ++i)
        {
            my_display[i]->setHeight(0);
            my_display[i]->setHSpeed(0,1);
            my_display[i]->setHdirection(0);
            my_display[i]->setVSpeed(0);
            my_display[i]->show();
            Vlayout->addWidget(my_display[i]);
            Vlayout->setStretch(i+1, 1);
        }
        Vlayout->addWidget(my_scale);
        Vlayout->setStretch(nLayers+1, 1);
    }
    setSubSize();
}

void wind_display::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    setSubSize();
}
