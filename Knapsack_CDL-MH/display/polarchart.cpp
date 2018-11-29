#include "polarchart.h"
#include <QtCharts/QValueAxis>
#include <QtCharts/QPolarChart>
#include <QFont>
#include <QDebug>

polarchart::polarchart():
   myseries(nullptr)
{
    angularAxis = new QValueAxis();
    radialAxis = new QValueAxis();
    setAxis();
    myseries = new QLineSeries();
    mypen.setColor(QColor(255,0,0));
    mypen.setWidth(4);
    myseries->setPen(mypen);
    myseries2 = new QScatterSeries();
    myseries2->setColor(QColor(100,0,255));
    myseries2->setMarkerSize(8);

    addSeries(myseries);
    addSeries(myseries2);
    addAxis(angularAxis,QPolarChart::PolarOrientationAngular);
    addAxis(radialAxis,QPolarChart::PolarOrientationRadial);
    myseries->attachAxis(angularAxis);
    myseries->attachAxis(radialAxis);
    myseries2->attachAxis(angularAxis);
    myseries2->attachAxis(radialAxis);
}

polarchart::~polarchart()
{

}

void polarchart::setAxis()
{
    mypen.setColor(QColor(200,200,200));
    mypen.setWidth(1);

    angularAxis->setRange(0,360);
    angularAxis->setGridLinePen(mypen);
    angularAxis->setTickCount(9);
    angularAxis->setLabelFormat("%d");

    QFont myfont;
    myfont.setPointSize(8);
    radialAxis->setRange(100,1300);
    radialAxis->setLabelsFont(myfont);
    radialAxis->setGridLinePen(mypen);
    radialAxis->setTickCount(7);
    radialAxis->setLabelFormat("%d");
}

void polarchart::updateData(const double *sp, const uint n)
{
    Angle = new double [n];
    for(uint i=0;i<n;i++)
    {
        Angle[i] = sp[i];
    }

    myseries->clear();
    myseries2->clear();
    for(uint i=0;i<n;i++)
    {
        myseries->append(Angle[i],Height[i]);
        myseries2->append(Angle[i],Height[i]);
    }
    delete Angle;
}

void polarchart::setHeight(const double *h, unsigned int n)
{
    Height = new double [n];
    for(unsigned int i=0;i<n;i++)
    {
        Height[i] = h[i];
        qDebug()<<"polarChart Height"<<i <<"= "<<Height[i];
    }
}
