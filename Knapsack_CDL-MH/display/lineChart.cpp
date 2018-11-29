#include "lineChart.h"

#include <QtCharts/QAbstractAxis>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QValueAxis>
#include <QtCore/QRandomGenerator>
#include <QtCore/QDebug>

Chart::Chart(QGraphicsItem *parent, Qt::WindowFlags wFlags):
    QChart(QChart::ChartTypeCartesian, parent, wFlags),
    m_series(nullptr),
  m_axisX(new QValueAxis),
  m_axisY(new QValueAxis),
  m_step(0)
{
    setChart();
    m_series = new QSplineSeries;
    mypen.setColor(QColor(0,220,255));
    mypen.setWidth(4);
    m_series->setPen(mypen);
    addSeries(m_series);
    legend()->hide();
    setAxisX(m_axisX,m_series);
    setAxisY(m_axisY,m_series);
}

Chart::~Chart()
{

}

void Chart::setChart()
{  
    mypen.setColor(QColor(0,0,0));
    mypen.setWidthF(0.5);
    linechartFont.setBold(true);
    linechartFont.setPointSize(14);

    m_axisX->setRange(0,20);
    m_axisX->setLabelFormat("%i");
    m_axisX->setTitleText(QString::fromLocal8Bit("风速 (m/s)"));
    m_axisX->setTitleFont(linechartFont);
    m_axisX->setGridLinePen(mypen);
    m_axisX->setMinorTickCount(4);

    m_axisY->setRange(100,1300);
    m_axisY->setLabelFormat("%i");
    m_axisY->setTitleText(QString::fromLocal8Bit("高度 (m)"));
    m_axisY->setTitleFont(linechartFont);
    m_axisY->setGridLinePen(mypen);
    m_axisY->setTickCount(13);
}

void Chart::updateData(const double *sp, const uint n)
{
    X = new double [n];

    for(uint i=0;i<n;i++)
    {
        X[i] = sp[i];
    }
    m_series->clear();
    for(uint i=0;i<n;i++)
    {
        m_series->append(X[i],Y[i]);
    }
    delete X;
}

void Chart::setHeight(const double *h, unsigned int n)
{
    Y = new double [n];
    for(unsigned int i=0;i<n;i++)
    {
        Y[i] = h[i];

        qDebug()<<"lineChart Y"<<i <<"= "<<Y[i];
    }
}
