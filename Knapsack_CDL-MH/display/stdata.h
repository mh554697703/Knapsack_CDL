#ifndef STDATA_H
#define STDATA_H

#include <QObject>
#include <qwt_raster_data.h>
#include <qwt_matrix_raster_data.h>
#include <QVector>

class STData : public QwtMatrixRasterData
{
//    Q_OBJECT
public:
    STData( );
    ~STData();

    void setdata(const double *s);
    void setSize(uint nLayers);
    void setHeightsLabel(double minHeight, double maxHeight);
private:
    uint n_layer;
    double *updatedata;
    QVector<double> displaydata;

};

#endif // STDATA_H
