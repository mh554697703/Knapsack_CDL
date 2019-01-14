#include "lineChart.h"

#include <QtCharts/QAbstractAxis>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QValueAxis>
#include <QtCore/QRandomGenerator>
#include <QtCore/QDebug>

LineChart::LineChart(QGraphicsItem *parent, Qt::WindowFlags wFlags):
    QChart(QChart::ChartTypeCartesian, parent, wFlags),
    m_series(nullptr),
  m_axisX(new QValueAxis),
  m_axisY(new QValueAxis),
  m_step(0)
{
    setChart();
    m_series = new QLineSeries;
    mypen.setColor(QColor(0,220,255));
    mypen.setWidth(4);
    m_series->setPen(mypen);

    myseries2 = new QScatterSeries;
    myseries2->setColor(QColor(0,100,250));
    myseries2->setMarkerSize(8);
    addSeries(m_series);
    addSeries(myseries2);
    legend()->hide();
    setAxisX(m_axisX,m_series);
    setAxisY(m_axisY,m_series);
    myseries2->attachAxis(m_axisX);
    myseries2->attachAxis(m_axisY);
}

LineChart::~LineChart()
{

}

void LineChart::setChart()
{  
    mypen.setColor(QColor(0,0,0));
    mypen.setWidthF(0.5);
    linechartFont.setFamily("Microsoft Yahei");
    linechartFont.setPointSize(10);
    linechartFont.setBold(true);

    m_axisX->setRange(0,20);
    m_axisX->setLabelFormat("%i");
//    m_axisX->setTitleText(QString::fromLocal8Bit("风速 (m/s)"));
    m_axisX->setTitleFont(linechartFont);
    m_axisX->setLabelsFont(linechartFont);
    m_axisX->setGridLinePen(mypen);
    m_axisX->setMinorTickCount(4);

    m_axisY->setRange(100,1300);
    m_axisY->setLabelFormat("%i");
//    m_axisY->setTitleText(QString::fromLocal8Bit("高度 (m)"));
    m_axisY->setTitleFont(linechartFont);
    m_axisY->setLabelsFont(linechartFont);
    m_axisY->setGridLinePen(mypen);
    m_axisY->setTickCount(13);
}

void LineChart::updateData(const double *sp, const uint n)
{
    X = new double [n];

    for(uint i=0;i<n;i++)
    {
        X[i] = sp[i];
    }
    m_series->clear();
    myseries2->clear();
    for(uint i=0;i<n;i++)
    {
        m_series->append(X[i],Y[i]);
        myseries2->append(X[i],Y[i]);
    }
    QList<int> list;
    for(int i=0;i<n;i++)
        list.push_back(X[i]);
    qSort(list.begin(), list.end());
    int Xmin=list.first();
    int Xmax=list.last();
    if(Xmin<0)
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
    if(Xmin == 0)
    {
        int count=(Xmax-Xmin)/5+1;
        m_axisX->setRange(Xmin,Xmax);
        m_axisX->setTickCount(count);
    }
    else
    {
        int count=(max*2)/5+1;
        m_axisX->setRange(-max,max);
        m_axisX->setTickCount(count);
    }

    delete X;
}

void LineChart::setHeight(const double *h, unsigned int n)
{
    Y = new double [n];
    for(unsigned int i=0;i<n;i++)
    {
        Y[i] = h[i];

        qDebug()<<"lineChart Y"<<i <<"= "<<Y[i];
    }
}
