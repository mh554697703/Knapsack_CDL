#ifndef CHART_H
#define CHART_H
#include <QtCharts/QSplineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <QFont>

QT_CHARTS_BEGIN_NAMESPACE
class QSplineSeries;
class QValueAxis;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class Chart : public QChart
{
    Q_OBJECT

public:
    Chart(QGraphicsItem *parent = nullptr, Qt::WindowFlags wFlags = nullptr);
    virtual ~Chart();
    void setChart();
    void updateData(const double *sp, const uint n);
    void setHeight(const double *h, unsigned int n);
private slots:


private:
    QSplineSeries *m_series;
    QStringList m_titles;
    QValueAxis *m_axisX,*m_axisY;
    qreal m_step;            //æÕ «double–Õ

    qreal *X = nullptr;
    qreal *Y = nullptr;
    QPen mypen;
    QFont linechartFont;
};

#endif // CHART_H
