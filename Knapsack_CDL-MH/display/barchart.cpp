#include "barchart.h"
#include <QDebug>

barchart::barchart()
{
    axis = new QBarCategoryAxis();
    barseries = new QHorizontalStackedBarSeries();
    addSeries(barseries);
    barchartFont.setPointSize(14);
    barchartFont.setBold(true);

    createDefaultAxes();
    setAxisY(axis,barseries);
    axisX(barseries)->setRange(-10,10);
    axisX(barseries)->setTitleFont(barchartFont);
    axisX(barseries)->setTitleText(QString::fromLocal8Bit("风速 (m/s)"));
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
        b = QString::number(h[i],'g',5);
        categories.append(b);
    }
//    categories << "200m" << "300m" << "400m" << "500m" << "600m" << "700m";
    axis->clear();
    axis->append(categories);
    axis->setTitleText(QString::fromLocal8Bit("高度 (m)"));
    removeAxis(axisY());
    setAxisY(axis,barseries);
    axisY()->setTitleFont(barchartFont);
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
    delete Velocity;
}
