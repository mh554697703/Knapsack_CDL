#ifndef POLARCHART_H
#define POLARCHART_H
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QPolarChart>

QT_CHARTS_BEGIN_NAMESPACE
class QLineSeries;
class QValueAxis;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class polarchart : public QPolarChart
{
    Q_OBJECT
public:
    polarchart();
    virtual ~polarchart();
    void setAxis();
    void updateData(const double *sp, const uint n);
    void setHeight(const double *h, unsigned int n);
private slots:


private:
    QLineSeries *myseries;
    QScatterSeries *myseries2;
    QValueAxis *angularAxis,*radialAxis;
    QPen mypen;
    qreal *Angle = nullptr;
    qreal *Height = nullptr;
};

#endif // POLARCHART_H
