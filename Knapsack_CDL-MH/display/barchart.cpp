#include "barchart.h"
#include <QDebug>

barchart::barchart()
{
    axis = new QBarCategoryAxis();
    barseries = new QHorizontalStackedBarSeries();
    addSeries(barseries);
    barchartFont.setPointSize(10);
    barchartFont.setFamily("Microsoft Yahei");

    createDefaultAxes();
    setAxisY(axis,barseries);
    axisX(barseries)->setRange(-5,5);
    axisX(barseries)->setTitleFont(barchartFont);
    axisX(barseries)->setLabelsFont(barchartFont);
//    axisX(barseries)->setTitleText(QString::fromLocal8Bit("风速 (m/s)"));
}

barchart::~barchart()
{
}

void barchart::setAxis(const double *h, unsigned int n)
{
    QStringList categories;
    QString b;
    for(uint i=0;i<n;i++)
    {
            b = QString::number(h[i],'f',0);
            categories.append(b);
    }
//    categories << "200m" << "300m" << "400m" << "500m" << "600m" << "700m";
    axis->clear();
    axis->append(categories);
//    axis->setTitleText(QString::fromLocal8Bit("高度 (m)"));
    removeAxis(axisY());
    setAxisY(axis,barseries);
    axisY()->setTitleFont(barchartFont);
    axisY()->setLabelsFont(barchartFont);
}

void barchart::updateData(const double *sp, const unsigned int n)
{
    Velocity = new double [n];
    for(unsigned int i=0;i<n;i++)
    {
        Velocity[i] = sp[i];
    }
    QBarSet *barset2 = new QBarSet("velocity");
    for(uint i=0;i<n;i++)
    {
        barset2->append(Velocity[i]);
    }
    barseries->clear();
    barseries->append(barset2);

    QList<int> list;
    for(int i=0;i<n;i++)
        list.push_back(Velocity[i]);
    qSort(list.begin(), list.end());
    int Xmin=list.first();
    int Xmax=list.last();
    if(Xmin<=0)
    {
        if(Xmin<=-5)
            Xmin=-10;
        else
            Xmin=-5;
    }
    else
        Xmin=0;
    if(Xmax>=0)
    {
        if(Xmax>=5)
        {
            if(Xmax>=10)
                Xmax = 20;
            else
                Xmax = 10;
        }
        else
            Xmax =5;
    }
    else
        Xmax=0;
    int XminA =qAbs(Xmin);
    int max = qMax(Xmax,XminA);
    axisX()->setRange(-max,max);
    delete Velocity;
}
