#ifndef BARCHART_H
#define BARCHART_H
#include <QtCharts/QChart>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QHorizontalStackedBarSeries>
#include <QFont>

QT_CHARTS_USE_NAMESPACE


class barchart : public QChart
{
    Q_OBJECT

public:
    barchart();
    virtual ~barchart();
    void setAxis(const double *h, unsigned int n);
    void updateData(const double *sp, const unsigned int n);

private slots:

private:
    QBarSet *barset;
    QHorizontalStackedBarSeries *barseries;
    QBarCategoryAxis *axis;
    double Height[6] = {200,300,400,500,600,700};
    qreal *Velocity;
    QFont barchartFont;
};

#endif // BARCHART_H
